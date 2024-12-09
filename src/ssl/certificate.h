/*
 * ==========================================================================
 *
 *       Filename:  certificate.h
 *
 *    Description:  ssl logic using openssl
 *
 *        Version:  1.0
 *        Created:  2024-12-06 14:50:00
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_SSL_CERTIFICATE_H
#define IO_SIMPLIFY_LIBEVENT_SSL_CERTIFICATE_H

#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace io_simplify {
    
    namespace ssl {

        void Initialize();

        class Certifcate {
        protected:
            int _type;

        protected:
            SSL_CTX* _ctx;

        protected:
            std::string _error_message;

        public:
            explicit Certifcate(int type = SSL_FILETYPE_PEM);
            virtual ~Certifcate();

            const std::string& GetError() { return _error_message; }

            virtual int Create(const std::string& path) = 0;
            void Destroy();

            SSL* Generate();

        public:
            Certifcate(Certifcate&&) = delete;
            Certifcate& operator=(Certifcate&&) = delete;

            Certifcate(const Certifcate&) = delete;
            Certifcate& operator=(const Certifcate&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_SSL_CERTIFICATE_H
