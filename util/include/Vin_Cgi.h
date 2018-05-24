//
// Created by cyz on 5/22/18.
//

#ifndef CLOG_VIN_CGI_H
#define CLOG_VIN_CGI_H

#include "Vin_Exception.h"
#include <map>
#include <sstream>
#include <vector>

using namespace std;
namespace vince {
    /////////////////////////////////////////////////
    /**
    * @file tc_cgi.h
    * @brief CGI处理类
    */
    /////////////////////////////////////////////////
    class VinCgi;

    class Vin_Cgi_Upload;

    class Vin_HttpRequest;

    /**
    * @brief 配置文件异常类
    */
    struct Vin_Cgi_Exception : public Vin_Exception {
        Vin_Cgi_Exception(const string &buffer) : Vin_Exception(buffer) {};

        Vin_Cgi_Exception(const string &buffer, int err) : Vin_Exception(buffer, err) {};

        ~Vin_Cgi_Exception() throw() {};
    };

    /**
    * @brief 全局的友元函数，定义该函数,
    *        则可以用TC_Common::tostr对 vector<Vin_Cgi_Upload>进行输出操作
    */
    ostream &operator<<(ostream &os, const Vin_Cgi_Upload &vinCgiUpload);

    /**
    * @brief cgi上传文件操作，通过该类获取cgi上传的文件信息
    */
    class Vin_Cgi_Upload {
    public:
        friend ostream &operator<<(ostream &os, const Vin_Cgi_Upload &vinCgiUpload);

        /**
        * @brief 构造函数
        */
        Vin_Cgi_Upload();

        /**
        * @brief 拷贝构造函数.
        */

        Vin_Cgi_Upload(const Vin_Cgi_Upload &vinCgiUpload) = default;

        /**
        * @brief 赋值构造函数
        */
        Vin_Cgi_Upload &operator=(const Vin_Cgi_Upload &vinCgiUpload)= default;

    public:
        /**
        * @brief 获取上传的信息.
        *
        * return 上传文件的信息
        */
        string tostr() const;

        /**
        * @brief 获取客户端IE INPUT上传控件的名称.
        *
        * return INPUT上传控件名称
        */
        string getFormFileName() const;

        /**
        * @brief 返回INPUT控件用户输入的名称,及客户端真实的文件名称.
        *
        * return  客户端真实的文件名称
        */
        string retRealFileName() const;

        /**
        * @brief 上传到服务器后,服务器端文件名称.
        *
        * return 服务器端文件名称
        */
        string getSeverFileName() const;

        /**
        * @brief 获取上传的文件大小.
        *
        * return size_t类型，上传的文件大小
        */
        size_t getFileSize() const;

        /**
        * @brief 上传的文件是否超过大小.
        *
        * return 超过大小返回true，否则返回false
        */
        bool isOverSize() const;

    protected:

        /**
        * 上传文件,浏览器file控件名称
        */
        string _sFileName;

        /**
        * 上传文件真实名称,客户端的文件名称
        */
        string _sRealFileName;

        /**
        * 上传文件服务器端名称
        */
        string _sServerFileName;

        /**
        * 上传文件大小,字节数
         */
        size_t _iSize;

        /**
        * 上传文件是否超过大小
        */
        bool _bOverSize;

        friend class Vin_Cgi;
    };

    /**
    * @brief cgi操作相关类. 
    *  
    * 主要操作包括： 
    *  
    * 1 支持参数解析 
    *  
    * 2 支持cookies解析 
    *  
    * 3 支持文件上传,设置上传文件的最大个数,文件的最大大小 
    *  
    * 4 上传文件时, 需要检查文件是否超过最大大小 
    *  
    * 5 上传文件时, 需要检查上传文件个数是否限制
    *  
    * 说明:多个文件同时上传时,浏览器的file控件必须取不同name,否则将无法正确的上传文件 
    *  
    * 注意:调用parseCgi解析标准输入,
    *  
    * 如果有文件上传需要调用setUpload, 并且需要在parseCgi之前调用
    *
    */
    class Vin_Cgi {
        /**
        * @brief TC_Cgi构造函数
        */
        Vin_Cgi();

