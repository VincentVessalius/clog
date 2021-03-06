//
// Created by cyz on 5/28/18.
//

#ifndef CLOG_VIN_HTTP_H
#define CLOG_VIN_HTTP_H

#include <string>
#include <map>
#include <list>
#include "util/include/Vin_Exception.h"

using namespace std;

namespace vince {
    /**
     * @brief  http协议解析异常类
     */
    struct Vin_URL_Exception : public Vin_Exception {
        Vin_URL_Exception(const string &sBuffer) : Vin_Exception(sBuffer) {};

        ~Vin_URL_Exception() noexcept {};
    };

    /**
     * @brief  http协议解析异常类
     */
    struct Vin_Http_Exception : public Vin_Exception {
        Vin_Http_Exception(const string &sBuffer) : Vin_Exception(sBuffer) {};

        ~Vin_Http_Exception() noexcept {};
    };

    /**
     * @brief  http应答协议解析异常类
     */
    struct Vin_HttpResponse_Exception : public Vin_Http_Exception {
        Vin_HttpResponse_Exception(const string &sBuffer) : Vin_Http_Exception(sBuffer) {};

        ~Vin_HttpResponse_Exception() noexcept {};
    };

    /**
     * @brief  http请求协议解析异常类
     */
    struct Vin_HttpRequest_Exception : public Vin_Http_Exception {
        Vin_HttpRequest_Exception(const string &sBuffer) : Vin_Http_Exception(sBuffer) {};

        ~Vin_HttpRequest_Exception() noexcept {};
    };

    /**
     * @brief  简单的URL解析类.
     *
     * eg：
     * ftp://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref.
     *
     * scheme: ftp.
     *
     * user:user.
     *
     * pass:password.
     *
     * domain:www.qq.com.
     *
     * port:8080.
     *
     * path:/abc/dev/query.
     *
     * query:a=b&c=3.
     *
     * ref:ref.
     *
     * request:/abc/dev/query?a=b&c=3#ref.
     *
     * relativePath:/abc/dev/.
     *
     * rootPath:ftp://user:password@www.qq.com:8080/.
     */
    class Vin_URL {
    public:
        /**
         *  @brief  URL类型
         */
        enum URL_TYPE {
            HTTP,
            HTTPS,
            FTP
        };

        Vin_URL();

        Vin_URL &operator=(const Vin_URL &)= default;

    public:
        /**
         * @brief  解析URL, url必须是绝对路径.
         *
         * @throws       TC_URL_Exception
         * @param sURL  具体的URL串
         * @return      解析成功返回true，否则返回false
         */
        bool parseURL(const string &sURL);

        /**
         * @brief   清空
        */
        void clear();

        /**
         * @brief  获取scheme.
         *
         * @return const string&类型的scheme
         */
        string getScheme() const;

        /**
         * @brief  获取用户名.
         *
         * @return const string& ：用户名
         */
        string getUserName() const;

        /**
         * @brief  获取密码.
         *
         * @return const string& ：密码
         */
        string getPassword() const;

        /**
         * @brief  获取域名.
         *
         * @return const string& ：域名
         */
        string getDomain() const;

        /**
         * @brief   获取端口.
         *
         * @return const string& ：端口
         */
        string getPort() const;

        /**
         * @brief   是否是缺省的端口.
         *
         * @return bool型，类缺省的端口返回true，否则返回false
         */
        bool isDefaultPort() const;

        /**
         * @brief   获取路径.
         *
         * @return const string& ：路径值
         */
        string getPath() const;

        /**
         * @brief   获取查询部分.
         *
         * @return string ：查询结果
         */
        string getQuery() const;

        /**
         * @brief 取获Request，不包括Host,
         *        例如http://www.qq.com/abc?a=b#ref则为:/abc?a=b#ref
         * @return 符合上述条件的request
         */
        string getRequest() const;

        /**
         * @brief  获取Ref.
         *
         * @return Ref串
         */
        string getRef() const;

