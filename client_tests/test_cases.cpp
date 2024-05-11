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

#include "libevent_tcp_listener.h"
#include "libevent_server.h"

#include "glog_logger.h"

#include "test1.h"
#include "test2.h"

#include <filesystem>

int main(int argc, char **argv) 
{
    std::filesystem::path param_path(argv[0]);
    glog::Logger::Instance().Initialize(argv[0], std::filesystem::current_path().string() + "/logs", param_path.filename().string());

    io_simplify::libevent::Initialize(true);

    test1_entry(1);
    test2_entry(2);

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

    io_simplify::libevent::SocketServer<io_simplify::libevent::TcpListener> server(base_config);


    char data_to_send[8] = {"hello"};

    server.BindConnectionCallback( 
        [] (struct bufferevent* bev, const struct sockaddr* sa) {
            const struct sockaddr_in* sa_in = (const struct sockaddr_in*)sa;

            char address[64] = {0};
            uint16_t port = ntohs(sa_in->sin_port);
            evutil_inet_ntop(sa->sa_family, &(sa_in->sin_addr), address, sizeof(address));

            GWARN() << "connection: " << address << ":" << port;

            timeval timeout_read{300L, 0L}, timeout_write{0L, 0L};
            int res = bufferevent_set_timeouts(bev, &timeout_read, &timeout_write);
            if (res < 0)
            {
                GWARN() << "bufferevent_set_timeouts failed";
            }
            
            if ((res = bufferevent_enable(bev, EV_READ|EV_WRITE)) < 0)
            {
                GERROR() << "bufferevent_enable failed";
            }
        },
        [&data_to_send](struct bufferevent* bev, const uint8_t* data_read, size_t data_size) {
            GDEBUG() << "data received: " << std::string(data_read, data_read + data_size);

            GINFO_EVERY_N(3) << "every 3 times";
            GINFO_EVERY_T(3) << "every 3 seconds";
            
            if (memcmp(data_read, "close", 5) != 0)
            {
                if (memcmp(data_read, "level", 5) != 0)
                {
                    int sent_res = bufferevent_write(bev, data_to_send, strlen(data_to_send));
                    (void)sent_res;

                    GINFO() << "test1_plus: " << test1_plus(1, 2);
                    GINFO() << "test2_plus: " << test2_plus(3, 4);
                }
                else
                {
                    GINFO() << "change level:" << (char)data_read[5];
                    if (data_read[5] == '1')
                    {
                        *(int*)(0) = 0;
                    }
                    glog::Logger::Instance().ChangeMinLogLevel(data_read[5] - '0' - 1);
                }
            }
            else
            {
                GERROR() << "server shutdown... ";
                event_base_loopbreak(bufferevent_get_base(bev));
            }
        },
        [&data_to_send](struct bufferevent* bev) {
            GDEBUG() << "send ready...";
        },
        [] (struct bufferevent* bev, short what) {
            if (what & BEV_EVENT_EOF) 
            {
                GWARN() << "Connection closed.";
                bufferevent_free(bev);
            }
            else if (what & BEV_EVENT_ERROR)
            {
                GERROR() << "Got an error on the connection: " << strerror(errno);
                bufferevent_free(bev);
            }
            else if (what & BEV_EVENT_TIMEOUT) 
            {
                GWARN() << "time out.";
                bufferevent_free(bev);
            }
            else
            {
                GDEBUG() << "event: " << (int)what;
            }
        });
        
    int res = server.BindEndpoint(
        io_simplify::Endpoint{"172.21.206.202", 3600},
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

    test1_exit();
    test2_exit();

    return res;
}
