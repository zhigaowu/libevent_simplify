
#include "tcp_connection.h"

#include <assert.h>

#include <cstring>

namespace io_simplify {

    namespace libevent {

        TcpConnection::TcpConnection(struct event_base *evbase, evutil_socket_t fd, int options)
            : Connection(evbase, fd, options)
        {
        }

        TcpConnection::TcpConnection(struct event_base *evbase, evutil_socket_t fd, struct ssl_st *ssl, enum bufferevent_ssl_state state, int options)
            : Connection(evbase, fd, ssl, state, options)
        {
        }

        TcpConnection::~TcpConnection()
        {
        }

        int TcpConnection::Connect(const Endpoint& endpoint)
        {
            int res = 0;

            do 
            {
                // set socket information
                struct sockaddr_in client_in;
                memset(&client_in, 0, sizeof(client_in));

                client_in.sin_family = AF_INET;

                if ((res = evutil_inet_pton(client_in.sin_family, endpoint.address.c_str(), &(client_in.sin_addr))) <= 0) 
                {
                    res = EVUTIL_SOCKET_ERROR();
                    break;
                }
                client_in.sin_port = htons(endpoint.port);

                assert(_bev);

                res = bufferevent_socket_connect(_bev, (struct sockaddr*)&client_in, sizeof(client_in));
            } while (false);

            return res;
        }
    }
}

