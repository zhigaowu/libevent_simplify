
#include "libevent_udp_listener.h"

#include <event2/util.h>

#include <cstring>

namespace io_simplify {

    namespace libevent {

        UdpListener::UdpListener()
            : Listener()

            , _evevent(nullptr)
        {
        }

        UdpListener::~UdpListener()
        {
            if (_evevent)
            {
                event_free(_evevent);
            }
        }

        void UdpListener::callbackReadReady(evutil_socket_t fd, short, void* ctx)
        {
            UdpListener* udplistener = (UdpListener*)ctx;

            struct event_base *base = event_get_base(udplistener->_evevent);

            struct sockaddr client_addr;
            socklen_t client_addr_size = sizeof(client_addr);

            udplistener->_callback_connection_ready(base, fd, &client_addr, client_addr_size);
        }

        int UdpListener::BindEndpoint(const EventBase& event_base, const Endpoint& endpoint, const CallbackConnectionReady& callback_connection_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred)
        {
            int res = 0;

            do 
            {
                // set socket information
                struct sockaddr_in server_in;
                memset(&server_in, 0, sizeof(server_in));

                server_in.sin_family = AF_INET;

                if ((res = evutil_inet_pton(server_in.sin_family, endpoint.address.c_str(), &(server_in.sin_addr))) <= 0) 
                {
                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }
                server_in.sin_port = htons(endpoint.port);

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
                    
                    if ((res = bind(udp_socket, (struct sockaddr*)&server_in, sizeof(server_in))) < 0) 
                    {
                        break;
                    }

                    _evevent = event_new(event_base.GetHandle(), 
                                            udp_socket, 
                                            EV_READ | EV_PERSIST, 
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

                _callback_connection_ready = callback_connection_ready;
                _callback_listener_error_occurred = callback_listener_error_occurred;

            } while (false);

            return res;
        }
    }
}

