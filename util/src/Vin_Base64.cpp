//
// Created by cyz on 5/11/18.
//

#include "util/include/Vin_Base64.h"

namespace vince {
    /**
    * base64编码表
    */
    static const char EnBase64Tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";;
    /**
    * base64解码表
    */
    static const char DeBase64Tab[] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            62,        // '+'
            0, 0, 0,
            63,        // '/'
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
            0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
            0, 0, 0, 0, 0, 0,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    //TODO
    std::string Vin_Base64::encode(const std::string &data) {
        int max_len = (data.length() / 3 * 4 + data.length() % 3 * 2) + 1;
        char *tmp = new char[max_len];
        int len = encode((const unsigned char *) data.c_str(), data.length(), tmp);
        if (len > max_len || len < 0) {
            throw Vin_Codec_Exception("[Vin_Base64::encode] length is greater than max_len");
        }
        std::string ret(tmp);
        delete tmp;
        std::cout << ret << std::endl;
        return ret;
    }

    std::string Vin_Base64::decode(const std::string &data) {
        unsigned long max_len = (data.length() / 4 * 3 + data.length() % 4 ) + 1;
        auto *tmp = new char[max_len];

        int len = decode((const unsigned char *) data.c_str(), data.length(), tmp);
        if (len > max_len || len < 0) {
            throw Vin_Codec_Exception("[Vin_Base64::encode] length is greater than max_len");
        }
        std::string ret(tmp);
        delete tmp;

        std::cout << ret << std::endl;
        return ret;
    }

    int Vin_Base64::encode(const unsigned char *pSrc, int nSrcLen, char *pDst) {
        if (nSrcLen <= 0 || pSrc == nullptr || pDst == nullptr) {
            throw Vin_Codec_Exception("[Vin_Base64::encode] nSrcLen error or pSrc/pDst is NULL");
        }

        unsigned int tmp;
        int i, j;
        for (i = 0, j = 0; i < nSrcLen - 2; i += 3) {
            tmp = pSrc[i] << 16 | pSrc[i + 1] << 8 | pSrc[i + 2];
            pDst[j++] = EnBase64Tab[tmp >> 18];
            pDst[j++] = EnBase64Tab[tmp >> 12 & 0x3F];
            pDst[j++] = EnBase64Tab[tmp >> 6 & 0x3F];
            pDst[j++] = EnBase64Tab[tmp & 0x3F];
        }

        int fag = nSrcLen - i;
        if (fag == 1) {
            pDst[j++] = EnBase64Tab[pSrc[i] >> 2];
            pDst[j++] = EnBase64Tab[(pSrc[i] & 0x3) << 4];
        } else if (fag == 2) {
            tmp = pSrc[i] << 8 | pSrc[i + 1];
            pDst[j++] = EnBase64Tab[tmp >> 10];
            pDst[j++] = EnBase64Tab[tmp >> 4 & 0x3F];
            pDst[j++] = EnBase64Tab[(tmp & 0xF) << 2];
        }

        pDst[j] = '\0';

        return j;
    }

    int Vin_Base64::decode(const unsigned char *pSrc, int nSrcLen, char *pDst) {
        if (nSrcLen <= 0 || pSrc == nullptr || pDst == nullptr) {
            throw Vin_Codec_Exception("[Vin_Base64::decode] nSrcLen error or pSrc/pDst is NULL");
        }

        unsigned int tmp;
        int i, j;
        for (i = 0, j = 0; i < nSrcLen - 3; i += 4) {
            tmp = DeBase64Tab[pSrc[i]] << 18 | DeBase64Tab[pSrc[i + 1]] << 12 | DeBase64Tab[pSrc[i + 2]]<<6|DeBase64Tab[pSrc[i+3]];
            pDst[j++] = tmp >> 16;
            pDst[j++] = tmp >> 8 & 0xFF;
            pDst[j++] = tmp & 0xFF;
        }

        int fag = nSrcLen - i;
        if (fag == 2) {
            tmp = DeBase64Tab[pSrc[i]] << 6 | DeBase64Tab[pSrc[i+1]];
            pDst[j++] = tmp>>4;
        } else if (fag == 3) {
            tmp = DeBase64Tab[pSrc[i]] << 12 | DeBase64Tab[pSrc[i+1]]<<6|DeBase64Tab[pSrc[i+2]];
            pDst[j++] = tmp >> 10;
            pDst[j++] = tmp >> 2 & 0xFF;
        }

        pDst[j] = '\0';

        return j;
    }
}