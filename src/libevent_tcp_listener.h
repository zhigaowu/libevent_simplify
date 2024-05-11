/*
 * ==========================================================================
 *
 *       Filename:  libevent_tcp_listener.h
 *
 *    Description:  tcp listener logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_TCP_LISTENER_H
#define IO_SIMPLIFY_LIBEVENT_TCP_LISTENER_H

#include "libevent_listener.h"

#include <event2/listener.h>

namespace io_simplify {

    namespace libevent {

        class TcpListener : public Listener {
            struct evconnlistener* _evlistener;

        private:
            static void callbackListenerErrorOccurred(struct evconnlistener *evlistener, void *ctx);

            static void callbackConnectionAccepted(struct evconnlistener *evlistener, evutil_socket_t fd,
                                     struct sockaddr *sa, int socklen, void *ctx);

        public:
            TcpListener();
            ~TcpListener();

            int BindEndpoint(const EventBase& event_base, const Endpoint& endpoint, const CallbackConnectionReady& callback_connection_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred) override;

        public:
            TcpListener(TcpListener&&) = delete;
            TcpListener& operator=(TcpListener&&) = delete;

            TcpListener(const TcpListener&) = delete;
            TcpListener& operator=(const TcpListener&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_TCP_LISTENER_H
