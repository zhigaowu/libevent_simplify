/*
 * ==========================================================================
 *
 *       Filename:  client_certificate.h
 *
 *    Description:  ssl client logic using openssl
 *
 *        Version:  1.0
 *        Created:  2024-12-06 14:50:00
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Zhigao Wu, wuzhigaoem@163.com
 * ==========================================================================
 */

#ifndef IO_SIMPLIFY_LIBEVENT_SSL_CLIENT_CERTIFICATE_H
#define IO_SIMPLIFY_LIBEVENT_SSL_CLIENT_CERTIFICATE_H

#include "certificate.h"

namespace io_simplify {
    
    namespace ssl {

        class ClientCertifcate : public Certifcate {
        public:
            explicit ClientCertifcate(int type = SSL_FILETYPE_PEM);
            ~ClientCertifcate();

            int Create(const std::string& path) override;

        public:
            ClientCertifcate(ClientCertifcate&&) = delete;
            ClientCertifcate& operator=(ClientCertifcate&&) = delete;

            ClientCertifcate(const ClientCertifcate&) = delete;
            ClientCertifcate& operator=(const ClientCertifcate&) = delete;
        };
    }
}

#endif // IO_SIMPLIFY_LIBEVENT_SSL_CLIENT_CERTIFICATE_H
