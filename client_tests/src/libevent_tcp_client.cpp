
#include "libevent_tcp_client.h"

#include <event2/util.h>

#include <string.h>

#include <iostream>

namespace client {

    namespace libevent {

        TcpClient::TcpClient(ClientServer* client_server)
            : _client_server(client_server)

            , _connected_callback()
            , _disconnected_callback()
        {
        }

        TcpClient::~TcpClient()
        {
        }

        int32_t TcpClient::Connect(const std::string& address, 
                            uint16_t port, 
                            const ConnectedCallback& connected_callback, 
                            const timeval& connect_timeout, 
                            const TimeoutCallback& connect_timeout_callback, 
                            const DisconnectedCallback& disconnected_callback, 
                            std::string& error)
        {
            int res = 0;

            io_simplify::libevent::TcpConnection* connection = _client_server->Generate();

            do
            {
                // failed to initialize connection
                if(!connection) 
                {
                    error = "initiailize connection handle failed";
                    res = -1;
                    break;
                }

                if ((res = connection->Connect(io_simplify::Endpoint{ address, port})) < 0)
                {
                    error = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
                    break;
                }

                _connected_callback = connected_callback;
                _disconnected_callback = disconnected_callback;

                void* timer = nullptr;
                res = _client_server->GetEventServer()->AddTimer(&connect_timeout, 
                    [this, connection, connect_timeout_callback] () mutable {
                        // delete connection handle
                        delete connection;
                        connection = nullptr;

                        connect_timeout_callback();
                    },
                    &timer);

                if (res < 0)
                {
                    error = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
                    break;
                }

                // connection disconnected event is activated with
                if ((res = connection->EnableRead()) < 0)
                {
                    error = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
                    break;
                }

                connection->SetConnectionCallback([this, connection, timer] (struct bufferevent*, short what) {
                    if (what & BEV_EVENT_CONNECTED) 
                    {
                        // free connection timeout timer
                        _client_server->GetEventServer()->DeleteTimer(timer);

                        // put the connection into management
                        _client_server->AddConnectionOf(this, connection);

                        _connected_callback();
                    }
                    else
                    {
                        // free the connection from management
                        _client_server->DeleteConnectionOf(this);
                        
                        _disconnected_callback();
                    }
                });
            } while (false);

            if (res < 0)
            {
                // delete connection handle
                delete connection;
                connection = nullptr;
            }

            return res;
        }

        void TcpClient::Disconnect()
        {
            _client_server->GetInvoker()->Async([this] () {
                _client_server->DeleteConnectionOf(this);
            }, 0);
        }

        void TcpClient::Write(const std::vector<uint8_t>& data, 
            const WriteCallback& write_callback, 
            const timeval& write_timeout, 
            const TimeoutCallback& write_timeout_callback)
        {
            _client_server->GetInvoker()->Async([this, data, write_callback, write_timeout, write_timeout_callback] () {
                io_simplify::libevent::TcpConnection* connection = _client_server->GetConnectionOf(this);
                if (connection)
                {
                    int32_t res = 0;
                    do
                    {
                        res = connection->SetReadWriteCallback(
                            EV_WRITE | EV_READ,

                            nullptr,

                            [this, write_callback](struct bufferevent*) mutable {
                                write_callback(0, "");
                            },

                            [this, write_callback, write_timeout_callback] (struct bufferevent*, short what) {
                                do
                                {
                                    if (what & BEV_EVENT_TIMEOUT)
                                    {
                                        write_timeout_callback();
                                        break;
                                    }

                                    if (what & BEV_EVENT_ERROR && what & BEV_EVENT_WRITING)
                                    {
                                        int e = EVUTIL_SOCKET_ERROR();
                                        write_callback(e, evutil_socket_error_to_string(e));
                                        break;
                                    }

                                    _disconnected_callback();
                                    _client_server->DeleteConnectionOf(this);
                                } while (false);
                        });

                        if (res < 0)
                        {
                            break;
                        }

                        if ((res = connection->SetTimetout(nullptr, &write_timeout)) < 0)
                        {
                            break;
                        }

                        if ((res = connection->Write(data.data(), data.size())) < 0)
                        {
                            break;
                        }
                    } while (false);

                    if (res < 0)
                    {
                        int e = EVUTIL_SOCKET_ERROR();
                        write_callback(e, evutil_socket_error_to_string(e));
                    }
                }
                else
                {
                    write_callback(-1, "connection is not established");
                }
            }, 0);
        }

        void TcpClient::Write(const std::vector<uint8_t> &data, 
            const timeval& write_timeout, 
            const TimeoutCallback& write_timeout_callback,
            const ReadCallback &read_callback, 
            const timeval &read_timeout, 
            const TimeoutCallback &read_timeout_callback)
        {
            _client_server->GetInvoker()->Async([this, data, write_timeout, write_timeout_callback, read_callback, read_timeout, read_timeout_callback] () {
                io_simplify::libevent::TcpConnection* connection = _client_server->GetConnectionOf(this);
                if (connection)
                {
                    int32_t res = 0;
                    do
                    {
                        res = connection->SetReadWriteCallback(
                            EV_WRITE | EV_READ,

                            [this, connection, write_timeout, read_callback](struct bufferevent*) mutable {
                                // reset read timeout to forever, wait for disconnection
                                connection->SetTimetout(nullptr, &write_timeout);

                                // start to read data from the buffer
                                read_callback(connection, 0, "");
                            },

                            [this, connection, read_callback](struct bufferevent*) mutable {
                                
                            },

                            [this, connection, write_timeout, write_timeout_callback, read_callback, read_timeout_callback] (struct bufferevent*, short what) {
                                
                                // reset read timeout to forever, wait for disconnection
                                connection->SetTimetout(nullptr, &write_timeout);

                                do
                                {
                                    if (what & BEV_EVENT_TIMEOUT)
                                    {
                                        if (what & BEV_EVENT_WRITING)
                                        {
                                            write_timeout_callback();
                                        }
                                        else if (what & BEV_EVENT_READING)
                                        {
                                            read_timeout_callback();
                                        }
                                        break;
                                    }

                                    if ((what & BEV_EVENT_ERROR) && (what & (BEV_EVENT_WRITING | BEV_EVENT_READING)))
                                    {
                                        int e = EVUTIL_SOCKET_ERROR();
                                        read_callback(nullptr, e, evutil_socket_error_to_string(e));
                                        break;
                                    }

                                    _disconnected_callback();
                                    _client_server->DeleteConnectionOf(this);
                                } while (false);
                        });

                        if (res < 0)
                        {
                            break;
                        }

                        if ((res = connection->SetTimetout(&read_timeout, &write_timeout)) < 0)
                        {
                            break;
                        }

                        if ((res = connection->Write(data.data(), data.size())) < 0)
                        {
                            break;
                        }
                    } while (false);

                    if (res < 0)
                    {
                        int e = EVUTIL_SOCKET_ERROR();
                        read_callback(connection, e, evutil_socket_error_to_string(e));
                    }
                }
                else
                {
                    read_callback(connection, -1, "connection is not established");
                }
            }, 0);
        }

    }

}