        /**
         * @brief   是否有效.
         *
         * @return 有效返回true，否则返回false
         */
        bool isValid() const;

        /**
         * @brief   获取解析后URL.
         *
         * @return 解析后的URL
         */
        string getURL() const;

        /**
         * @brief   获取URL类型.
         *
         * @return  URL类型
         */
        int getType() const;

        /**
         * @brief   获取相对路径.
         *
         * @return 相对路径字符串
         */
        string getRelativePath() const;

        /**
         * @brief   获取根路径.
         *
         * @return  根路径字符串
         */
        string getRootPath() const;

        /**
         * @brief 调整路径.
         *
         * 以当前URL调整路径.
         *
         * 1 http://www.qq.com/a/b?a=b, /test/abc => http://www.qq.com/test/abc
         * 2 http://www.qq.com/a/b?a=b, test/abc => http://www.qq.com/a/b/test/abc
         * 3 http://www.qq.com/a/b?a=b, ../test/abc   => http://www.qq.com/a/test/abc
         *
         * @param  sRelativeURL: 相对当前URL的地址
         * @return 返回调整后的URL
         */
        Vin_URL buildWithRelativePath(const string &sRelativeURL) const;

        /**
         * @brief 规整化.
         */
        void specialize();

    protected:
        /**
         * @brief  换成URL.
         *
         * @return URL串
         */
        string _toURL();

        /**
         * @brief 获取段.
         *
         * @param frag
         * @return string
         */
        //string _getFragment(const string& frag) const;

        /**
         * @brief  类型到字符串的转换
         *
         * @return string：转换后的字符串
         */
        string _type2String() const;

        /**
         * @brief  获取协议的缺省端口.
         *
         * @return string：缺省端口
         */
        string _getDefaultPort() const;

        /**
         * @brief  简化URL.
         *
         * @param sPath 原路径
         * @return      简化后的URL
         */
        string _simplePath(const string &sPath) const;

    protected:

        /**
         * URL类型
         */
        enum URL_TYPE _iURLType;

        string _sScheme;
        string _sUser;
        string _sPass;
        string _sDomain;
        string _sPort;
        string _sPath;
        string _sQuery;
        string _sRef;

        /**
         * 解析后的URL
         */
        string _sURL;

    };

    /**
     * @brief  http协议解析类, 请求和响应都在该类中解析
     */
    class Vin_Http {
    public:
        Vin_Http();

        /**
         * @brief  不区分头部大小写
         */
        struct CmpCase {
            bool operator()(const string &s1, const string &s2) const {
                //return TC_Common::upper(s1) < TC_Common::upper(s2);
                return strcasecmp(s1.c_str(), s2.c_str()) < 0;
            }
        };

        typedef multimap<string, string, CmpCase> http_header_type;

    public:
        /**
         * @brief  删除头部.
         *
         * @param sHeader:头部信息
         */
        void eraseHeader(const string &sHeader);

        /**
         * @brief  设置 Cache-Control.
         *
         * @param sCacheControl
         */
        void setCacheControl(const string &sCacheControl);

        /**
         * @brief  设置 Connection.
         *
         * @param sConnection：Connection信息
         */
        void setConnection(const string &sConnection);

        /**
         * @brief  设置 Content-Type.
         *
         * @param sContentType：Content信息
         */
        void setContentType(const string &sContentType);

        /**
         * @brief  设置内容长度.
         *
         * @param iContentLength：要设置的长度值
         */
        void setContentLength(size_t iContentLength);

        /**
         * @brief  设置 Referer.
         *
         * @param sReferer：Referer信息
         */
        void setReferer(const string &sReferer);

        /**
         * @brief  设置 Host.
         *
         * @param sHost 例如 www.qq.com:80
         */
        void setHost(const string &sHost);

        /**
         * @brief  设置 Accept-Encoding.
         *
         * @param sAcceptEncoding, gzip, compress, deflate, identity
         */
        void setAcceptEncoding(const string &sAcceptEncoding);

