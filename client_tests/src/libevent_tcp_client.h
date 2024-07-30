/*
 * ==========================================================================
 *
 *       Filename:  libevent_tcp_client.h
 *
 *    Description:  libevent tcp client logic
 *
 *        Version:  1.0
 *        Created:  2024-07-29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef NETIO_IO_LIBEVENT_TCP_CLIENT_H
#define NETIO_IO_LIBEVENT_TCP_CLIENT_H

#include "libevent_client_server.h"

#include <cstdint>
#include <string>

#include <vector>

#include <functional>

namespace client {

    namespace libevent {

        class TcpClient {
        public:
            using ConnectedCallback = std::function<void()>;

            using ReadCallback = std::function<void(io_simplify::libevent::TcpConnection*, int32_t, const std::string&)>;
            using WriteCallback = std::function<void(int32_t, const std::string&)>;

            using TimeoutCallback = std::function<void()>;

            using DisconnectedCallback = std::function<void()>;

        public:
            explicit TcpClient(ClientServer* client_server);
            ~TcpClient();

            int32_t Connect(const std::string& address, 
                uint16_t port, 
                const ConnectedCallback& connected_callback, 
                const timeval& connect_timeout, 
                const TimeoutCallback& connect_timeout_callback, 
                const DisconnectedCallback& disconnected_callback, 
                std::string& error);

            void Disconnect();

            void Write(const std::vector<uint8_t>& data, 
                const WriteCallback& write_callback, 
                const timeval& write_timeout, 
                const TimeoutCallback& write_timeout_callback);

            void Write(const std::vector<uint8_t>& data, 
                const timeval& write_timeout, 
                const TimeoutCallback& write_timeout_callback,
                const ReadCallback& read_callback, 
                const timeval& read_timeout, 
                const TimeoutCallback& read_timeout_callback);

        private:
            ClientServer* _client_server;

        private:
            ConnectedCallback _connected_callback;
            DisconnectedCallback _disconnected_callback;

        private:
            TcpClient(TcpClient&&) = delete;
            TcpClient& operator=(TcpClient&&) = delete;

            TcpClient(const TcpClient&) = delete;
            TcpClient& operator=(const TcpClient&) = delete;
        };
    }
}

#endif // NETIO_IO_LIBEVENT_TCP_CLIENT_H
