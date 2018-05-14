//
// Created by cyz on 5/11/18.
//

#ifndef CLOG_VIN_CODEC_H
#define CLOG_VIN_CODEC_H

#include <string>
#include <iostream>
#include "Vin_Singleton.h"
#include "Vin_Exception.h"

namespace vince {
    struct Vin_Codec_Exception : public Vin_Exception {
        explicit Vin_Codec_Exception(const std::string &buffer) noexcept : Vin_Exception(buffer) {};

        Vin_Codec_Exception(const std::string &buffer, int err) noexcept : Vin_Exception(buffer, err) {};

        ~Vin_Codec_Exception() noexcept override = default;
        
    };

    class Vin_Codec {
        friend class Vin_Starver<Vin_Codec>;

    public:
        virtual std::string encode(const std::string &data)=0;

        virtual std::string decode(const std::string &data)=0;

        virtual int encode(const unsigned char *pSrc, int nSrcLen, char *pDst)=0;

        virtual int decode(const unsigned char *pSrc, int nSrcLen, char *pDst)=0;

        Vin_Codec()= default;

    private:
        Vin_Codec(const Vin_Codec &)= delete;

        Vin_Codec &operator=(const Vin_Codec &)= delete;
    };
}


#endif //CLOG_VIN_CODEC_H
