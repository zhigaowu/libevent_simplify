
#include "libevent_listener.h"

namespace io_simplify {

    namespace libevent {

        Listener::Listener()
            : _callback_connection_ready()
            , _callback_listener_error_occurred()
        {
        }

        Listener::~Listener()
        {
        }
    }
}

