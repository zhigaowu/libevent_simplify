/*
 * ==========================================================================
 *
 *       Filename:  connection.h
 *
 *    Description:  connection common logic
 *
 *        Version:  1.0
 *        Created:  2024-04-12 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_CONNECTION_H
#define IO_SIMPLIFY_LIBEVENT_CONNECTION_H

#include "communication.h"

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>

#include <functional>

namespace io_simplify {
    
    namespace libevent {

        using CallbackConnectionReadReady = std::function<void(struct bufferevent*)>;
        using CallbackConnectionWriteDone = std::function<void(struct bufferevent*)>;
        using CallbackConnectionEventOccurred = std::function<void(struct bufferevent*, short)>;

        class Connection : public Communication {
        protected:
            struct bufferevent* _bev;

        protected:
            CallbackConnectionReadReady _callback_connection_read_ready;
            CallbackConnectionWriteDone _callback_connection_write_done;
            CallbackConnectionEventOccurred _callback_connection_event_occurred;

        protected:
            static void callbackToRead(struct bufferevent *bev, void *ctx);
            static void callbackReadyToWrite(struct bufferevent *bev, void *ctx);
            static void callbackEventOccurred(struct bufferevent *bev, short what, void *ctx);

        public:
            Connection(struct event_base *evbase, evutil_socket_t fd, int options);
            Connection(struct event_base *evbase, evutil_socket_t fd, struct ssl_st *ssl, enum bufferevent_ssl_state state, int options);
            virtual ~Connection();

            size_t Read(void* data, size_t size);
            size_t ReadBuffer(struct evbuffer *evbuf);

            int Write(const void* data, size_t size);
            int WriteBuffer(struct evbuffer *evbuf);

            int SetTimetout(const struct timeval *timeout_read, const struct timeval *timeout_write);

            int EnableRead();
            int DisableRead();

            int EnableWrite();
            int DisableWrite();

            int SetConnectionCallback(const CallbackConnectionEventOccurred& callback_connection_event_occurred, const CallbackConnectionReadReady& callback_connection_read_ready = nullptr);

            int BindCallback(
                short event,
                const CallbackConnectionReadReady& callback_connection_read_ready,
                const CallbackConnectionWriteDone& callback_connection_write_done,
                const CallbackConnectionEventOccurred& callback_connection_event_occurred);

            struct bufferevent* GetHandle() { return _bev; }

        public:
            Connection() = delete;

            Connection(Connection&&) = delete;
            Connection& operator=(Connection&&) = delete;

            Connection(const Connection&) = delete;
            Connection& operator=(const Connection&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_CONNECTION_H