        /**
         * @brief  设置 Accept-Language.
         *
         * @param sAcceptLanguage：Accept-Language信息
         */
        void setAcceptLanguage(const string &sAcceptLanguage);

        /**
         * @brief  设置 Accept.
         *
         * @param sAccept Accept信息
         */
        void setAccept(const string &sAccept);

        /**
         * @brief  设置 Transfer-Encoding.
         *
         * @param sTransferEncoding：Transfer-Encoding信息
         */
        void setTransferEncoding(const string &sTransferEncoding);

        /**
         * @brief  设置header，
         * 常用的值请使用已经有的get/set方法设置
         * (除Set-Cookie&Cookie外,头部字段不能重复)
         * @param sHeadName  header的名字
         * @param sHeadValue header的值
         */
        void setHeader(const string &sHeadName, const string &sHeadValue);

        /**
         * @brief  设置header，常用的值请使用已经有的get/set方法设
         *         (头部字段可以重复)
         * @param sHeadName   header的名字
         * @param sHeadValue  header的值
         */
        void setHeaderMulti(const string &sHeadName, const string &sHeadValue);


        /**
         * @brief 获取http头部参数，(Set-Cookie和Cookie不要用这个获取,
         *        单独有函数获取)
         * @param sHeader header名字
         * @return        header的相关信息
         */
        string getHeader(const string &sHeader) const;

        /**
         * @brief  获取头(重复的也提取).
         *
         * @param sHeadName  header的名字
         * @return           vector<string>header的值
         */
        vector<string> getHeaderMulti(const string &sHeadName) const;

        /**
         * @brief  获取http内容长度.
         *
         * @return http内容长度.
         */
        size_t getContentLength() const;

        /**
         * @brief  获取http请求Host.
         *
         * @return host请求信息
         */
        string getHost() const;

        /**
         * @brief 获取http头部长度.
         *
         * @return 头部长度
         */
        size_t getHeadLength() const;

        /**
         * @brief 获取http内容.
         *
         * @return http内容串
         */
        string getContent() const;

        /**
         * @brief 设置http body(默认不修改content-length).
         *
         * @param content               http body内容
         * @param bUpdateContentLength  是否更新ContentLength
         */
        void setContent(const string &content, bool bUpdateContentLength = false);

        /**
         * @brief 获取内容类型.
         *
         * @return string
         */
        string getContentType() const;

        /**
         * @brief 获取http头部map.
         *
         * @return http_header_type&
         */
        const http_header_type &getHeaders() const;

        /**
         * @brief 重置
         */
        void reset();

        /**
         * @brief 读取一行.
         *
         * @param ppChar  读取位置指针
         * @return string 读取的内容
         */
        static string getLine(const char **ppChar);

        /**
         * @brief 读取一行.
         *
         * @param ppChar   读取位置指针
         * @param iBufLen  长度
         * @return string  读取的内容
         */
        static string getLine(const char **ppChar, int iBufLen);

        /**
         * @brief 生成头部字符串(不包含第一行).
         *
         * @return string：头部字符串
         */
        string genHeader() const;

        /**
         * @brief 该http原始数据包是否是chunk编码格式.
         *
         * @return bool：包含返回true，否则返回false
         */
        bool isChunked() const;

        /**
         * @brief 解析请求head，不解析第一行,
         *        第一行请求包和响应包不一样， 后面的数据解析为map格式
         * @param szBuffer
         * @return const char*, 偏移的指针
         */
        static const char *parseHeader(const char *szBuffer, http_header_type &sHeader);

    protected:

        /**
         * http头部所有变量
         */
        http_header_type _headers;

        /**
         * http头部长度
         */
        size_t _headLength;

        /**
         * http头部内容
         */
        string _content;

        /**
         * 是否是chunk模式
         */
        bool _bIsChunked;
    };

    /********************* TC_HttpCookie***********************/
    /**
     * @brief 简单的Cookie管理类.
     */
    class Vin_HttpCookie {
    public:
        Vin_HttpCookie() = default;

