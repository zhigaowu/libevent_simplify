/*
 * ==========================================================================
 *
 *       Filename:  libevent_client_server.h
 *
 *    Description:  libevent client server logic
 *
 *        Version:  1.0
 *        Created:  2024-07-29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef CLIENT_LIBEVENT_CLIENT_SERVER_H
#define CLIENT_LIBEVENT_CLIENT_SERVER_H

#include "libevent_event_server.h"
#include "libevent_async_invoker.h"

#include "libevent_tcp_connection.h"

#include <cstdint>
#include <string>

#include <unordered_map>

namespace client {

    namespace libevent {

        class ClientServer {
            io_simplify::libevent::EventServer* _event_server;
            io_simplify::libevent::AsyncInvoker* _invoker;

            using TcpConnections = std::unordered_map<const void*, io_simplify::libevent::TcpConnection*>;
            TcpConnections _tcp_connections;

        public:
            ClientServer(const io_simplify::libevent::BaseConfig& base_config, const timeval tval);
            ~ClientServer();

            io_simplify::libevent::AsyncInvoker* GetInvoker();
            io_simplify::libevent::EventServer* GetEventServer();

            io_simplify::libevent::TcpConnection* Generate();

            void AddConnectionOf(const void* owner, io_simplify::libevent::TcpConnection* connection);
            io_simplify::libevent::TcpConnection* GetConnectionOf(const void* owner);
            void DeleteConnectionOf(const void* owner);

        private:
            ClientServer(ClientServer&&) = delete;
            ClientServer& operator=(ClientServer&&) = delete;

            ClientServer(const ClientServer&) = delete;
            ClientServer& operator=(const ClientServer&) = delete;
        };
    }
}

#endif // CLIENT_LIBEVENT_CLIENT_SERVER_H
