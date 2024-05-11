
#include "libevent_connection.h"

namespace io_simplify {

    namespace libevent {

        Connection::Connection(struct event_base *evbase, evutil_socket_t fd, int options)
            : _bev(bufferevent_socket_new(evbase, fd, options))
            
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

        int Connection::BindConnectionCallback(
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
                bufferevent_setcb(_bev, Connection::callbackToRead, _callback_connection_write_done ? Connection::callbackReadyToWrite : nullptr, Connection::callbackEventOccurred, this);

                res = bufferevent_enable(_bev, event);
            }
            return res;
        }
    }
}