        typedef map<string, string, Vin_Http::CmpCase> http_cookie_data;

        struct Cookie {
            http_cookie_data _data;
            string _domain;
            string _path;
            time_t _expires;       //等于0表示只在当前回话有效
            bool _isSecure;
        };

    public:
        /**
         * @brief 清空数据
         */
        void clear();

        /**
         * @brief 增加Cookie到管理器中.
         *
         * @param sCookieRspURL 产生该Cookie HTTP响应的URL
         * @param vCookies      set-Cookie字符串
         */
        void addCookie(const string &sCookieRspURL, const vector<string> &vCookies);

        /**
         * @brief 增加Cookie到管理器.
         *
         * @param cookie
         */
        void addCookie(const Cookie &cookie);

        /**
         * @brief 增加Cookie到管理器.
         *
         * @param cookie
         */
        void addCookie(const list<Cookie> &cookie);

        /**
         * @brief 获取某个url的cookie域值对，去掉了Domain,Path等字段
         * @param sReqURL 需要匹配的URL
         * @param sCookie
         */
        void getCookieForURL(const string &sReqURL, list<Cookie> &cookies);

        /**
         * @brief 获取某个url的cookie域值对，去掉了Domain,Path等字段
         * @param sReqURL  需要匹配的URL
         * @param sCookie
         * @param string
         */
        void getCookieForURL(const string &sReqURL, string &sCookie);

        /**
         * @brief 匹配域名，sCookieDomain串有多个域
         * sCookieDomain未以.开头, 则sCookieDomain=sDomain
         * sCookieDomain以.开头,则sDomain右边匹配sDomain,sDomain比sCookieDomain最多多一个.
         * (x.y.z.com 匹配 .y.z.com, x.y.z.com 不匹配.z.com )
         * @param sCookieDomain
         * @param sDomain       域名
         * @return bool         匹配成功返回true，否则返回false
         */
        static bool matchDomain(const string &sCookieDomain, const string &sDomain);

        /**
         * @brief 匹配路径.
         *
         * @param sCookiePath sPath的前缀
         * @param sPath       路径
         * @return            匹配路径的长度
         */
        static size_t matchPath(const string &sCookiePath, const string &sPath);

        /**
         * @brief 获取所有的Cookie.
         *
         * @return list<Cookie>&
         */
        list<Cookie> getAllCookie();

        /**
         * @brief  删除过期的Cookie，仅仅存在与当前回话的Cookie不删除
         * @param t       超过当前时间的cookie都删除
         * @param bErase true:存在于当前回话的Cookie也删除
         *               false:存在于当前回话的Cookie不删除
         */
        void deleteExpires(time_t t, bool bErase = false);

        /**
         * @brief  返回所有没有过期的Cookie且有Expires字段的Cookie，
         * 即仅仅存在与当前回话的Cookie是不返回的通常用于永久序列化
         * @param t 超过当前时间的cookie都删除
         * @return  list<Cookie>:所有符合条件的cookie
         */
        list<Cookie> getSerializeCookie(time_t t);

    protected:
        /**
         * @brief  判断Cookie是有效.
         *
         * @param tURL
         * @return size_t
         */
        size_t lenCookieMatch(const Cookie &cookie, const Vin_URL &tURL) const;

        /**
         * @brief  检查Cookie是否过期，当前回话的不算过期(expires=0)
         * @param cookie 被检查的cookie对象
         * @return bool  过期返回true，否则返回false
         */
        bool isCookieExpires(const Cookie &cookie, time_t t = time(nullptr)) const;

        /**
         * @brief  添加Cookie.
         *
         * @param cookie  要添加的cookie对象
         * @param cookies 要被添加的list对象
         */
        void addCookie(const Cookie &cookie, list<Cookie> &cookies);

        /**
         * @brief 修正Domain.
         *
         * @param sDomain  修正前的Domain
         * @return string 修正后的Domain
         */
        static bool fixDomain(const string &sDomain, string &sFixDomain);

