/*
 * ==========================================================================
 *
 *       Filename:  libevent_async_invoker.h
 *
 *    Description:  async invoke logic
 *
 *        Version:  1.0
 *        Created:  2024-04-026 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_ASYNC_INVOKER_H
#define IO_SIMPLIFY_LIBEVENT_ASYNC_INVOKER_H

#include "libevent_objects.h"

#include <functional>
#include <list>
#include <mutex>

namespace io_simplify {

    namespace libevent {

        using CallbackToInvoke = std::function<void()>;

        class AsyncInvoker {
            struct event* _evevent;

            std::mutex _locker;

            using CallbackToInvokeList = std::list<CallbackToInvoke>;
            CallbackToInvokeList _callback_to_invoke_list;

        private:
            void invokeCallbacks();

            static void callbackInvoked(evutil_socket_t fd, short what, void* ctx);

        public:
            explicit AsyncInvoker(struct event_base *evbase, short flags = 0);
            ~AsyncInvoker();

            void Async(const CallbackToInvoke& callback_to_invoke);

        public:
            AsyncInvoker() = delete;

            AsyncInvoker(AsyncInvoker&&) = delete;
            AsyncInvoker& operator=(AsyncInvoker&&) = delete;

            AsyncInvoker(const AsyncInvoker&) = delete;
            AsyncInvoker& operator=(const AsyncInvoker&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_ASYNC_INVOKER_H
