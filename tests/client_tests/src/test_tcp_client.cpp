

#include "libevent/client/tcp_client.h"

#include "common.h"

#include <mutex>
#include <condition_variable>

static std::mutex locker;
static std::condition_variable condition;

#define NOTIFY_TO_WORK() std::unique_lock<std::mutex> lg(locker); \
                            condition.notify_one()

#define WAIT_FOR_NOTIFIED() do \
    { \
        std::unique_lock<std::mutex> lg(locker); \
        condition.wait(lg); \
    } while (false)

int test_tcp_client(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv) 
{
    io_simplify::libevent::client::ClientServer server(base_config, timeval{0,100});

    std::vector<uint8_t> data_to_send{'h', 'e', 'l', 'l', 'o'};
    int res = 0;

    do
    {
        io_simplify::libevent::client::TcpClient client(&server);

        client.Connect(
            io_simplify::Endpoint{ argv[ADDRESS_INDEX], (uint16_t)atoi(argv[PORT_INDEX])}, 
            [] (int32_t status, const std::string& message) {
                if (status < 0)
                {
                    std::cout << "connect to server status(" << message << ")" << std::endl;
                }
                else
                {
                    std::cout << "connected to server" << std::endl;
                }
                NOTIFY_TO_WORK();
            },
            []() {
                std::cout << "disconnected to server" << std::endl;
                NOTIFY_TO_WORK();
            },
            timeval{2,0});

        WAIT_FOR_NOTIFIED();

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
                client.Connect(
                    io_simplify::Endpoint{ "172.21.206.202", 3600}, 
                    [] (int32_t status, const std::string& message) {
                        if (status < 0)
                        {
                            std::cout << "connect to server status(" << message << ")" << std::endl;
                        }
                        else
                        {
                            std::cout << "connected to server" << std::endl;
                        }
                        NOTIFY_TO_WORK();
                    },
                    []() {
                        std::cout << "disconnected to server" << std::endl;
                        NOTIFY_TO_WORK();
                    },
                    timeval{2,0});

                WAIT_FOR_NOTIFIED();
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
            [] (int32_t status, const std::string& message) {
                std::cout << "write status: " << message << std::endl;
                if (status < 0)
                {
                    NOTIFY_TO_WORK();
                }
            },
            timeval{1, 0},
            [] () {
                std::cout << "write timeout" << std::endl;
                NOTIFY_TO_WORK();
            },
            [&read_buffer, &reply] (io_simplify::libevent::TcpConnection* connection) mutable {
                size_t s = connection->Read(read_buffer.data(), read_buffer.size());
                    
                reply.insert(reply.begin(), read_buffer.begin(), read_buffer.begin() + s);
                reply.resize(s);

                std::cout << "received data:[" << reply  << "]" << std::endl;

                NOTIFY_TO_WORK();
            },
            timeval{2, 0},
            [] () {
                std::cout << "read timeout" << std::endl;
                NOTIFY_TO_WORK();
            });

            WAIT_FOR_NOTIFIED();
#endif
        }

        client.Disconnect();
    } while (false);

    return res;
}
