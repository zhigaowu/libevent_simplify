

#include <event2/util.h>

#include <cstring>
#include "libevent_async_invoker.h"

namespace io_simplify {

    namespace libevent {

        AsyncInvoker::AsyncInvoker(struct event_base *evbase, short events)
            : _event(event_new(evbase, 
                                    -1, 
                                    events | EV_PERSIST, 
                                    callbackInvoked,
                                    this))

            , _locker()

            , _callback_to_invoke_list()
        {
        }

        AsyncInvoker::~AsyncInvoker()
        {
            if (_event)
            {
                event_free(_event);
            }
        }

        struct event *AsyncInvoker::GetHandle()
        {
            return _event;
        }

        void AsyncInvoker::invokeCallbacks()
        {
            CallbackToInvokeList callback_to_invoke_list;
            do
            {
                std::lock_guard<std::mutex> guard(_locker);
                _callback_to_invoke_list.swap(callback_to_invoke_list);
            } while (false);

            for (CallbackToInvoke& callback_invoke : callback_to_invoke_list)
            {
                callback_invoke();
            }
        }

        void AsyncInvoker::callbackInvoked(evutil_socket_t, short, void *ctx)
        {
            AsyncInvoker* communication = (AsyncInvoker*)ctx;

            communication->invokeCallbacks();
        }

        void AsyncInvoker::Async(const CallbackToInvoke& callback_to_invoke, int what)
        {
            if (callback_to_invoke)
            {
                std::lock_guard<std::mutex> guard(_locker);
                _callback_to_invoke_list.push_back(callback_to_invoke);
            }
            
            event_active(_event, what, 0);
        }
        
    }
}