    protected:

        /**
         * @brief  保存Cookie.
         *
         * key:domain+path
         */
        list<Cookie> _cookies;
    };

    /********************* TC_HttpResponse ***********************/

    class Vin_HttpResponse : public Vin_Http {
    public:
        Vin_HttpResponse();

    public:
        /**
         * @brief reset响应包
         */
        void reset();

        /**
         * @brief 增量decode,输入的buffer会自动在解析过程中被清除掉，
         * 增量decode之前必须reset，
         * (网络接收的buffer直接添加到sBuffer里面即可, 然后增量解析)
         * (能够解析的数据TC_HttpResponse会自动从sBuffer里面消除，直到网络接收完毕或者解析返回true)
         * @param buffer
         * @throws TC_HttpResponse_Exception, 不支持的http协议, 抛出异常
         * @return true:解析出一个完整的buffer
         *        false:还需要继续解析，如果服务器主动关闭连接的模式下
         *        , 也可能不需要再解析了
         */
        bool incrementDecode(string &sBuffer);

        /**
         * @brief 解析http应答(采用string方式) ，
         * 注意:如果http头部没有Content-Length且非chunk模式, 则返回true
         * 需要网络层判断(http服务端主动关闭连接也算是发送http响应完整了)
         * @param sBuffer
         * @return bool, sBuffer是否是完整的http请求
         */
        bool decode(const string &sBuffer);

        /**
         *  @brief 解析http应答，
         * 注意:如果http头部没有Content-Length且非chunk模式, 则返回true
         * 需要网络层判断(http服务端主动关闭连接也算是发送http响应完整了)
         * @param sBuffer
         * @return bool, sBuffer是否是完整的http请求
         */
        bool decode(const char *sBuffer, size_t iLength);

        /**
         * @brief 编码应答包(采用string方式).
         *
         * @param sBuffer
         * @return string 编码后的应答包
         */
        string encode() const;

        /**
         * @brief 编码应答包(采用vector<char>方式).
         *
         * @param sBuffer
         * @return string 编码后的应答包(vector<char>形式的)
         */
        void encode(vector<char> &vBuffer) const;

        /**
         * @brief 获取第一行(HTTP/1.1 200 OK).
         *
         * @return string 获取的内容
         */
        string getResponseHeaderLine() const;

        /**
         * @brief 获取HTTP响应状态(例如200).
         *
         * @return int
         */
        int getStatus() const;

        /**
         * @brief 设置状态.
         *
         * @param status 状态值
         */
        void setStatus(int status);

        /**
         * @brief 获取描述(例如OK).
         *
         * @return string 描述信息
         */
        string getAbout() const;

        /**
         * @brief 设置描述.
         *
         * @param about 描述信息
         */
        void setAbout(const string &about);

        /**
         * @brief 获取版本, 例如HTTP/1.1   .
         *
         * @return string 版本信息
         */
        string getVersion() const;

        /**
         * @brief 另种模式:HTTP/1.1(默认)或者HTTP/1.0  .
         *
         * @param sVersion
         */
        void setVersion(const string &sVersion);

        /**
         * @brief 设置应答状态(采用string方式).
         *
         * @param status 状态码
         * @param about 描述信息
         * @param sBody post协议body的内容
         */
        void setResponse(int status = 200, const string &about = "OK", const string &sBody = "");

        /**
         * @brief 设置应答状态.
         *
         * @param status 状态码
         * @param about  描述信息
         * @param sBuffer post协议body的内容
         * @param iLength sBuffer长度
         */
        void setResponse(int status, const string &about, const char *sBuffer, size_t iLength);

        /**
         * @brief 设置应答, 缺省status=200, about=OK.
         *
         * @param sBuffer 应答内容
         * @param iLength sBuffer长度
         */
        void setResponse(const char *sBuffer, size_t iLength);

        /**
         * @brief 设置服务.
         *
         * @param sServer 服务的信息
         */
        void setServer(const string &sServer);

