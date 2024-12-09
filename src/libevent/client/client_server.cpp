
#include "client_server.h"

#include <event2/util.h>


namespace io_simplify {

    namespace libevent::client {

        ClientServer::ClientServer(const io_simplify::libevent::BaseConfig& base_config, const timeval tval)
                : _event_server(new io_simplify::libevent::EventServer(base_config))
                , _invoker(new io_simplify::libevent::AsyncInvoker(_event_server->GetBase().GetHandle()))
        {
            _event_server->Serve(_invoker->GetHandle(), &tval);
        }

        ClientServer::~ClientServer()
        {
            if (_invoker)
            {
                delete _invoker;
                _invoker = nullptr;
            }

            if (_event_server)
            {
                _event_server->Exit();

                delete _event_server;
                _event_server = nullptr;
            }
        }

        io_simplify::libevent::AsyncInvoker *ClientServer::GetInvoker()
        {
            return _invoker;
        }

        io_simplify::libevent::EventServer *ClientServer::GetEventServer()
        {
            return _event_server;
        }
    }
}