        /**
        * @brief 析构函数
        */
        virtual ~Vin_Cgi();

        /**
        * @brief 定义环境变量
        */
        enum {
            ENM_SERVER_SOFTWARE = 0,//调用CGI程序的服务器的类型如： CERN/3.0 或 NCSA/1.3.
            ENM_SERVER_NAME,//服务器的IP或名字
            ENM_GATEWAY_INTERFACE,//服务器所实现的CGI版本,对于UNIX服务器, 这是CGI/1.1.
            ENM_SERVER_PROTOCOL,//服务器运行的HTTP协议,这里当是HTTP/1.0.
            ENM_SERVER_PORT,//服务器的端口号
            ENM_REQUEST_METHOD,//POST 或 GET, 取决于你的表单是怎样递交的.
            ENM_PATH_INFO,//附加的路径信息, 由浏览器通过GET方法发出.
            ENM_PATH_TRANSLATED,//在PATH_INFO中系统规定的路径信息.
            ENM_SCRIPT_NAME,//指向这个CGI脚本的路径, 是在URL中显示的(如, /cgi-bin/thescript).
            ENM_HTTP_COOKIE,//cookie的接收是通过设置环境变量HTTP_COOKIE来实现的，CGI程序可以通过检索该变量获取cookie信息。
            ENM_QUERY_STRING,//表单输入的数据，URL中问号后的内容
            ENM_REMOTE_HOST,//递交脚本的主机名，这个值不能被设置.
            ENM_REMOTE_ADDR,//递交脚本的主机IP地址.
            ENM_AUTH_TYPE,//如果服务器支持保护CGI程序的验证机制，这个环境变量的值就是验证机制的类型。
            ENM_REMOTE_USER,//递交脚本的用户名. 如果服务器的authentication被激活，这个值可以设置。
            ENM_REMOTE_IDENT,//如果Web服务器是在ident (一种确认用户连接你的协议)运行, 递交表单的系统也在运行ident, 这个变量就含有ident返回值.
            ENM_CONTENT_TYPE,//如果表单是用POST递交, 这个值将是 application/x-www-form-urlencoded. 在上载文件的表单中, content-type 是个 multipart/form-data.
            ENM_CONTENT_LENGTH,//对于用POST递交的表单, 标准输入口的字节数.
            ENM_HTTP_USER_AGENT//递交表单的浏览器的名称、版本 和其他平台性的附加信息。
        };

        /**
         * @brief 设置上传文件. 
         *  
         * @param sUploadFilePrefix, 文件前缀(包含路径), 如果有文件上传,则文件保存在以该前缀为名称的路径下
         *                           如果有多个文件上传,则文件名称以次在后面加"_序号"
         * @param iMaxUploadFiles    最多上传文件个数,<0:没有限制
         * @param iUploadMaxSize     每个文件上传的最大大小(字节)
         */
        void
        setUpload(const string &sUploadFilePrefix, int iMaxUploadFiles = 5, size_t iUploadMaxSize = 1024 * 1024 * 10,
                  size_t iMaxContentLength = 1024 * 1024 * 10);

        /**
         * @brief 从标准输入解析cgi. 
         */
        void parseCgi();

        /**
         * @brief 直接从http请求解析. 
         *  
         * @param request http请求
         */
        void parseCgi(const Vin_HttpRequest &request);

        /**
        * @brief 获取cgi的url参数multimap. 
        *  
        * @return multimap<string, string>cgi的url参数
        */
        const multimap<string, string> &getParamMap() const;

        /**
        * @brief 获取cgi的参数map, 将multimap转换成map返回 
        *        , 对于一个参数名称对应多个参数值的情况, 只取其中一个值.
        *  
        * @return map<string, string>
        */
        map<string, string> getParamMapEx() const;

