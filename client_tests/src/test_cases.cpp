

#include "libevent_tcp_client.h"

#include <mutex>
#include <condition_variable>

#include <iostream>

static std::mutex locker;
static std::condition_variable condition;

#define NOTIFY_TO_WORK() std::unique_lock<std::mutex> lg(locker); \
                            condition.notify_one()

#define WAIT_FOR_NOTIFIED() do \
    { \
        std::unique_lock<std::mutex> lg(locker); \
        condition.wait(lg); \
    } while (false)

int main(int argc, char **argv) 
{
    io_simplify::libevent::Initialize(true);

    const char** supported_method_head = io_simplify::libevent::BaseConfig::SupportedMethods();

    std::cout << "supported methods:" << std::endl;
    for (int i = 0; supported_method_head[i] != nullptr; ++i)
    {
        std::cout << i + 1 << ", " << supported_method_head[i] << std::endl;
    }
    std::cout << "----------------" << std::endl;
    
    io_simplify::libevent::BaseConfig base_config;

#ifdef _WIN32
    base_config.SetFlag(EVENT_BASE_FLAG_STARTUP_IOCP);
#else
    base_config.RequireFeatures(EV_FEATURE_O1);
#endif

    client::libevent::ClientServer server(base_config, timeval{0,100});

    std::vector<uint8_t> data_to_send{'h', 'e', 'l', 'l', 'o'};
    int res = 0;
    std::string error;

    do
    {
        client::libevent::TcpClient client(&server);

        res = client.Connect(
            "192.168.2.30", 
            3801, 
            [] () {
                std::cout << "connected to server" << std::endl;
                NOTIFY_TO_WORK();
            },
            timeval{5,0},
            []() {
                std::cout << "connected to server timeout" << std::endl;
                NOTIFY_TO_WORK();
            },
            [&client] () {
                std::cout << "disconnected to server" << std::endl;
                NOTIFY_TO_WORK();
            }, 
            error);

        if (res < 0)
        {
            std::cout << "connected to server failed: " << error << std::endl;
        }
        else
        {
            WAIT_FOR_NOTIFIED();
        }

        std::vector<uint8_t> read_buffer;
        read_buffer.resize(128, 0);

        std::string reply;
        reply.reserve(read_buffer.size());

        while (true)
        {
            std::cout << "input command: ";
            std::string line;
            std::cin >> line;

            if ("exit" == line)
            {
                break;
            }

            if ("reconnect" == line)
            {
                client.Disconnect();

                res = client.Connect(
                    "192.168.2.30", 
                    3801, 
                    [] () {
                        std::cout << "connected to server" << std::endl;
                        NOTIFY_TO_WORK();
                    },
                    timeval{5,0},
                    []() {
                        std::cout << "connected to server timeout" << std::endl;
                        NOTIFY_TO_WORK();
                    },
                    [&client] () {
                        std::cout << "disconnected to server" << std::endl;
                        NOTIFY_TO_WORK();
                    }, 
                    error);

                if (res < 0)
                {
                    std::cout << "connected to server failed: " << error << std::endl;
                }
                else
                {
                    WAIT_FOR_NOTIFIED();
                }
                continue;
            }

            data_to_send.clear();
            data_to_send.insert(data_to_send.end(), line.begin(), line.end());

#if false
            client.Write(data_to_send, 
            [] (int32_t code, const std::string& desc) {
                std::cout << "write status(" << code << "): " << desc << std::endl;
                NOTIFY_TO_WORK();
            }, 
            timeval{1, 0},
            [] () {
                std::cout << "write timeout" << std::endl;
                NOTIFY_TO_WORK();
            });

            WAIT_FOR_NOTIFIED();
#else
            client.Write(data_to_send, 
            timeval{1, 0},
            [] () {
                std::cout << "write timeout" << std::endl;
                NOTIFY_TO_WORK();
            },
            [&read_buffer, &reply] (io_simplify::libevent::TcpConnection* connection, int32_t code, const std::string& desc) mutable {
                if (0 == code)
                {
                    size_t s = connection->Read(read_buffer.data(), read_buffer.size());
                    
                    reply.insert(reply.begin(), read_buffer.begin(), read_buffer.begin() + s);
                    reply.resize(s);

                    std::cout << "received data:" << reply << std::endl;
                }
                else
                {
                    std::cout << "received error: " << desc << std::endl;
                }
                NOTIFY_TO_WORK();
            },
            timeval{5, 0},
            [] () {
                std::cout << "read timeout" << std::endl;
                NOTIFY_TO_WORK();
            });

            WAIT_FOR_NOTIFIED();
#endif
        }

        //std::this_thread::sleep_for(std::chrono::seconds(5));

        client.Disconnect();
    } while (false);

    return res;
}
