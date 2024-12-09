/*
 * ==========================================================================
 *
 *       Filename:  tcp_client.h
 *
 *    Description:  libevent tcp client logic [one tcp client associate with one tcp connection]
 *
 *        Version:  1.0
 *        Created:  2024-07-29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_CLIENT_TCP_CLIENT_H
#define IO_SIMPLIFY_LIBEVENT_CLIENT_TCP_CLIENT_H

#include "libevent/tcp_connection.h"

#include "client_server.h"

#include <cstdint>
#include <string>

#include <functional>

namespace io_simplify {

    namespace libevent::client {

        class TcpClient {
        public:
            using CallbackConnect = std::function<void(int32_t, const std::string&)>;
            using CallbackDisconnected = std::function<void()>;

            using CallbackRead = std::function<void(io_simplify::libevent::TcpConnection*)>;
            using CallbackWrite = std::function<void(int32_t, const std::string&)>;

            using CallbackTimeout = std::function<void()>;

        public:
            explicit TcpClient(ClientServer* client_server);
            ~TcpClient();

            void Connect(const io_simplify::Endpoint& endpoint, 
                const CallbackConnect& connect_callback,
                const CallbackDisconnected& disconnected_callback,
                const timeval& connect_timeout, 
                const CallbackRead& read_callback = nullptr,
                struct ssl_st *ssl = nullptr, 
                enum bufferevent_ssl_state ssl_state = BUFFEREVENT_SSL_CONNECTING);

            void Disconnect();

            void Write(const std::vector<uint8_t>& data, 
                const CallbackWrite& write_callback, 
                const timeval& write_timeout, 
                const CallbackTimeout& write_timeout_callback);

            void Write(const std::vector<uint8_t>& data, 
                const CallbackWrite& write_callback, 
                const timeval& write_timeout, 
                const CallbackTimeout& write_timeout_callback,
                const CallbackRead& read_callback, 
                const timeval& read_timeout, 
                const CallbackTimeout& read_timeout_callback);

        private:
            ClientServer* _client_server;

        private:
            io_simplify::libevent::TcpConnection* _connection;

        private:
            CallbackRead _read_callback;
            CallbackDisconnected _disconnected_callback;

        private:
            TcpClient(TcpClient&&) = delete;
            TcpClient& operator=(TcpClient&&) = delete;

            TcpClient(const TcpClient&) = delete;
            TcpClient& operator=(const TcpClient&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_CLIENT_TCP_CLIENT_H
