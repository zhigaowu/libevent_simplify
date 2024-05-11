/*
 * ==========================================================================
 *
 *       Filename:  libevent_tcp_connection.h
 *
 *    Description:  tcp connection logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_TCP_CONNECTION_H
#define IO_SIMPLIFY_LIBEVENT_TCP_CONNECTION_H

#include "libevent_connection.h"

namespace io_simplify {

    namespace libevent {
        
        class TcpConnection : public Connection {
        public:
            TcpConnection(struct event_base *evbase, evutil_socket_t fd, int options);
            ~TcpConnection();

            int Connect(const Endpoint& endpoint) override;

        public:
            TcpConnection() = delete;

            TcpConnection(TcpConnection&&) = delete;
            TcpConnection& operator=(TcpConnection&&) = delete;

            TcpConnection(const TcpConnection&) = delete;
            TcpConnection& operator=(const TcpConnection&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_TCP_CONNECTION_H
