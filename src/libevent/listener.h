/*
 * ==========================================================================
 *
 *       Filename:  listener.h
 *
 *    Description:  server common logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_LISTENER_H
#define IO_SIMPLIFY_LIBEVENT_LISTENER_H

#include "libevent_base.h"

#include <functional>

namespace io_simplify {
    
    namespace libevent {
        using CallbackConnectionReady = std::function<void(struct event_base*, evutil_socket_t, struct sockaddr*, socklen_t)>; // evutil_socket_t fd, struct sockaddr *sa, int socklen
        using CallbackListenerErrorOccurred = std::function<void(int, const char*)>; 

        class Listener {
        protected:
            CallbackConnectionReady _callback_connection_ready;
            CallbackListenerErrorOccurred _callback_listener_error_occurred;

        public:
            Listener();
            virtual ~Listener();

            virtual int BindEndpoint(const EventBase& event_base, const Endpoint& endpoint, const CallbackConnectionReady& callback_connection_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred) = 0;

        public:
            Listener(Listener&&) = delete;
            Listener& operator=(Listener&&) = delete;

            Listener(const Listener&) = delete;
            Listener& operator=(const Listener&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_LISTENER_H
