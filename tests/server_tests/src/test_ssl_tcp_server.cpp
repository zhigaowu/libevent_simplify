/*
 * ==========================================================================
 *
 *       Filename:  test_ssl_tcp_server.cpp
 *
 *    Description:  ssl tcp test cases
 *
 *        Version:  1.0
 *        Created:  2024-12-06 09:22:54
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#include "libevent/tcp_connection.h"
#include "libevent/tcp_listener.h"

#include "libevent/socket_server.h"

#include "ssl/server_certificate.h"

#include "common.h"

int test_ssl_tcp_server(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv)
{
    io_simplify::ssl::Initialize();

    io_simplify::ssl::ServerCertifcate certificate;

    int res = certificate.Create(argv[CERTIFICATE_FILE_INDEX]);
    if (res < 0)
    {
        std::cerr << "load certificate file failed: " << certificate.GetError() << std::endl;
        return res;
    }

    res = certificate.UseKeyFile(argv[PRIVATE_KEY_FILE_INDEX]);
    if (res < 0)
    {
        std::cerr << "use private key file failed: " << certificate.GetError() << std::endl;
        return res;
    }

    io_simplify::libevent::SocketServer<io_simplify::libevent::TcpListener> server(base_config);
        
    res = server.Bind(
        io_simplify::Endpoint{argv[ADDRESS_INDEX], (uint16_t)atoi(argv[PORT_INDEX])},
        [&base_config, &certificate](event_base* evbase, evutil_socket_t fd, struct sockaddr *sa, socklen_t socklen) {
            
            io_simplify::Endpoint endpoint = io_simplify::FromSockAddress(sa, socklen);
            
            std::cout << "new connection[" << endpoint.address << ":" << endpoint.port << "] arrived" << std::endl;

            io_simplify::libevent::TcpConnection* connection = new io_simplify::libevent::TcpConnection(evbase, fd, certificate.Generate(), BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);

            std::vector<uint8_t> stream_buffer(128, 0);

            int res = connection->BindCallback(
                EV_READ|EV_WRITE,
                [connection, stream_buffer](struct bufferevent* bev) mutable {
                    size_t read_size = connection->Read(stream_buffer.data(), stream_buffer.size());

                    stream_buffer[read_size] = 0;

                    static char data_to_send[128] = {0};
                    sprintf(data_to_send, "you said: %s", (char*)stream_buffer.data());
                    std::cout << "data received[" << (char*)stream_buffer.data() << "]" << std::endl;
                    std::cout << "send data result: " << connection->Write(data_to_send, read_size + 12) << std::endl;
                },
                [](struct bufferevent* bev) {
                    std::cout << "send ready..." << std::endl;
                },
                [connection] (struct bufferevent* bev, short what) {
                    if (what & BEV_EVENT_CONNECTED) 
                    {
                        std::cout << "connection established." << std::endl;
                    }
                    else
                    {
                        if (what & BEV_EVENT_EOF) 
                        {
                            std::cout << "connection closed." << std::endl;
                        }
                        else if (what & BEV_EVENT_ERROR)
                        {
                            std::cerr << "got an error on the connection: " << evutil_socket_error_to_string(errno) << std::endl;
                        }
                        else if (what & BEV_EVENT_TIMEOUT) 
                        {
                            std::cout << "time out." << std::endl;
                        }
                        else
                        {
                            std::cout << "event: " << (int)what << std::endl;
                        }
                        
                        delete connection;
                    }
            });

            if (res < 0)
            {
                std::cerr << "accept connection failed: " << evutil_socket_error_to_string(res) << std::endl;
                delete connection;
            }
        }, 
        [] (int err, const char* desc) {
            std::cerr << "listen socket error: " << desc << std::endl;
        });
    
    if (res >= 0)
    {
        res =  server.Serve();
    }
    else
    {
        std::cerr << "bind endpoint error: " << evutil_socket_error_to_string(res) << std::endl;
    }

    return res;
}

