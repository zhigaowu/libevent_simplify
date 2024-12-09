/*
 * ==========================================================================
 *
 *       Filename:  server_certificate.h
 *
 *    Description:  ssl server logic using openssl
 *
 *        Version:  1.0
 *        Created:  2024-12-06 14:50:00
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_SSL_SERVER_CERTIFICATE_H
#define IO_SIMPLIFY_LIBEVENT_SSL_SERVER_CERTIFICATE_H

#include "certificate.h"

namespace io_simplify {
    
    namespace ssl {

        class ServerCertifcate : public Certifcate {
        public:
            explicit ServerCertifcate(int type = SSL_FILETYPE_PEM);
            ~ServerCertifcate();

            int Create(const std::string& path) override;

            int UseKeyFile(const std::string& path);

        public:
            ServerCertifcate(ServerCertifcate&&) = delete;
            ServerCertifcate& operator=(ServerCertifcate&&) = delete;

            ServerCertifcate(const ServerCertifcate&) = delete;
            ServerCertifcate& operator=(const ServerCertifcate&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_SSL_SERVER_CERTIFICATE_H