        /**
         * @brief 获取cgi环境变量map.
         *
         * @return map<string,string>cgi的环境变量
         */
        map<string, string> getEnvMap() const;

        /**
        * @brief 获取cookies的参数map. 
        *  
        * @return map<string, string>
        */
        const map<string, string> &getCookiesMap() const;

        /**
        * @brief 获取cgi的某个参数. 
        *  
        * @param sName  参数名称
        * @return       
        */
        string &operator[](const string &sName);

        /**
        * @brief 获取cgi的某个参数. 
        *  
        * @param sName 参数名称
        * @return      参数的值
        */
        string getValue(const string &sName) const;


        /**
        * @brief 获取某一名称的参数的多个值. 
        *  
        * 用于解析checkbox这类控件的值( 一个参数名,多个参数值)
        * @param sName   参数名称
        * @param vtValue 该名称的参数值组成的vector
        * @return        vector<string>, 该名称的参数值组成的vector
        */
        const vector<string> &getMultiValue(const string &sName, vector<string> &vtValue) const;

        /**
        * @brief 获取cookie值. 
        *  
        * @param sName cookie名称
        * @return      string类型的cookie值
        */
        string getCookie(const string &sName) const;

        /**
        * @brief 设置cookie值. 
        *  
        * @param sName    cookie名称
        * @param sValue   cookie值
        * @param sExpires 过期日期
        * @param sPath    cookie有效路径
        * @param sDomain  cookie有效域
        * @param bSecure  是否安全(ssl时有效)
        * @return         返回字符串，代表cookie值
        */
        string
        setCookie(const string &sName, const string &sValue, const string &sExpires = "", const string &sPath = "/",
                  const string &sDomain = "", bool bSecure = false);

        /**
        * @brief 参数链表是否为空. 
        *  
        * @return 参数链表为空返回true，否则返回false
        */
        bool isParamEmpty() const;

        /**
        * @brief 参数是否存在. 
        *  
        * @param sName 参数名称
        * @return      存在返回true，否则返回false
        */
        bool isParamExist(const string &sName) const;

        /**
        * @brief 上传文件是否超过大小，多个文件上传时, 
        *        只要有一个文件超过大小,则超过
        * @return 按照以上标准超过大小的返回true，否则返回false
        */
        bool isUploadOverSize() const;

        /**
        * @brief 上传文件是否超过大小,多个文件上传时, 
        *        只要有一个文件超过大小,则超过
        * @param vtUploads 返回超过大小的文件名称(浏览器file控件的名称)
        * @return          按照以上标准超过大小的返回true，否则返回false
        */
        bool isUploadOverSize(vector<Vin_Cgi_Upload> &vtUploads) const;

        /**
         * @brief 是否超过上传文件个数.
         *
         * @return 超过上传个数返回true，否则返回false
         */
        bool isOverUploadFiles() const;

        /**
        * @brief 获取上传文件个数. 
        *  
        * @return size_t上传文件的个数
        */
        size_t getUploadFilesCount() const;

        /**
        * @brief 获取上传文件的相关信息
        *  
        * @return map<string,Vin_Cgi_Upload>结构中， 
        *         保存文件名和文件相关信息的map
        */
        const map<string, Vin_Cgi_Upload> &getUploadFilesMap() const;

        /**
        * @brief 获取环境变量. 
        *  
        * @param iEnv  枚举变量
        * @return      环境变量
        */
        string getCgiEnv(int iEnv);

        /**
        * @brief 获取环境变量. 
        *  
        * @param sEnv 环境变量名称
        * @return     环境变量的值
        */
        string getCgiEnv(const string &sEnv);

        /**
         * @brief 设置环境变量. 
         *  
         * @param sName 环境变量名称
         * @param sValue 环境变量的值
         */
        void setCgiEnv(const string &sName, const string &sValue);

        /**
        * @brief 返回html头，content-type .
        *  
        * @param sHeader 缺省值为"text/html"
        * @return 
        */
        static string htmlHeader(const string &sHeader = "text/html");

