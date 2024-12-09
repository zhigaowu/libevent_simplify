
#include "tcp_client.h"

#include <event2/util.h>

#include <string.h>

#include <iostream>

namespace io_simplify {

    namespace libevent::client {

        TcpClient::TcpClient(ClientServer* client_server)
            : _client_server(client_server)

            , _connection(nullptr)

            , _read_callback([](io_simplify::libevent::TcpConnection*) {})
            , _disconnected_callback()
        {
        }

        TcpClient::~TcpClient()
        {
        }

        void TcpClient::Connect(const io_simplify::Endpoint& endpoint, 
                const CallbackConnect& connect_callback,
                const CallbackDisconnected& disconnected_callback, 
                const timeval& connect_timeout, 
                const CallbackRead& read_callback,
                struct ssl_st *ssl, 
                enum bufferevent_ssl_state ssl_state)
        {
            // put all net operation into event loop
            _client_server->GetInvoker()->Async([
                this, 
                endpoint, 
                connect_callback,
                disconnected_callback, 
                connect_timeout,
                read_callback,
                ssl,
                ssl_state
            ] () {
                if (_connection)
                {
                    delete _connection;
                    _connection = nullptr;
                }

                if (ssl)
                {
                    _connection = new io_simplify::libevent::TcpConnection(_client_server->GetEventServer()->GetBase().GetHandle(), -1, ssl, ssl_state, BEV_OPT_CLOSE_ON_FREE);
                }
                else
                {
                    _connection = new io_simplify::libevent::TcpConnection(_client_server->GetEventServer()->GetBase().GetHandle(), -1, BEV_OPT_CLOSE_ON_FREE);
                }

                void* timer = nullptr;
                int32_t res = _client_server->GetEventServer()->AddTimer(&connect_timeout, 
                    [this, connect_callback] () {
                        connect_callback(-1, "connect timeout");
                    },
                    &timer);

                if (res < 0)
                {
                    connect_callback(res, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
                    return;
                }

                if (read_callback)
                {
                    _read_callback = read_callback;
                }

                _disconnected_callback = [this, disconnected_callback] () {
                        Disconnect();

                        disconnected_callback();
                    };

                io_simplify::libevent::CallbackConnectionEventOccurred callback_connection_event_occurred = [
                    this, 
                    timer, 
                    connect_callback
                ] (struct bufferevent*, short what) {
                    if (what == BEV_EVENT_CONNECTED) 
                    {
                        // free connection timeout timer
                        _client_server->GetEventServer()->DeleteTimer(timer);

                        connect_callback(0, "");
                    }
                    else if (what == BEV_EVENT_ERROR) 
                    {
                        // don't delete, wait timeout
                        connect_callback(-1, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
                    }
                    else
                    {
                        _disconnected_callback();
                    }
                };

                io_simplify::libevent::CallbackConnectionReadReady read_callback = [this] (struct bufferevent*) {
                    _read_callback(_connection);
                };

                if ((res = _connection->SetConnectionCallback(callback_connection_event_occurred, read_callback)) < 0)
                {
                    connect_callback(res, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
                    return;
                }

                if ((res = _connection->Connect(endpoint)) < 0)
                {
                    connect_callback(res, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
                }
            }, 0);
        }

        void TcpClient::Disconnect()
        {
            _client_server->GetInvoker()->Async([
                this
            ] () {
                if (_connection)
                {
                    delete _connection;
                    _connection = nullptr;
                }
            });
        }

        void TcpClient::Write(const std::vector<uint8_t>& data, 
            const CallbackWrite& write_callback, 
            const timeval& write_timeout, 
            const CallbackTimeout& write_timeout_callback)
        {
            _client_server->GetInvoker()->Async([
                this, 
                data,
                write_callback, 
                write_timeout, 
                write_timeout_callback
            ] () {
                if (_connection)
                {
                    int32_t res = 0;
                    do
                    {
                        res = _connection->BindCallback(
                            EV_WRITE | EV_READ,

                            [this] (struct bufferevent*) {
                                _read_callback(_connection);
                            },

                            [this, write_callback](struct bufferevent*) {
                                write_callback(0, "success");
                            },

                            [this, write_callback, write_timeout_callback] (struct bufferevent*, short what) {
                                do
                                {
                                    if (what & BEV_EVENT_TIMEOUT)
                                    {
                                        write_timeout_callback();
                                        break;
                                    }

                                    if ((what & BEV_EVENT_ERROR) && (what & BEV_EVENT_WRITING))
                                    {
                                        int e = EVUTIL_SOCKET_ERROR();
                                        write_callback(e, evutil_socket_error_to_string(e));
                                        break;
                                    }

                                    _disconnected_callback();
                                } while (false);
                        });

                        if (res < 0)
                        {
                            break;
                        }

                        if ((res = _connection->SetTimetout(nullptr, &write_timeout)) < 0)
                        {
                            break;
                        }

                        if ((res = _connection->Write(data.data(), data.size())) < 0)
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
            const CallbackWrite& write_callback, 
            const timeval& write_timeout, 
            const CallbackTimeout& write_timeout_callback,
            const CallbackRead &read_callback, 
            const timeval &read_timeout, 
            const CallbackTimeout &read_timeout_callback)
        {
            _client_server->GetInvoker()->Async([
                this, 
                data, 
                write_callback, 
                write_timeout, 
                write_timeout_callback, 
                read_callback, 
                read_timeout, 
                read_timeout_callback
            ] () {
                if (_connection)
                {
                    int32_t res = 0;
                    do
                    {
                        // check validity
                        if (read_callback)
                        {
                            _read_callback = read_callback;
                        }

                        res = _connection->BindCallback(
                            EV_WRITE | EV_READ,

                            [this, write_timeout](struct bufferevent*) {
                                _connection->SetTimetout(nullptr, &write_timeout);

                                // start to read data from the buffer
                                _read_callback(_connection);
                            },

                            [this, write_callback](struct bufferevent*) {
                                write_callback(0, "success");
                            },

                            [
                                this, 
                                write_timeout_callback, 
                                read_timeout_callback
                            ] (struct bufferevent*, short what) {
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

                                    _disconnected_callback();
                                } while (false);
                        });

                        if (res < 0)
                        {
                            break;
                        }

                        if ((res = _connection->SetTimetout(&read_timeout, &write_timeout)) < 0)
                        {
                            break;
                        }

                        if ((res = _connection->Write(data.data(), data.size())) < 0)
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

    }

}
