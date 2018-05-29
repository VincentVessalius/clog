//
// Created by cyz on 5/5/18.
//

#ifndef CLOG_TOOLS_H
#define CLOG_TOOLS_H

#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <cstring>
#include <sstream>

using namespace std;
namespace vince {
    class Vin_Tools {
    public:
        static int mkdirs(std::string dir, mode_t mode);

        /**
        * @brief  将一个string类型转成一个字节 .
        *
        * @param sWhat 要转换的字符串
        * @return char    转换后的字节
        */
        static char x2c(const string &sWhat);

        static string c2x(const char &cWhat);

        /**
        * @brief  替换字符串.
        *
        * @param sString  输入字符串
        * @param sSrc     原字符串
        * @param sDest    目的字符串
        * @return string  替换后的字符串
        */
        static string replace(const string &sString, const string &sSrc, const string &sDest);

        /**
        * @brief  去掉头部以及尾部的字符或字符串.
        *
        * @param sStr    输入字符串
        * @param s       需要去掉的字符
        * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
        * @return string 返回去掉后的字符串
        */
        static string trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

        /**
        * @brief  去掉左边的字符或字符串.
        *
        * @param sStr    输入字符串
        * @param s       需要去掉的字符
        * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
        * @return string 返回去掉后的字符串
        */
        static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

        /**
        * @brief  去掉右边的字符或字符串.
        *
        * @param sStr    输入字符串
        * @param s       需要去掉的字符
        * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
        * @return string 返回去掉后的字符串
        */
        static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

        template<typename T>
        static T strto(const string &str);

        /**
        * @brief  解析字符串,用分隔符号分隔,保存在vector里
        *
        * 例子: |a|b||c|
        *
        * 如果withEmpty=true时, 采用|分隔为:"","a", "b", "", "c", ""
        *
        * 如果withEmpty=false时, 采用|分隔为:"a", "b", "c"
        *
        * 如果T类型为int等数值类型, 则分隔的字符串为"", 则强制转化为0
        *
        * @param sStr      输入字符串
        * @param sSep      分隔字符串(每个字符都算为分隔符)
        * @param withEmpty true代表空的也算一个元素, false时空的过滤
        * @return          解析后的字符vector
        */
        template<typename T>
        static vector<T> sepstr(const string &sStr, const string &sSep, bool withEmpty = false);

        /**
        * @brief  字符串转换成时间结构.
        *
        * @param sString  字符串时间格式
        * @param sFormat  格式
        * @param stTm     时间结构
        * @return         0: 成功, -1:失败
        */
        static int str2tm(const string &sString, const string &sFormat, struct tm &stTm);

        /**
         * @brief GMT格式的时间转化为时间结构
         *
         * eg.Sat, 06 Feb 2010 09:29:29 GMT, %a, %d %b %Y %H:%M:%S GMT
         *
         * 可以用mktime换成time_t, 但是注意时区 可以用mktime(&stTm)
         *
         * - timezone换成本地的秒(time(NULL)值相同) timezone是系统的 ,
         *
         * 需要extern long timezone;
         *
         * @param sString  GMT格式的时间
         * @param stTm     转换后的时间结构
         * @return         0: 成功, -1:失败
         */
        static int strgmt2tm(const string &sString, struct tm &stTm);

        static string lower(const string& sStr);

        static string upper(const string& sStr);
    };

    template<typename T>
    T Vin_Tools::strto(const string &str) {
        T t;
        stringstream sstream(str);
        sstream >> t;
        return t;
    }

    template<typename T>
    vector<T> Vin_Tools::sepstr(const string &sStr, const string &sSep, bool withEmpty) {
        vector<T> res;

        if (sStr.empty())
            return res;

        string::size_type pos1 = 0, pos2;

        while (true) {
            string s;
            pos2 = sStr.find_first_of(sSep, pos1);
            if (pos2 == string::npos) {
                s = sStr.substr(pos1);
            } else if (pos2==pos1){
                s="";
            }else{
                s = sStr.substr(pos1,pos2-pos1);
                pos1=pos2;
            }

            if(withEmpty||!s.empty()){
                res.push_back(strto<T>(s));
            }

            if (pos2 == string::npos) {
                break;
            }

            pos1++;
        }

        return res;
    }
}


#endif //CLOG_TOOLS_H
