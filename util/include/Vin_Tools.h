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

using namespace std;
namespace vince{
    class Vin_Tools{
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
    };
}


#endif //CLOG_TOOLS_H
