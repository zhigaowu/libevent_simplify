/*
 * ==========================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  test cases
 *
 *        Version:  1.0
 *        Created:  2024-02-27 16:22:54
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#include "libevent/libevent_base.h"

#include "common.h"

extern int test_tcp_server(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);
extern int test_udp_server(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);

extern int test_ssl_tcp_server(io_simplify::libevent::BaseConfig& base_config, int argc, char **argv);

int main(int argc, char **argv) 
{
    if (argc < 4)
    {
        std::cerr << "usage: " << argv[0] << " case_name address port" << std::endl;
        return -1;
    }

    io_simplify::libevent::Initialize();

    const char** supported_method_head = io_simplify::libevent::BaseConfig::SupportedMethods();

    std::cout << "supported methods: " << std::endl;
    for (int i = 0; supported_method_head[i] != nullptr; ++i)
    {
        std::cout << i + 1 << ", " << supported_method_head[i] << std::endl;
    }
    
    io_simplify::libevent::BaseConfig base_config;

#ifdef _WIN32
    base_config.SetFlag(EVENT_BASE_FLAG_STARTUP_IOCP);
#else
    base_config.RequireFeatures(EV_FEATURE_O1);
#endif

    if (strcmp(argv[TESTCASE_NAME_INDEX], "tcp_server") == 0)
    {
        return test_tcp_server(base_config, argc, argv);
    }

    else if (strcmp(argv[TESTCASE_NAME_INDEX], "udp_server") == 0)
    {
        return test_udp_server(base_config, argc, argv);
    }

    else if (strcmp(argv[TESTCASE_NAME_INDEX], "ssl_tcp_server") == 0)
    {
        return test_ssl_tcp_server(base_config, argc, argv);
    }
    else
    {
        std::cerr << "unkown test case name: " << argv[TESTCASE_NAME_INDEX] << std::endl;
        std::cin >> argc;
        return -1;
    }
}
