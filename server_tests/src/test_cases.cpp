/*
 * ==========================================================================
 *
 *       Filename:  test_cases.cpp
 *
 *    Description:  test cases
 *
 *        Version:  1.0
 *        Created:  2024-02-27 16:22:54
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Jingang Zhou, jingang.zhou@baltamatica.com
 * ==========================================================================
 */

#include "libevent_tcp_connection.h"
#include "libevent_tcp_listener.h"

#include "libevent_udp_listener.h"

#include "libevent_socket_server.h"

#include "libevent_event_server.h"

#include "logger/glog_logger.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <list>

#include <filesystem>

#define USE_EVENT_SERVER true

static char data_to_send[8] = {"hello"};

void KeepEventServerAlive(evutil_socket_t, short, void *)
{
    GWARN() << "KeepEventServerAlive";
}

class Cleaner {
    using EventServerList = std::list<io_simplify::libevent::EventServer*>;

    EventServerList _event_servers;

    std::mutex _locker;
    std::condition_variable _condition;

    std::atomic_bool _working;
    std::thread _server;

    void work()
    {
        EventServerList event_servers;
        while (_working)
        {
            do
            {
                std::unique_lock<std::mutex> guard(_locker);
                _condition.wait(guard, [this, &event_servers] () {
                    bool wakeup = _working;
                    if (!_event_servers.empty())
                    {
                        _event_servers.swap(event_servers);
                        wakeup = true;
                    }
                    return wakeup;
                });
            } while (false);
            
            for (io_simplify::libevent::EventServer* event_server : event_servers)
            {
                delete event_server;
            }
            event_servers.clear();
        }
        
    }

public:
    Cleaner()
        : _event_servers()

        , _locker()
        , _condition()

        , _working(true)
        , _server(&Cleaner::work, this)
    {
    }

    ~Cleaner()
    {
        _working = false;
        _condition.notify_one();
        _server.join();
    }

    void Clean(io_simplify::libevent::EventServer* event_server)
    {
        std::unique_lock<std::mutex> guard(_locker);
        _event_servers.push_back(event_server);
        _condition.notify_one();
    }
};

class ThreadConnection : public io_simplify::libevent::EventServer {
    io_simplify::libevent::TcpConnection _tcp_connection;

public:
    ThreadConnection(const io_simplify::libevent::BaseConfig& base_config, evutil_socket_t fd, int options)
        : io_simplify::libevent::EventServer(base_config)
        , _tcp_connection(_base.GetHandle(), fd, options)
    {
    }

    ~ThreadConnection()
    {
    }

    int Serve(Cleaner& cleaner)
    {
        std::vector<uint8_t> stream_buffer(128, 0);

        int res = _tcp_connection.BindConnectionCallback(
            EV_READ|EV_WRITE,
            [this, stream_buffer](struct bufferevent* bev) mutable {
                size_t read_size = _tcp_connection.Read(stream_buffer.data(), stream_buffer.size());

                GINFO() << "data received: " << std::string(stream_buffer.data(), stream_buffer.data() + read_size) << "("<< read_size << ")";

                std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 10));

                GINFO() << "send data: " << _tcp_connection.Write(data_to_send, strlen(data_to_send));
            },
            nullptr/*[](struct bufferevent* bev) {
                GDEBUG() << "send ready...";
            }*/,
            [this, &cleaner] (struct bufferevent* bev, short what) {
                if (what & BEV_EVENT_EOF) 
                {
                    GWARN() << "Connection closed.";
                }
                else if (what & BEV_EVENT_ERROR)
                {
                    GERROR() << "Got an error on the connection: " << strerror(errno);
                }
                else if (what & BEV_EVENT_TIMEOUT) 
                {
                    GWARN() << "time out.";
                }
                else
                {
                    GDEBUG() << "event: " << (int)what;
                }

                GDEBUG() << "Exit completed: " << Exit();

                cleaner.Clean(this);
            });

        if (res >= 0)
        {
            res = io_simplify::libevent::EventServer::Serve();
        }

        return res;
    }
};

