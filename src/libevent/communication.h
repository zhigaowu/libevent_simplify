/*
 * ==========================================================================
 *
 *       Filename:  communication.h
 *
 *    Description:  communication interface
 *
 *        Version:  1.0
 *        Created:  2024-04-12 19:30:29
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_COMMUNICATION_H
#define IO_SIMPLIFY_LIBEVENT_COMMUNICATION_H

#include "libevent_base.h"

namespace io_simplify {
    
    namespace libevent {

        class Communication {
        public:
            virtual int Connect(const Endpoint& endpoint) = 0;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_COMMUNICATION_H
