/*
 * ==========================================================================
 *
 *       Filename:  test_tcp_server.cpp
 *
 *    Description:  tcp test cases
 *
 *        Version:  1.0
 *        Created:  2024-12-06 09:22:54
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#include "libevent/udp_listener.h"

#include "libevent/socket_server.h"

#include "common.h"

int test_udp_server(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv)
{
    io_simplify::libevent::SocketServer<io_simplify::libevent::UdpListener> server(base_config);
    
    char read_buffer[128] = {0};
    int res = server.Bind(
        io_simplify::Endpoint{argv[ADDRESS_INDEX], (uint16_t)atoi(argv[PORT_INDEX])},
        [&read_buffer](event_base* evbase, evutil_socket_t fd, struct sockaddr *sa, socklen_t socklen) {
            char address[64] = {0};

            int res = recvfrom(fd, read_buffer, sizeof(read_buffer), 0, sa, &socklen);

            const struct sockaddr_in* sav4 = (const struct sockaddr_in*)(sa);
            evutil_inet_ntop(sa->sa_family, &(sav4->sin_addr), address, socklen);
            uint16_t port = ntohs(sav4->sin_port);

            if (res > 0)
            {
                read_buffer[res] = 0;

                std::cout << "data received: [" << read_buffer  << "] from: " << address << ":" << port << std::endl;

                static char data_to_send[128] = {0};
                sprintf(data_to_send, "you said: %s", read_buffer);
                sendto(fd, data_to_send, res + 12, 0, sa, socklen);
            }
            else
            {
                std::cout << "data received: " << res  << "from: " << address << ":" << port << std::endl;
            }
        }, 
        [] (int err, const char* desc) {
            std::cerr << "Listener error: " << desc << std::endl;
        });
    
    if (res >= 0)
    {
        std::cerr << "begin serve: " << evutil_socket_error_to_string(res) << std::endl;
        res =  server.Serve();
        std::cerr << "end serve: " << evutil_socket_error_to_string(res) << std::endl;
    }
    else
    {
        std::cout << "ready to serve ... " << std::endl;

        std::cerr << "Bind endpoint error: " << evutil_socket_error_to_string(res) << std::endl;
    }

    return res;
}