        /**
         * @brief 设置Set-Cookie.
         *
         * @param sCookie Cookie信息
         */
        void setSetCookie(const string &sCookie);

        /**
         * @brief 获取Set-Cookie.
         *
         * @return vector<string>
         */
        vector<string> getSetCookie() const;

        /**
         * @brief 解析应答头.
         *
         * @param szBuffer 应答头信息
         * @return
         */
        void parseResponseHeader(const char *szBuffer);

    protected:

        /**
         * 应答状态
         */
        int _status;

        /**
         * 应答描述
         */
        string _about;

        /**
         * 获取版本
         */
        string _version;

        /**
         * 获取第一行
         */
        string _headerLine;

        /**
         * 临时的content length
         */
        size_t _iTmpContentLength;
    };

    /********************* TC_HttpRequest ***********************/
    class Vin_HttpRequest : public Vin_Http {
    public:
        ///////////////////////////////////////////////////////////////////
        Vin_HttpRequest();

        /**
         * @brief 请求协议类型
         */
        enum {
            REQUEST_GET = 0,
            REQUEST_POST,
            REQUEST_OPTIONS,
            REQUEST_HEAD,
            REQUEST_PUT,
            REQUEST_DELETE
        };

    public:
        /**
         * @brief 检查http请求是否收全.
         *
         * @param sBuffer http请求
         * @throws TC_HttpRequest_Exception, 不支持的http协议, 抛出异常
         * @return  true: 收全, false:不全
         */
        static bool checkRequest(const char *sBuffer, size_t len);

        /**
         * @brief 重置
         */
        void reset();

        /**
         * @brief 设置 User-Agent.
         *
         * @param sUserAgent
         */
        void setUserAgent(const string &sUserAgent);

        /**
         * @brief 设置 Cookie.
         *
         * @param sCookie
         */
        void setCookie(const string &sCookie);

        /**
         * @brief 获取原始Cookie行.
         *
         * @return vector<string>
         */
        vector<string> getCookie();

        /**
         * @brief 解析http请求, 如果不是HTTP请求则抛出异常.
         *
         * @param sBuffer 要解析的http请求
         * @return        sBuffer是否是完整的http请求
         * @throw         TC_HttpRequest_Exception
         */
        bool decode(const string &sBuffer);

        /**
         * @brief 解析http请求,
         *        如果不是HTTP请求则抛出异常(采用vector<char>方式).
         *
         * @param sBuffer http请求
         * @param iLength 长度
         * @throw         TC_HttpRequest_Exception
         * @return        sBuffer是否是完整的http请求
         */
        bool decode(const char *sBuffer, size_t iLength);

        /**
         * @brief 生成请求(采用string方式).
         *
         * @param sUrl
         * @return string
         */
        string encode();

        /**
         * @brief 生成请求(采用vector<char>方式).
         *
         * @param buffer请求内容
         */
        void encode(vector<char> &buffer);

        /**
         * @brief 设置Get请求包.
         *
         * @param sUrl         例如:http://www.qq.com/query?a=b&c=d
         * @param bCreateHost  是否新创建头部的Host信息
         *                     (默认, 如果有Host信息了, 就不创建)
         *                     (注意, 是在encode的时候创建的)
         */
        void setGetRequest(const string &sUrl, bool bNewCreateHost = false);

        /**
         * @brief 设置Head请求包.
         *
            * @param sUrl         例如:http://www.qq.com/query?a=b&c=d
            * @param bCreateHost  是否新创建头部的Host信息
            *                     (默认, 如果有Host信息了, 就不创建)
            *                     (注意, 是在encode的时候创建的)
            */
        void setHeadRequest(const string &sUrl, bool bNewCreateHost = false);

        /**
         * @brief 设置POST请求包(采用string方式).
         *
         * @param sUrl        例如:http://www.qq.com/query
         * @param sPostBody   如果requestType是GET, 则sPostBody无效
         * @param bCreateHost 是否新创建头部的Host信息
         *                     (默认, 如果有Host信息了, 就不创建)
         *                     (注意, 是在encode的时候创建的)
         */
        void setPostRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost = false);

