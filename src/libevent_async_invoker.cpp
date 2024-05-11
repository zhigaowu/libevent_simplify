

#include <event2/util.h>

#include <cstring>
#include "libevent_async_invoker.h"

namespace io_simplify {

    namespace libevent {

        AsyncInvoker::AsyncInvoker(struct event_base *evbase, short flags)
            : _evevent(event_new(evbase, 
                                    -1, 
                                    flags, 
                                    callbackInvoked,
                                    this))

            , _locker()

            , _callback_to_invoke_list()
        {
            if (_evevent)
            {
                event_add(_evevent, nullptr);
            }
            
        }

        AsyncInvoker::~AsyncInvoker()
        {
            if (_evevent)
            {
                event_free(_evevent);
            }
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

        void AsyncInvoker::Async(const CallbackToInvoke& callback_to_invoke)
        {
            if (callback_to_invoke)
            {
                std::lock_guard<std::mutex> guard(_locker);
                _callback_to_invoke_list.push_back(callback_to_invoke);
            }
            
            event_active(_evevent, 0, 0);
        }
        
    }
}

