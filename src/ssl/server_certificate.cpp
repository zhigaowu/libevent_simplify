
#include "server_certificate.h"

namespace io_simplify {

    namespace ssl {

        ServerCertifcate::ServerCertifcate(int type)
            : Certifcate(type)
        {
        }

        ServerCertifcate::~ServerCertifcate()
        {
        }

        int ServerCertifcate::Create(const std::string &path)
        {
            static const SSL_METHOD* method = TLS_server_method();

            int res = -1;
            do
            {
                char error_buff[256] = {0};

                if (!(_ctx = SSL_CTX_new(method)))
                {
                    _error_message = ERR_error_string(ERR_get_error(), error_buff);
                    break;
                }
            
                if (SSL_CTX_use_certificate_file(_ctx, path.c_str(), _type) <= 0) 
                {
                    _error_message = ERR_error_string(ERR_get_error(), error_buff);
                    break;
                }

                res = 0;
            } while (false);

            return res;
        }

        int ServerCertifcate::UseKeyFile(const std::string &path)
        {
            int res = -1;
            do
            {
                char error_buff[256] = {0};

                if (SSL_CTX_use_PrivateKey_file(_ctx, path.c_str(), _type) <= 0)
                {
                    _error_message = ERR_error_string(ERR_get_error(), error_buff);
                    break;
                }

                if (!SSL_CTX_check_private_key(_ctx)) 
                {
                    _error_message = ERR_error_string(ERR_get_error(), error_buff);
                    break;
                }

                res = 0;

            } while (false);

            return res;
        }
    }
}