        /**
         * @brief 设置POST请求包(采用char*方式).
         *
         * @param sUrl        例如:http://www.qq.com/query
         * @param sPostBody   如果requestType是GET, 则sPostBody无效
         * @param bCreateHost 是否新创建头部的Host信息
         *                     (默认, 如果有Host信息了, 就不创建)
         *                     (注意, 是在encode的时候创建的)
         */
        void setPostRequest(const string &sUrl, const char *sBuffer, size_t iLength, bool bNewCreateHost = false);

        void setPutRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost = false);

        void setDeleteRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost = false);

        /**
         * @brief 设置Get请求包.
         *
         * @param sUrl         例如:http://www.qq.com/query?a=b&c=d
         * @param bCreateHost  是否新创建头部的Host信息
         *                     (默认, 如果有Host信息了, 就不创建)
         *                     (注意, 是在encode的时候创建的)
         */
        void setOptionsRequest(const string &sUrl, bool bNewCreateHost = false);

        /**
         * @brief 获取url里面的地址和端口.
         *
         * @param sHost
         * @param iPort
         */
        void getHostPort(string &sDomain, uint32_t &iPort);

        /**
         * @brief 发送HTTP请求.
         *
         * @param iTimeout 毫秒
         * @return        0 成功；
         *               <0失败, 具体值参见TC_ClientSocket声明
         */
        int doRequest(Vin_HttpResponse &stHttpRep, int iTimeout = 3000);

        /**
         * @brief 是否是GET请求.
         *
         * @return 是GET请求返回true，否则返回false
         */
        bool isGET() const;

        /**
         * @brief 是否是HEAD请求.
         *
         * @return 是HEAD请求返回true，否则返回false
         */
        bool isHEAD() const;

        /**
         * @brief 是否是POST请求.
         *
         * @return 是GET请求返回true，否则返回false
         */
        bool isPOST() const;

        /**
         * @brief 是否是OPTIONS请求.
         *
         * @return 是GET请求返回true，否则返回false
         */
        bool isOPTIONS() const;

        /**
         * @brief 获取请求的URL.
         *
         * @return const TC_URL&
         */
        const Vin_URL &getURL() const;

        /**
         * @brief 获取完整的http请求.
         *
         * @return http请求串
         */
        string getOriginRequest() const;

        /**
         * @brief 获取http请求的url，不包括Host,
         *        例如http://www.qq.com/abc?a=b, 则为:/abc?a=b
         * @return http请求的url
         */
        string getRequest() const;

        /**
         * @brief 获取http请求的url部分, 即?前面，不包括Host,
         *        例如http://www.qq.com/abc?a=b#def, 则为:/abc
         * @return http请求的url部分
         * */
        string getRequestUrl() const;

        /**
         * @brief 获取http请求url后的参数部分，即?后面#前面部分不
         *        包括Host, 例如http://www.qq.com/abc?a=b#def, 则为:a=b
         * @return http请求url后的参数部分
         */
        string getRequestParam() const;

        /**
         * @brief 解析请求头部.
         *
         * @param szBuffer 请求头部
         * @return size_t
         */
        size_t parseRequestHeader(const char *szBuffer);

        /**
         * @brief 请求类型到字符串.
         *
         * @param iRequestType  请求
         * @return              解析后的字符串
         */
        string requestType2str(int iRequestType) const;

    protected:

        /**
         * @brief 对http请求编码.
         *
         * @param sUrl         需要进行编码的http请求
         * @param iRequestType 编码后的输出流
         * @return void
         */
        void encode(int iRequestType, ostream &os);

        /**
         * @brief 解析URL
         */
        void parseURL(const string &sUrl);

    protected:

        /**
         * 请求URL
         */
        Vin_URL _httpURL;

        /**
         * 请求类型
         */
        int _requestType;
    };
}


#endif //CLOG_VIN_HTTP_H
