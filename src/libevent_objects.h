/*
 * ==========================================================================
 *
 *       Filename:  libevent_objects.h
 *
 *    Description:  libevent struct to object
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_OBJECTS_H
#define IO_SIMPLIFY_LIBEVENT_OBJECTS_H

#include <event2/event.h>

#include <string>

namespace io_simplify {

    struct Endpoint {
        std::string address = {};
        uint16_t port = 0;
    };

    namespace libevent {

        /*
            debug: Generally, these checks cause code that would otherwise crash mysteriously to fail earlier with an assertion failure. Note that this method MUST be called before any events or event_bases have been created.
                    Debug mode can currently catch the following errors: An event is re-assigned while it is added Any function is called on a non-assigned event
                    Note that debugging mode uses memory to track every event that has been initialized (via event_assign, event_set, or event_new) but not yet released (via event_free or event_debug_unassign). 
                    If you want to use debug mode, and you find yourself running out of memory, you will need to use event_debug_unassign to explicitly stop tracking events that are no longer considered set-up.

            verbose_switch: "EVENT_DBG_ALL" to turn debugging logs on, or "EVENT_DBG_NONE" to turn debugging logs off.
        */
        int Initialize(bool uing_threads = true, bool debug = false, ev_uint32_t verbose_option = EVENT_DBG_NONE);

        class EventBase {
            struct event_base* _evbase;

        public:
            explicit EventBase(event_base* evbase = nullptr)
                : _evbase(evbase)
            {                
            }

            ~EventBase()
            {
                if (_evbase)
                {
                    /*
                        Deallocate all memory associated with an event_base, and free the base.
                        Note that this function will not close any fds or free any memory passed to event_new as the argument to callback.
                        If there are any pending finalizer callbacks, this function will invoke them.
                    */
                    event_base_free(_evbase);
                }
            }

            int Loop()
            {
                return event_base_dispatch(_evbase);
            }

            int BreakLoop()
            {
                return event_base_loopbreak(_evbase);
            }

            event_base* GetHandle() { return _evbase; }

            event_base* GetHandle() const { return _evbase; }

        public:
            EventBase(EventBase&&) = delete;
            EventBase& operator=(EventBase&&) = delete;

            EventBase(const EventBase&) = delete;
            EventBase& operator=(const EventBase&) = delete;
        };

        class BaseConfig {
            struct event_config* _evconfig;

        public:
            static const char** SupportedMethods()
            {
                return event_get_supported_methods();
            }

        public:
            BaseConfig()
                : _evconfig(event_config_new())
            {                
            }

            ~BaseConfig()
            {
                if (_evconfig)
                {
                    /*
                        Deallocates all memory associated with an event configuration object.
                    */
                    event_config_free(_evconfig);
                }
            }

            /*
                method: const char **event_get_supported_methods(void)
            */
            int AvoidMethod(const char* method)
            {
                return event_config_avoid_method(_evconfig, method);
            }

            /*
                features: enum event_method_feature
            */
            int RequireFeatures(int features)
            {
                return event_config_require_features(_evconfig, features);
            }

            /*
                flag: enum event_base_config_flag
            */
            int SetFlag(int flag)
            {
                return event_config_set_flag(_evconfig, flag);
            }

            event_base* NewBase()
            {
                return event_base_new_with_config(_evconfig);
            }

            event_base* NewBase() const
            {
                return event_base_new_with_config(_evconfig);
            }

        public:
            BaseConfig(BaseConfig&&);
            BaseConfig& operator=(BaseConfig&&);

            BaseConfig(const BaseConfig&);
            BaseConfig& operator=(const BaseConfig&);
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_OBJECTS_H
