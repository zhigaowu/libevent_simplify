/*
 * ==========================================================================
 *
 *       Filename:  libevent_udp_communication.h
 *
 *    Description:  udp communication logic
 *
 *        Version:  1.0
 *        Created:  2024-04-07 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_UDP_COMMUNICATION_H
#define IO_SIMPLIFY_LIBEVENT_UDP_COMMUNICATION_H

#include "libevent_communication.h"

#include <functional>

namespace io_simplify {

    namespace libevent {

        using CallbackUdpReadReady = std::function<void(evutil_socket_t, short, struct sockaddr*, socklen_t)>;

        class UdpCommunication : public Communication {
            struct event_base* _evbase;
            short _flags;

            struct event* _evevent;

            CallbackUdpReadReady _callback_read_ready;

        private:
            static void callbackReadReady(evutil_socket_t fd, short what, void* ctx);

        public:
            UdpCommunication(struct event_base *evbase, short flags);
            ~UdpCommunication();

            void BindReadCallback(const CallbackUdpReadReady& callback_read_ready);

            size_t RecvFrom(evutil_socket_t fd, void* data, size_t size, int flags, struct sockaddr* sa, socklen_t socklen);

            int SendTo(evutil_socket_t fd, void* data, size_t size, int flags, struct sockaddr* sa, socklen_t socklen);

            int Connect(const Endpoint& endpoint) override;

        public:
            UdpCommunication() = delete;

            UdpCommunication(UdpCommunication&&) = delete;
            UdpCommunication& operator=(UdpCommunication&&) = delete;

            UdpCommunication(const UdpCommunication&) = delete;
            UdpCommunication& operator=(const UdpCommunication&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_UDP_COMMUNICATION_H
