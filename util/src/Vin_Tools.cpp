//
// Created by cyz on 5/6/18.
//

#include <algorithm>
#include "util/include/Vin_Tools.h"

namespace vince{
    int Vin_Tools::mkdirs(std::string dir, mode_t mode) {
        if (dir.size() == 0 || mode < 0 || mode > 0777)
            return -1;
        std::string now;
        for (int i = 0; i < dir.size(); i++) {
            if (dir[i] == '/') {
                now = dir.substr(0, i + 1);
                if (NULL == opendir(now.c_str())) {
                    mkdir(now.c_str(), mode);
                }
            }
        }
        if (NULL == opendir(dir.c_str()))
            mkdir(dir.c_str(), mode);
        return 0;
    }

    char Vin_Tools::x2c(const string &sWhat) {
        char digit;

        if(sWhat.length()<2)
            return '\0';

        digit=(sWhat[0] >= 'A' ? ((sWhat[0] & 0xdf) - 'A')+10 : (sWhat[0] - '0'));
        digit <<=4;
        digit += (sWhat[1] >= 'A' ? ((sWhat[1] & 0xdf) - 'A')+10 : (sWhat[1] - '0'));
        return digit;
    }

    string Vin_Tools::c2x(const char &cWhat) {
        string digit="%";

        char tmp=cWhat>>4;
        digit+=((tmp&0xf)<=9?tmp&0xf+'0':tmp&0xf-10+'A');
        digit+=((cWhat&0xf)<=9?cWhat&0xf+'0':cWhat&0xf-10+'A');

        return digit;
    }

    string Vin_Tools::replace(const string &sString, const string &sSrc, const string &sDest) {
        if(sString.empty()||sSrc.empty())
            return sString;

        string sBuf = sString;

        string::size_type pos = 0;

        while( (pos = sBuf.find(sSrc, pos)) != string::npos)
        {
            sBuf.replace(pos, sSrc.length(), sDest);
            pos += sDest.length();
        }

        return sBuf;

    }

    string Vin_Tools::trimleft(const string &sStr, const string &s, bool bChar) {
        if(sStr.empty())
            return "";

        string sRet;
        if(bChar){
            auto pos=sStr.find_first_not_of(s);
            if(pos==string::npos)
                return "";
            sRet=sStr.substr(pos);
        }else{
            sRet=sStr;
            while(sRet.compare(0,s.length(),s)==0){
                sRet=sRet.substr(s.length());
            }
        }
        return sRet;
    }

    string Vin_Tools::trimright(const string &sStr, const string &s, bool bChar) {
        if(sStr.empty())
            return "";

        string sRet;
        if(bChar){
            sRet=sStr;
            reverse(sRet.begin(),sRet.end());
            auto pos=sRet.find_first_not_of(s);
            if(pos==string::npos)
                return "";
            sRet=sRet.substr(pos);
            reverse(sRet.begin(),sRet.end());
        }else{
            sRet=sStr;
            while(sRet.length()>=s.length()&&sRet.compare(sRet.length()-s.length(),s.length(),s)==0){
                sRet=sRet.substr(sRet.length()-s.length());
            }
        }
        return sRet;
    }

    string Vin_Tools::trim(const string &sStr, const string &s, bool bChar) {
        if(sStr.empty())
            return "";

        return trimright(trimleft(sStr,s,bChar),s,bChar);
    }
}
