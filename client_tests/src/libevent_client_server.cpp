
#include "libevent_client_server.h"

#include <event2/util.h>


namespace client {

    namespace libevent {

        ClientServer::ClientServer(const io_simplify::libevent::BaseConfig& base_config, const timeval tval)
                : _event_server(new io_simplify::libevent::EventServer(base_config))
                , _invoker(new io_simplify::libevent::AsyncInvoker(_event_server->GetBase().GetHandle()))

                , _tcp_connections()
        {
            _event_server->Serve(_invoker->GetHandle(), &tval);
        }

        ClientServer::~ClientServer()
        {
            if (_invoker)
            {
                delete _invoker;
            }

            if (_event_server)
            {
                _event_server->Exit();

                delete _event_server;
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

        io_simplify::libevent::TcpConnection *ClientServer::Generate()
        {
            return new io_simplify::libevent::TcpConnection(_event_server->GetBase().GetHandle(), -1, BEV_OPT_CLOSE_ON_FREE);
        }

        void ClientServer::AddConnectionOf(const void *owner, io_simplify::libevent::TcpConnection *connection)
        {
            _tcp_connections[owner] = connection;
        }

        io_simplify::libevent::TcpConnection *ClientServer::GetConnectionOf(const void *owner)
        {
            TcpConnections::iterator it = _tcp_connections.find(owner);
            if (it != _tcp_connections.end())
            {
                return it->second;
            }
            
            return nullptr;
        }

        void ClientServer::DeleteConnectionOf(const void *owner)
        {
            TcpConnections::iterator it = _tcp_connections.find(owner);
            if (it != _tcp_connections.end())
            {
                delete it->second;
                
                _tcp_connections.erase(it);
            }
        }
    }
}
