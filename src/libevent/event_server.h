/*
 * ==========================================================================
 *
 *       Filename:  event_server.h
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

#include "libevent_base.h"

#include <thread>
#include <functional>

namespace io_simplify {
    namespace libevent {

        using CallbackTimeout = std::function<void()>;

        class EventServer {
            std::thread _server;

        protected:
            EventBase _base;

        private:
            void serve()
            {
                _base.Loop();
            }

        private:
            struct Timer {
                struct event* handle = nullptr;
                short events = 0;
                CallbackTimeout callback = nullptr;
            };

            static void timeout_callback(evutil_socket_t, short, void* cb_data)
            {
                Timer* timer = (Timer*)cb_data;
                
                timer->callback();

                if ((timer->events & EV_PERSIST) == 0)
                {
                    event_free(timer->handle);

                    delete timer;
                }
            }

        public:
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

            EventBase& GetBase()
            {
                return _base;
            }

            int Serve(struct event* user_event, const timeval* tval = nullptr)
            {
                int res = -1;

                do
                {
                    if (_server.joinable())
                    {
                        break;
                    }

                    if (user_event)
                    {
                        if ((res = event_add(user_event, tval)) < 0)
                        {
                            break;
                        }
                    }
                    
                    _server = std::thread(&EventServer::serve, this);

                    res = 0;
                } while (false);

                return res;
            }

            int AddTimer(const timeval* tval, const CallbackTimeout& callback_timeout, void** timer)
            {
                int res = 0;
                Timer* new_timer = new Timer{nullptr, EV_TIMEOUT, callback_timeout};

                if (new_timer)
                {
                    new_timer->handle = event_new(_base.GetHandle(), -1, new_timer->events, timeout_callback, new_timer);
                    
                    if ((res = event_add(new_timer->handle, tval)) < 0)
                    {
                        event_free(new_timer->handle);

                        delete new_timer;
                        new_timer = nullptr;
                    }
                }

                if (timer)
                {
                    *timer = new_timer;
                }

                return res;
            }

            int AddPersistTimer(const timeval* tval, const CallbackTimeout& callback_timeout, void** timer)
            {
                int res = 0;
                Timer* new_timer = new Timer{nullptr, EV_TIMEOUT | EV_PERSIST, callback_timeout};

                if (new_timer)
                {
                    new_timer->handle = event_new(_base.GetHandle(), -1, new_timer->events, timeout_callback, new_timer);
                    
                    if ((res = event_add(new_timer->handle, tval)) < 0)
                    {
                        event_free(new_timer->handle);

                        delete new_timer;
                        new_timer = nullptr;
                    }
                }

                if (*timer)
                {
                    *timer = new_timer;
                }

                return res;
            }

            void DeleteTimer(void* timer)
            {
                Timer* delete_timer = (Timer*)timer;

                if (delete_timer)
                {
                    event_free(delete_timer->handle);

                    delete delete_timer;
                }
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
