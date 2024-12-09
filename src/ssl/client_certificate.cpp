
#include "client_certificate.h"

namespace io_simplify {

    namespace ssl {

        ClientCertifcate::ClientCertifcate(int type)
            : Certifcate(type)
        {
        }

        ClientCertifcate::~ClientCertifcate()
        {
        }

        int ClientCertifcate::Create(const std::string &path)
        {
            static const SSL_METHOD* method = TLS_client_method();

            int res = -1;
            do
            {
                char error_buff[256] = {0};

                if (!(_ctx = SSL_CTX_new(method)))
                {
                    _error_message = ERR_error_string(ERR_get_error(), error_buff);
                    break;
                }
            
                if (SSL_CTX_load_verify_locations(_ctx, path.c_str(), nullptr) <= 0) 
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