        /**
        * @brief http请求的url解码, %后面的换成字符. 
        *  
        * @param sUrl http请求url
        * @return    解码后的字符串
        */
        static string decodeURL(const string &sUrl);

        /**
         * @brief 对url进行编码, 非数字和字母用%XX代替. 
         *  
         * @param sUrl http请求url
         * @return     编码后的url
         */
        static string encodeURL(const string &sUrl);

        /**
         * @brief 对源字符串进行HTML编码(<>"&) 
         *  
         * @param src         源字符串
         * @param blankEncode 是否对空格也编码(空格, \t, \r\n, \n) 
         * @return            HTML编码后的字符串
         */
        static string encodeHTML(const string &src, bool blankEncode = false);

        /**
         * @brief 对源字符串进行XML编码(<>"&'). 
         *  
         * @param src  源字符串
         * @return     XML编码后的字符串
         */
        static string encodeXML(const string &src);

    protected:

        /**
        * @brief 申明,但是不定义,保证这个函数不会被使用
        */
        Vin_Cgi &operator=(const Vin_Cgi &tcCgi)= delete;

        /**
        * @brief GET method. 
        *  
        * @param sBuffer GET的QueryString
        * return 
        */
        void getGET(string &sBuffer);

        /**
        * @brief POST method. 
        *  
        * @param sBuffer   POST的QueryString
        * return 
        */
        void getPOST(string &sBuffer);

        /**
        * @brief 解析文件上传. 
        *  
        * @param mmpParams  [out]输出参数multimap
        * return 
        */
        void parseUpload(multimap<string, string> &mmpParams);

        /**
         * @brief 解析form数据
         */
        void parseFormData(multimap<string, string> &mmpParams, const string &sBoundary);

        /**
         * @brief  忽略空行
         */
        void ignoreLine();

        /**
         * @brief 写文件. 
         *  
         * @param sFileName 文件名称
         * @param sBuffer   要写入的内容
         */
        bool writeFile(FILE *fp, const string &sFileName, const string &sBuffer, size_t &iTotalWrite);

        /**
        * @brief 非上传模式下解析. 
        *  
        * @param mmpParams  [out]输出参数multimap
        * @param sBuffer    [in]输入QueryString
        * return 
        */
        void parseNormal(multimap<string, string> &mmpParams, const string &sBuffer);

        /**
        * @brief 解析cookies. 
        *  
        * @param mpCookies [out]输出cookiesmap
        * @param sBuffer  [in]输入Cookies字符串
        * return
        */
        void parseCookies(map<string, string> &mpCookies, const string &sBuffer);

        /**
        * @brief 控制解析cgi input的基本流程. 
        *  
        * @param mmpParams [out]输出参数 multimap
        * @param mpCooies [out]输出cookies
        * return 
        */
        void readCgiInput(multimap<string, string> &mmpParams, map<string, string> &mpCooies);

    protected:

        /**
         * buffer
         */
        string _buffer;

        /**
         * 流
         */
        istringstream _iss;

        /**
         * 读入
         */
        istream *_is;

        /**
         * 环境变量
         */
        map<string, string> _env;

        /**
        * cgi参数
        */
        multimap<string, string> _mmpParams;

        /**
        * cookies
        */
        map<string, string> _mpCookies;

        /**
        * 上传文件名称前缀
        */
        string _sUploadFilePrefix;

        /**
        * 上传文件的最大个数,<0:无限制
        */
        int _iMaxUploadFiles;

        /**
        * 上传文件的最大大小
        */
        size_t _iUploadMaxSize;

        /**
         * 是否超过上传文件个数
         */
        bool _bOverUploadFiles;

        /**
         * 最大的content-length
         */
        size_t _iMaxContentLength;

        /**
        * 是否超过大小,有一个文件超过则超过
        */
        bool _bUploadFileOverSize;

        /**
        * 上传文件相关信息保存在该map中
        */
        map<string, Vin_Cgi_Upload> _mpUpload;
    };
}


#endif //CLOG_VIN_CGI_H
