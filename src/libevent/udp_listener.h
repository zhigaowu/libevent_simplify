/*
 * ==========================================================================
 *
 *       Filename:  udp_listener.h
 *
 *    Description:  udp listener logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_UDP_LISTENER_H
#define IO_SIMPLIFY_LIBEVENT_UDP_LISTENER_H

#include "listener.h"

#include <event2/listener.h>

namespace io_simplify {

    namespace libevent {

        class UdpListener : public Listener {
            struct event* _evevent;

        private:
            static void callbackReadReady(evutil_socket_t fd, short what, void* ctx);

        public:
            UdpListener();
            ~UdpListener();

            int BindEndpoint(const EventBase& event_base, const Endpoint& endpoint, const CallbackConnectionReady& callback_connection_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred) override;

        public:
            UdpListener(UdpListener&&) = delete;
            UdpListener& operator=(UdpListener&&) = delete;

            UdpListener(const UdpListener&) = delete;
            UdpListener& operator=(const UdpListener&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_UDP_LISTENER_H
