
#include "libevent_objects.h"

#ifndef _WIN32
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#include <sys/socket.h>
#endif

#include <event2/thread.h>

namespace io_simplify {

    namespace libevent {

        int Initialize(bool uing_threads, bool debug, ev_uint32_t verbose_option)
        {
#ifdef _WIN32
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSADATA wsaData;

            int res = WSAStartup(wVersionRequested, &wsaData);
#else
            int res = 0;
#endif
            if (0 == res) 
            {
                if (uing_threads)
                {
#ifdef _WIN32
                    evthread_use_windows_threads();
#else
                    evthread_use_pthreads();
#endif
                }

                if (debug)
                {
                    event_enable_debug_mode();
                }   

                event_enable_debug_logging(verbose_option);
            }

            return res;
        }
    }
}

