//
// Created by cyz on 5/11/18.
//

#ifndef CLOG_VIN_BASE64_H
#define CLOG_VIN_BASE64_H

#include "Vin_Codec.h"

namespace vince{
    class Vin_Base64 :public Vin_Codec{
        friend class Vin_Starver<Vin_Base64>;

    public:
        virtual std::string encode(const std::string &data) final ;

        virtual std::string decode(const std::string &data) final ;

        virtual int encode(const unsigned char *pSrc, int nSrcLen, char *pDst) final ;

        virtual int decode(const unsigned char *pSrc, int nSrcLen, char *pDst) final ;

    private:

    };
}

#endif //CLOG_VIN_BASE64_H
