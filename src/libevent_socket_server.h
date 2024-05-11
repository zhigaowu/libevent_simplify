/*
 * ==========================================================================
 *
 *       Filename:  libevent_socket_server.h
 *
 *    Description:  socket server common logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_SOCKET_SERVER_H
#define IO_SIMPLIFY_LIBEVENT_SOCKET_SERVER_H

#include "libevent_objects.h"

namespace io_simplify {
    namespace libevent {

        template<typename ListenerType>
        class SocketServer {
            EventBase _base;
            ListenerType _listener;

        public:
            SocketServer()
                : _base(BaseConfig().NewBase())
                , _listener()
            {

            }

            explicit SocketServer(const BaseConfig& base_config)
                : _base(base_config.NewBase())
                , _listener()
            {
            }

            ~SocketServer()
            {
            }

            int Bind(const Endpoint& endpoint, const CallbackConnectionReady& callback_bufferevent_ready, const CallbackListenerErrorOccurred& callback_listener_error_occurred)
            {
                return _listener.BindEndpoint(_base, endpoint, callback_bufferevent_ready, callback_listener_error_occurred);
            }

            int Serve()
            {
                return _base.Loop();
            }

            int Exit()
            {
                return _base.BreakLoop();
            }

        public:
            SocketServer(SocketServer&&) = delete;
            SocketServer& operator=(SocketServer&&) = delete;

            SocketServer(const SocketServer&) = delete;
            SocketServer& operator=(const SocketServer&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_SOCKET_SERVER_H
