
#include "libevent_tcp_listener.h"

#include <event2/util.h>

#include <cstring>

namespace io_simplify {

    namespace libevent {

        TcpListener::TcpListener()
            : Listener()

            , _evlistener(nullptr)
        {
        }

        TcpListener::~TcpListener()
        {
            if (_evlistener)
            {
                evconnlistener_free(_evlistener);
            }
        }

        void TcpListener::callbackListenerErrorOccurred(struct evconnlistener *evlistener, void *ctx)
        {
            TcpListener* tcplistener = (TcpListener*)ctx;
            
            struct event_base *base = evconnlistener_get_base(evlistener);

            if (tcplistener->_callback_listener_error_occurred)
            {
                int err = EVUTIL_SOCKET_ERROR();
                tcplistener->_callback_listener_error_occurred(err, evutil_socket_error_to_string(err));
            }

            event_base_loopexit(base, nullptr);
        }

        void TcpListener::callbackConnectionAccepted(struct evconnlistener *evlistener, evutil_socket_t fd,
                                                     struct sockaddr *sa, int socklen, void *ctx)
        {
            TcpListener* tcplistener = (TcpListener*)ctx;

            struct event_base *base = evconnlistener_get_base(evlistener);

            tcplistener->_callback_connection_ready(base, fd, sa, socklen);
        }

        int TcpListener::BindEndpoint(const EventBase& event_base, const Endpoint& endpoint, const CallbackConnectionReady& callback_connection_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred)
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

                // set input parameters
                _evlistener = evconnlistener_new_bind(event_base.GetHandle(), 
                                                TcpListener::callbackConnectionAccepted, 
                                                this,
                                                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 
                                                -1,
                                                (struct sockaddr*)(&server_in),
                                                sizeof(server_in));

                if (!_evlistener) 
                {
                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }

                _callback_connection_ready = callback_connection_ready;
                _callback_listener_error_occurred = callback_listener_error_occurred;

                evconnlistener_set_error_cb(_evlistener, callbackListenerErrorOccurred);

            } while (false);

            return res;
        }
    }
}

