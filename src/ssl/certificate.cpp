
#include "certificate.h"

namespace io_simplify {

    namespace ssl {

        void Initialize()
        {
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();
        }

        Certifcate::Certifcate(int type)
            : _type(type)

            , _ctx(nullptr)

            , _error_message()
        {
        }

        Certifcate::~Certifcate()
        {
            Destroy();
        }

        void Certifcate::Destroy()
        {
            if (_ctx)
            {
                SSL_CTX_free(_ctx);
                _ctx = nullptr;
            }
        }

        SSL *Certifcate::Generate()
        {
            return SSL_new(_ctx);
        }
    }
}

