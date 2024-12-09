/*
 * ==========================================================================
 *
 *       Filename:  client_server.h
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

#ifndef IO_SIMPLIFY_LIBEVENT_CLIENT_CLIENTSERVER_H
#define IO_SIMPLIFY_LIBEVENT_CLIENT_CLIENTSERVER_H

#include "libevent/event_server.h"
#include "libevent/async_invoker.h"

namespace io_simplify {

    namespace libevent::client {

        class ClientServer {
            io_simplify::libevent::EventServer* _event_server;
            io_simplify::libevent::AsyncInvoker* _invoker;

        public:
            explicit ClientServer(const io_simplify::libevent::BaseConfig& base_config, const timeval tval = timeval{1, 0});
            ~ClientServer();

            io_simplify::libevent::AsyncInvoker* GetInvoker();
            io_simplify::libevent::EventServer* GetEventServer();

        private:
            ClientServer(ClientServer&&) = delete;
            ClientServer& operator=(ClientServer&&) = delete;

            ClientServer(const ClientServer&) = delete;
            ClientServer& operator=(const ClientServer&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_CLIENT_CLIENTSERVER_H
