
#include "connection.h"

namespace io_simplify {

    namespace libevent {

        Connection::Connection(struct event_base *evbase, evutil_socket_t fd, int options)
            : _bev(bufferevent_socket_new(evbase, fd, options))
            
            , _callback_connection_read_ready()
            , _callback_connection_write_done()
            , _callback_connection_event_occurred()
        {
        }

        Connection::Connection(event_base * evbase, evutil_socket_t fd, ssl_st * ssl, bufferevent_ssl_state state, int options)
            : _bev(bufferevent_openssl_socket_new(evbase, fd, ssl, state, options))
            
            , _callback_connection_read_ready()
            , _callback_connection_write_done()
            , _callback_connection_event_occurred()
        {
        }

        Connection::~Connection()
        {
            if (_bev)
            {
                bufferevent_free(_bev);
                
                _bev = nullptr;
            }
        }

        size_t Connection::Read(void *data, size_t size)
        {
            if (_bev)
            {
                return bufferevent_read(_bev, data, size);
            }
            return 0;
        }

        size_t Connection::ReadBuffer(evbuffer *evbuf)
        {
            if (_bev)
            {
                return bufferevent_read_buffer(_bev, evbuf);
            }
            return 0;
        }

        int Connection::Write(const void *data, size_t size)
        {
            if (_bev)
            {
                return bufferevent_write(_bev, data, size);
            }
            return -1;
        }

        int Connection::WriteBuffer(evbuffer *evbuf)
        {
            if (_bev)
            {
                return bufferevent_write_buffer(_bev, evbuf);
            }
            return -1;
        }

        void Connection::callbackToRead(struct bufferevent *bev, void *ctx)
        {
            Connection* connection = (Connection*)ctx;

            connection->_callback_connection_read_ready(bev);
        }

        void Connection::callbackReadyToWrite(struct bufferevent *bev, void *ctx)
        {
            Connection* connection = (Connection*)ctx;

            connection->_callback_connection_write_done(bev);
        }

        void Connection::callbackEventOccurred(struct bufferevent *bev, short what, void *ctx)
        {
            Connection* connection = (Connection*)ctx;
            connection->_callback_connection_event_occurred(bev, what);
        }

        int Connection::SetTimetout(const struct timeval *timeout_read, const struct timeval *timeout_write)
        {
            if (_bev)
            {
                return bufferevent_set_timeouts(_bev, timeout_read, timeout_write);
            }

            return -1;
        }

        int Connection::EnableRead()
        {
            if (_bev)
            {
                return bufferevent_enable(_bev, EV_READ);
            }

            return -1;
        }

        int Connection::DisableRead()
        {
            if (_bev)
            {
                return bufferevent_disable(_bev, EV_READ);
            }

            return -1;
        }

        int Connection::EnableWrite()
        {
            if (_bev)
            {
                return bufferevent_enable(_bev, EV_WRITE);
            }

            return -1;
        }

        int Connection::DisableWrite()
        {
            if (_bev)
            {
                return bufferevent_disable(_bev, EV_WRITE);
            }

            return -1;
        }

        int Connection::SetConnectionCallback(const CallbackConnectionEventOccurred &callback_connection_event_occurred, const CallbackConnectionReadReady& callback_connection_read_ready)
        {
            _callback_connection_event_occurred = callback_connection_event_occurred;
            _callback_connection_read_ready = callback_connection_read_ready;
            if (_bev)
            {
                bufferevent_setcb(_bev, 
                    _callback_connection_read_ready ? Connection::callbackToRead : nullptr, 
                    nullptr, 
                    _callback_connection_event_occurred ? Connection::callbackEventOccurred : nullptr, 
                    this);
            }

            // connection event is activated by read
            return EnableRead();
        }

        int Connection::BindCallback(
            short event,
            const CallbackConnectionReadReady& callback_connection_read_ready,
            const CallbackConnectionWriteDone& callback_connection_write_done,
            const CallbackConnectionEventOccurred& callback_connection_event_occurred)
        {
            _callback_connection_read_ready = callback_connection_read_ready;
            _callback_connection_write_done = callback_connection_write_done;
            _callback_connection_event_occurred = callback_connection_event_occurred;

            int res = -1;
            if (_bev)
            {
                bufferevent_setcb(_bev, 
                    _callback_connection_read_ready ? Connection::callbackToRead : nullptr, 
                    _callback_connection_write_done ? Connection::callbackReadyToWrite : nullptr, 
                    _callback_connection_event_occurred ? Connection::callbackEventOccurred : nullptr, 
                    this);

                res = bufferevent_enable(_bev, event);
            }
            return res;
        }
    }
}

