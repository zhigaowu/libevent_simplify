

#include "libevent/client/tcp_client.h"

#include "common.h"

extern int test_tcp_client(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);
extern int test_udp_client(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);
extern int test_ssl_tcp_client(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);

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

    if (strcmp(argv[TESTCASE_NAME_INDEX], "tcp_client") == 0)
    {
        return test_tcp_client(base_config, argc, argv);
    }

    else if (strcmp(argv[TESTCASE_NAME_INDEX], "udp_client") == 0)
    {
        return test_udp_client(base_config, argc, argv);
    }

    else if (strcmp(argv[TESTCASE_NAME_INDEX], "ssl_tcp_client") == 0)
    {
        return test_ssl_tcp_client(base_config, argc, argv);
    }
    else
    {
        std::cerr << "unkown test case name: " << argv[TESTCASE_NAME_INDEX] << std::endl;
        std::cin >> argc;
        return -1;
    }
}
