/*
 * ==========================================================================
 *
 *       Filename:  libevent_event_server.h
 *
 *    Description:  event server common logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_EVENT_SERVER_H
#define IO_SIMPLIFY_LIBEVENT_EVENT_SERVER_H

#include "libevent_objects.h"

#include <thread>

namespace io_simplify {
    namespace libevent {

        class EventServer {
            std::thread _server;

        protected:
            EventBase _base;

        private:
            void serve()
            {
                _base.Loop();
            }

        public:
            static void KeepAlive(evutil_socket_t, short, void *)
            {}

            EventServer()
                : _server()
                , _base(BaseConfig().NewBase())
            {
                
            }

            explicit EventServer(const BaseConfig& base_config)
                : _server()
                , _base(base_config.NewBase())
            {
            }

            virtual ~EventServer()
            {
                if (_server.joinable())
                {
                    _server.join();
                }
            }

            EventBase& GetEventBase()
            {
                return _base;
            }

            int Serve(event_callback_fn callback_timeout = KeepAlive, const timeval tval = {1, 0}, void* callback_ctx = nullptr)
            {
                int res = -1;

                do
                {
                    if (_server.joinable())
                    {
                        break;
                    }

                    if (callback_timeout)
                    {
                        struct event* timer = event_new(_base.GetHandle(), -1, EV_TIMEOUT | EV_PERSIST, callback_timeout, callback_ctx);
                        if (!timer)
                        {
                            break;
                        }

                        if ((res = event_add(timer, &tval)) < 0)
                        {
                            break;
                        }
                    }
                    
                    _server = std::thread(&EventServer::serve, this);

                    res = 0;
                } while (false);

                return res;
            }

            int Exit()
            {
                int res = 0;

                if (_server.joinable())
                {
                    res = _base.BreakLoop();
                }

                return res;
            }

        public:
            EventServer(EventServer&&) = delete;
            EventServer& operator=(EventServer&&) = delete;

            EventServer(const EventServer&) = delete;
            EventServer& operator=(const EventServer&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_EVENT_SERVER_H