int test_tcp_server(io_simplify::libevent::BaseConfig& base_config)
{
    Cleaner cleaner;

    //io_simplify::libevent::EventServer event_server(base_config);
    //GWARN() << "event_server.Serve: " << event_server.Serve(KeepEventServerAlive);

    io_simplify::libevent::SocketServer<io_simplify::libevent::TcpListener> server(base_config);
        
    int res = server.Bind(
        io_simplify::Endpoint{"172.21.206.202", 3600},
        [&base_config, &cleaner](event_base* evbase, evutil_socket_t fd, struct sockaddr *sa, socklen_t socklen) {
            char address[64] = {0};
            evutil_inet_ntop(sa->sa_family, sa->sa_data, address, sizeof(address));
            GWARN() << "Connection open: " << address;

#if 1
            ThreadConnection* connection = new ThreadConnection(base_config, fd, BEV_OPT_CLOSE_ON_FREE);

            int res = connection->Serve(cleaner);
#else
            io_simplify::libevent::TcpConnection* connection = new io_simplify::libevent::TcpConnection(event_server.GetEventBase().GetHandle(), fd, BEV_OPT_CLOSE_ON_FREE);

            std::vector<uint8_t> stream_buffer(128, 0);

            int res = connection->BindConnectionCallback(
                EV_READ|EV_WRITE,
                [connection, stream_buffer](struct bufferevent* bev) mutable {
                    size_t read_size = connection->Read(stream_buffer.data(), stream_buffer.size());

                    GINFO() << "data received: " << std::string(stream_buffer.data(), stream_buffer.data() + read_size) << "("<< read_size << "), send data: " << connection->Write(data_to_send, strlen(data_to_send));
                },
                [](struct bufferevent* bev) {
                    GDEBUG() << "send ready...";
                },
                [connection] (struct bufferevent* bev, short what) {
                    if (what & BEV_EVENT_EOF) 
                    {
                        GWARN() << "Connection closed.";
                    }
                    else if (what & BEV_EVENT_ERROR)
                    {
                        GERROR() << "Got an error on the connection: " << strerror(errno);
                    }
                    else if (what & BEV_EVENT_TIMEOUT) 
                    {
                        GWARN() << "time out.";
                    }
                    else
                    {
                        GDEBUG() << "event: " << (int)what;
                    }
                    delete connection;
            });
#endif

            if (res < 0)
            {
                GERROR() << "construct connection failed: " << res;
                delete connection;
            }
        }, 
        [] (int err, const char* desc) {
            GERROR() << "Listener error: " << desc;
        });
    
    if (res >= 0)
    {
        res =  server.Serve();
    }
    else
    {
        GERROR() << "Bind endpoint error: " << evutil_socket_error_to_string(res);
    }

    return res;
}

int test_udp_server(io_simplify::libevent::BaseConfig& base_config)
{
    io_simplify::libevent::SocketServer<io_simplify::libevent::UdpListener> server(base_config);
    
    char read_buffer[128] = {0};
    int res = server.Bind(
        io_simplify::Endpoint{"172.21.206.202", 3600},
        [&read_buffer](event_base* evbase, evutil_socket_t fd, struct sockaddr *sa, socklen_t socklen) {
            char address[64] = {0};

            int res = recvfrom(fd, read_buffer, sizeof(read_buffer), 0, sa, &socklen);

            const struct sockaddr_in* sav4 = (const struct sockaddr_in*)(sa);
            evutil_inet_ntop(sa->sa_family, &(sav4->sin_addr), address, socklen);
            uint16_t port = ntohs(sav4->sin_port);

            if (res > 0)
            {
                GWARN() << "data received: [" << std::string(read_buffer, read_buffer + res)  << "] from: " << address << ":" << port;

                sendto(fd, data_to_send, strlen(data_to_send), 0, sa, socklen);
            }
            else
            {
                GWARN() << "data received: " << res  << "from: " << address << ":" << port;
            }
        }, 
        [] (int err, const char* desc) {
            GERROR() << "Listener error: " << desc;
        });
    
    if (res >= 0)
    {
        GERROR() << "begin serve: " << evutil_socket_error_to_string(res);
        res =  server.Serve();
        GERROR() << "end serve: " << evutil_socket_error_to_string(res);
    }
    else
    {
        GERROR() << "Bind endpoint error: " << evutil_socket_error_to_string(res);
    }

    return res;
}

int main(int argc, char **argv) 
{
    std::filesystem::path param_path(argv[0]);
    glog::Logger::Instance().Initialize(argv[0], std::filesystem::current_path().string() + "/logs", param_path.filename().string());

    io_simplify::libevent::Initialize();

    const char** supported_method_head = io_simplify::libevent::BaseConfig::SupportedMethods();

    GDEBUG() << "supported methods:";
    for (int i = 0; supported_method_head[i] != nullptr; ++i)
    {
        GDEBUG() << i + 1 << ", " << supported_method_head[i];
    }
    
    io_simplify::libevent::BaseConfig base_config;

#ifdef _WIN32
    base_config.SetFlag(EVENT_BASE_FLAG_STARTUP_IOCP);
#else
    base_config.RequireFeatures(EV_FEATURE_O1);
#endif

    return test_tcp_server(base_config);
}
