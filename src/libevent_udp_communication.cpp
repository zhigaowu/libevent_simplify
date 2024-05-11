
#include "libevent_communication.h"

#include <event2/util.h>

#include <cstring>
#include "libevent_udp_communication.h"

namespace io_simplify {

    namespace libevent {

        UdpCommunication::UdpCommunication(struct event_base *evbase, short flags)
            : _evbase(evbase)
            , _flags(flags)

            , _evevent(nullptr)

            , _callback_read_ready()
        {
        }

        UdpCommunication::~UdpCommunication()
        {
            if (_evevent)
            {
                event_free(_evevent);
            }
        }

        void UdpCommunication::callbackReadReady(evutil_socket_t fd, short what, void *ctx)
        {
            UdpCommunication* communication = (UdpCommunication*)ctx;

            struct sockaddr sa;
            socklen_t socklen = sizeof(sa);

            communication->_callback_read_ready(fd, what, &sa, socklen);
        }

        void UdpCommunication::BindReadCallback(const CallbackUdpReadReady &callback_read_ready)
        {
            _callback_read_ready = callback_read_ready;
        }

        size_t UdpCommunication::RecvFrom(evutil_socket_t fd, void* data, size_t size, int flags, struct sockaddr* sa, socklen_t socklen)
        {
            size_t read_size = 0;
            if (_evevent)
            {
                ssize_t res = recvfrom(fd, data, size, flags, sa, &socklen);
                if (res > 0)
                {
                    read_size = res;
                }
                
            }
            return read_size;
        }

        int UdpCommunication::SendTo(evutil_socket_t fd, void* data, size_t size, int flags, struct sockaddr* sa, socklen_t socklen)
        {
            if (_evevent)
            {
                return sendto(fd, data, size, flags, sa, socklen);;
            }
            return -1;
        }

        int UdpCommunication::Connect(const Endpoint& endpoint)
        {
            int res = 0;

            do 
            {
                // set socket information
                struct sockaddr_in client_in;
                memset(&client_in, 0, sizeof(client_in));

                client_in.sin_family = AF_INET;

                if ((res = evutil_inet_pton(client_in.sin_family, endpoint.address.c_str(), &(client_in.sin_addr))) <= 0) 
                {
                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }
                client_in.sin_port = htons(endpoint.port);

                // create socket
                evutil_socket_t udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
                if (udp_socket < 0)
                {
                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }

                do
                {
                    if ((res = evutil_make_socket_nonblocking(udp_socket)) < 0) 
                    {
                        break;
                    }

                    if ((res = evutil_make_listen_socket_reuseable_port(udp_socket)) < 0) 
                    {
                        break;
                    }
                    
                    if ((res = bind(udp_socket, (struct sockaddr*)&client_in, sizeof(client_in))) < 0) 
                    {
                        break;
                    }

                    _evevent = event_new(_evbase, 
                                        udp_socket, 
                                        _flags, 
                                        callbackReadReady,
                                        this);

                    if (!_evevent) 
                    {
                        res = -1;
                        break;
                    }
                } while (false);
                
                if (res < 0)
                {
                    evutil_closesocket(udp_socket);

                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }

                event_add(_evevent, nullptr);

            } while (false);

            return res;
        }
    }
}

