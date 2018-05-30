//
// Created by cyz on 5/28/18.
//

#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>
#include <util/include/Vin_ClientSocket.h>
#include "util/include/Vin_Http.h"
#include "util/include/Vin_Tools.h"

namespace vince {
/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_URL
    Vin_URL::Vin_URL() : _iURLType(HTTP) {

    }

    bool Vin_URL::parseURL(const string &sURL) {
        string originRequest = Vin_Tools::trim(sURL, " ");

        if (originRequest.empty()) {
            return false;
        }

        clear();

        string::size_type iPos = 0;

        if (strncasecmp(originRequest.c_str(), "http://", 7) == 0) {
            //http开头
            _iURLType = HTTP;
            iPos = 7;
            _sScheme = "http";
        } else if (strncasecmp(originRequest.c_str(), "https://", 8) == 0) {
            //https开头
            _iURLType = HTTPS;
            iPos = 8;
            _sScheme = "https";
        } else if (strncasecmp(originRequest.c_str(), "ftp://", 6) == 0) {
            //ftps开头
            _iURLType = FTP;
            iPos = 6;
            _sScheme = "ftp";
        } else {
            //默认用http
            _iURLType = HTTP;
            iPos = 0;
            _sScheme = "http";
        }

        string::size_type index = originRequest.find('/', iPos);
        string::size_type nQuestionIndex = originRequest.find('?', iPos);
        string::size_type nNumbersignIndex = originRequest.find('#', iPos);

        string sUrlAuthority;
        string sUrlPath;

        if (nQuestionIndex < index) {
            sUrlAuthority = originRequest.substr(iPos, nQuestionIndex - iPos);

            string sTemp = originRequest.substr(nQuestionIndex);

            sUrlPath += '/';
            sUrlPath += sTemp;
        } else if (nNumbersignIndex < index) {
            sUrlAuthority = originRequest.substr(iPos, nNumbersignIndex - iPos);

            string sTemp = originRequest.substr(nNumbersignIndex);

            sUrlPath += '/';
            sUrlPath += sTemp;
        } else {
            if (index == string::npos) {
                sUrlAuthority = originRequest.substr(iPos, index);
                sUrlPath = "";
            } else {
                sUrlAuthority = originRequest.substr(iPos, index - iPos);
                sUrlPath = originRequest.substr(index);
            }
        }

        //////解析Authority
        index = sUrlAuthority.find('@');
        if (index != string::npos) {
            _sUser = sUrlAuthority.substr(0, index);
            _sDomain = sUrlAuthority.substr(index + 1);
        } else {
            _sDomain = sUrlAuthority;
        }

        //////解析User:Pass
        index = _sUser.find(':');
        if (index != string::npos) {
            _sPass = _sUser.substr(index + 1);
            _sUser = _sUser.substr(0, index);
        }

        //////解析Host:Port
        index = _sDomain.find(':');
        if (index != string::npos) {
            _sPort = _sDomain.substr(index + 1);

            _sDomain = _sDomain.substr(0, index);
        } else {
            _sPort = _getDefaultPort();
        }

        //////解析Path Query Ref
        index = sUrlPath.find('?');
        if (index != string::npos) {
            _sPath = sUrlPath.substr(0, index);
            _sQuery = sUrlPath.substr(index + 1);

        } else {
            _sPath = sUrlPath;
            _sQuery = "";

        }

        index = _sQuery.rfind('#');
        if (index != string::npos) {
            _sRef = _sQuery.substr(index + 1);
            _sQuery = _sQuery.substr(0, index);
        }

        if (_sPath.empty()) {
            _sPath = "/";
        }

        _toURL();

        //域名或者IP必须包含一个点
        return _sDomain.find('.') != string::npos;
    }

    void Vin_URL::clear() {
        _sScheme = "";
        _sUser = "";
        _sPass = "";
        _sDomain = "";
        _sPort = "";
        _sPath = "";
        _sQuery = "";
        _sRef = "";
        _sURL = "";
    }

    string Vin_URL::getScheme() const {
        return _sScheme;
    }

    string Vin_URL::getUserName() const {
        return _sUser;
    }

    string Vin_URL::getPassword() const {
        return _sPass;
    }

    string Vin_URL::getDomain() const {
        return _sDomain;
    }

    string Vin_URL::getPort() const {
        return _sPort;
    }

    bool Vin_URL::isDefaultPort() const {
        return _sPort == _getDefaultPort();
    }

    string Vin_URL::getPath() const {
        return _sPath;
    }

    string Vin_URL::getQuery() const {
        return _sQuery;
    }

    string Vin_URL::getRequest() const {
        string sURL;

        if (!_sPath.empty())
            sURL += _sPath;

        if (!_sQuery.empty())
            sURL += "?" + _sQuery;

        if (!_sRef.empty())
            sURL += "#" + _sRef;

        return sURL;
    }

    string Vin_URL::getRef() const {
        return _sRef;
    }

    bool Vin_URL::isValid() const {
        return !_sURL.empty();
    }

    string Vin_URL::getURL() const {
        return _sURL;
    }

    int Vin_URL::getType() const {
        return _iURLType;
    }

    string Vin_URL::getRelativePath() const {
        string sURL = getPath();

        string::size_type pos;

        pos = sURL.rfind('/');

        if (pos == string::npos) {
            return "/";
        } else {
            return sURL.substr(0, pos + 1);
        }
    }

    string Vin_URL::getRootPath() const {
        string sURL = _sScheme;
        sURL += "://";

        if (!_sUser.empty())
            sURL += _sUser;

        if (!_sUser.empty() && !_sPass.empty()) {
            sURL += ":";
            sURL += _sPass;
        }

        if (!_sUser.empty())
            sURL += "@";

        sURL += _sDomain;

        if (!isDefaultPort()) {
            sURL += ":";
            sURL += _sPort;
        }

        sURL += "/";

        return sURL;
    }

    Vin_URL Vin_URL::buildWithRelativePath(const string &sRelativeURL) const {
        Vin_URL retURL;

        if (sRelativeURL.empty())
            return retURL;

        string sNewPath;

        if (sRelativeURL[0] == '/') {
            sNewPath = _simplePath(sRelativeURL);
        } else {
            sNewPath = _simplePath(_sPath + "/" + sRelativeURL);
        }

        retURL = *this;
        retURL._sPath = sNewPath.empty() ? "/" : sNewPath;
        retURL._toURL();

        return retURL;
    }

    void Vin_URL::specialize() {
        //规整化路径
        _sPath = _simplePath(_sPath);
        _sURL = _toURL();
    }

    string Vin_URL::_getDefaultPort() const {
        switch (_iURLType) {
            case HTTP:
                return "80";
            case HTTPS:
                return "443";
            case FTP:
                return "21";
        }

        return "80";
    }

    string Vin_URL::_type2String() const {
        switch (_iURLType) {
            case HTTP:
                return "http";
            case HTTPS:
                return "https";
            case FTP:
                return "ftp";
        }

        return "http";
    }

    string Vin_URL::_toURL() {
        _sURL.clear();

        _sURL = _sScheme;
        _sURL += "://";

        if (!_sUser.empty())
            _sURL += _sUser;

        if (!_sUser.empty() && !_sPass.empty()) {
            _sURL += ":";
            _sURL += _sPass;
        }

        if (!_sUser.empty())
            _sURL += "@";

        _sURL += _sDomain;

        if (!isDefaultPort()) {
            _sURL += ":";
            _sURL += _sPort;
        }

        _sURL += getRequest();

        return _sURL;
    }

    string Vin_URL::_simplePath(const string &sPath) const {
        string sNewPath;
        deque<string> stPath;
        string::size_type lastPos = 0, nowPos = 0;

        while (lastPos < sPath.length()) {
            string slot;
            if ((nowPos = sPath.find('/', lastPos)) == string::npos) {
                slot = sPath.substr(lastPos);

                if (slot == "..") {
                    if (stPath.empty())
                        throw Vin_URL_Exception("[Vin_URL::_simplePath] Wrong Path");
                    stPath.pop_back();
                } else if (slot == ".") { ;
                } else {
                    stPath.push_back(slot);
                }
                break;
            }

            if (nowPos == 0 && lastPos == 0) {
                lastPos++;
                continue;
            }

            slot = sPath.substr(lastPos, nowPos - lastPos);

            if (slot == "..") {
                if (stPath.empty())
                    throw Vin_URL_Exception("[Vin_URL::_simplePath] Wrong Path");
                stPath.pop_back();
            } else if (slot == ".") { ;
            } else {
                stPath.push_back(slot);
            }

            lastPos = nowPos + 1;
        }

        while (!stPath.empty()) {
            sNewPath += "/" + stPath.front();
            stPath.pop_front();
        }

        return sNewPath;
    }

/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_Http

    Vin_Http::Vin_Http() {
        reset();
        setConnection("close");//默认就用短连接
    }

    void Vin_Http::eraseHeader(const string &sHeader) {
        _headers.erase(sHeader);
    }

    void Vin_Http::setHeader(const string &sHeadName, const string &sHeadValue) {
        //Set-Cookie和Cookie可以有多个头
        const char *pStr1 = "SET-COOKIE";
        const char *pStr2 = "COOKIE";//原则上COOKIE只有一个，担心有兼容性问题，保留
        string sUpperHeadName = Vin_Tools::upper(sHeadName);

        if ((strcmp(sUpperHeadName.c_str(), pStr1) != 0) && (strcmp(sUpperHeadName.c_str(), pStr2) != 0)) {
            _headers.erase(sHeadName);
        }

        _headers.insert(multimap<string, string>::value_type(sUpperHeadName, sHeadValue));
    }

    void Vin_Http::setCacheControl(const string &sCacheControl) {
        setHeader("CACHE-CONTROL", sCacheControl);
    }

    void Vin_Http::setConnection(const string &sConnection) {
        setHeader("CONNECTION", sConnection);
    }

    void Vin_Http::setContentType(const string &sContentType) {
        setHeader("CONTENT-TYPE", sContentType);
    }

    void Vin_Http::setContentLength(size_t iContentLength) {
        setHeader("CONTENT-LENGTH", to_string(iContentLength));
    }

    void Vin_Http::setReferer(const string &sReferer) {
        setHeader("REFERER", sReferer);
    }

    void Vin_Http::setHost(const string &sHost) {
        setHeader("HOST", sHost);
    }

    void Vin_Http::setAcceptEncoding(const string &sAcceptEncoding) {
        setHeader("ACCEPT-ENCODING", sAcceptEncoding);
    }

    void Vin_Http::setAcceptLanguage(const string &sAcceptLanguage) {
        setHeader("ACCEPT-LANGUAGE", sAcceptLanguage);
    }

    void Vin_Http::setAccept(const string &sAccept) {
        setHeader("ACCEPT", sAccept);
    }

    void Vin_Http::setTransferEncoding(const string &sTransferEncoding) {
        setHeader("TRANSFER-ENCODING", sTransferEncoding);
    }

    void Vin_Http::setHeaderMulti(const string &sHeadName, const string &sHeadValue) {
        _headers.insert(multimap<string, string>::value_type(sHeadName, sHeadValue));
    }

    string Vin_Http::getHeader(const string &sHeadName) const {
        string sUpperHeadName = Vin_Tools::upper(sHeadName);

        auto it = _headers.find(sUpperHeadName);
        if (it == _headers.end()) {
            return "";
        }

        return it->second;
    }

    vector<string> Vin_Http::getHeaderMulti(const string &sHeadName) const {
        vector<string> ret;
        for (auto it = _headers.lower_bound(sHeadName); it != _headers.upper_bound(sHeadName); it++) {
            ret.push_back(it->second);
        }
        return ret;
    }

    size_t Vin_Http::getContentLength() const {
        string ret = getHeader("CONTENT-LENGTH");
        if (ret.empty())
            return 0;
        else
            return (size_t) stoi(ret);
    }

    string Vin_Http::getHost() const {
        return getHeader("HOST");
    }

    string Vin_Http::getContentType() const {
        return getHeader("CONTENT-TYPE");
    }

    size_t Vin_Http::getHeadLength() const {
        return _headLength;
    }

    string Vin_Http::getContent() const {
        return _content;
    }

    void Vin_Http::setContent(const string &content, bool bUpdateContentLength) {
        _content = content;

        if (bUpdateContentLength) {
            eraseHeader("Content-Length");
            setContentLength(_content.length());
        }
    }

    const Vin_Http::http_header_type &Vin_Http::getHeaders() const {
        return _headers;
    }

    void Vin_Http::reset() {
        _headers.clear();
        _headLength = 0;
        _content.clear();
        _bIsChunked = false;
    }

    string Vin_Http::getLine(const char **ppChar) {
        string sTmp;

        sTmp.reserve(512);

        while ((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0') {
            sTmp.append(1, (**ppChar));
            (*ppChar)++;
        }

        if ((**ppChar) == '\r') {
            (*ppChar)++;   /* pass the char '\n' */
        }

        (*ppChar)++;

        return sTmp;
    }

    /// @cyz TODO
    string Vin_Http::getLine(const char **ppChar, int iBufLen) {
        string sTmp;

        sTmp.reserve(512);

        int iCurIndex = 0;
        while ((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0') {
            if (iCurIndex < iBufLen) {
                sTmp.append(1, (**ppChar));
                (*ppChar)++;
                iCurIndex++;
            } else {
                //MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 1 " << endl;
                break;
            }
        }

        if ((**ppChar) == '\r') {
            if (iCurIndex < iBufLen) {
                (*ppChar)++;   /* pass the char '\n' */
                iCurIndex++;
            } else {
                //MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 2 " << endl;
            }
        }

        if (iCurIndex < iBufLen) {
            (*ppChar)++;
            iCurIndex++;
        } else {
            //MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 3 " << endl;
        }

        return sTmp;
    }

    string Vin_Http::genHeader() const {
        string sHttpHeader;

        for (auto it = _headers.begin(); it != _headers.end(); ++it) {
            if (it->second != "") {
                sHttpHeader += it->first;
                sHttpHeader += ": ";
                sHttpHeader += it->second;
                sHttpHeader += "\r\n";
            }
        }

        return sHttpHeader;
    }

    bool Vin_Http::isChunked() const {
        return _bIsChunked;
    }

    const char *Vin_Http::parseHeader(const char *szBuffer, Vin_Http::http_header_type &sHeader) {
        sHeader.clear();

        const char **ppChar = &szBuffer;

        long length = strlen(szBuffer);
        long srcPtr = (long) (*ppChar);

        while (true) {
            string sLine = getLine(ppChar);

            if (sLine.empty())
                break;

            //如果是第一行, 则忽略掉
            if (strncasecmp(sLine.c_str(), "GET ", 4) == 0
                || strncasecmp(sLine.c_str(), "POST ", 5) == 0
                || strncasecmp(sLine.c_str(), "OPTIONS ", 8) == 0
                || strncasecmp(sLine.c_str(), "HEAD ", 5) == 0
                || strncasecmp(sLine.c_str(), "HTTP/", 5) == 0) {
                continue;
            }

            string::size_type index = sLine.find(":");
            if (index != string::npos) {
                string sTmp = Vin_Tools::upper(Vin_Tools::trim(sLine.substr(0, index), " "));
                sHeader.insert(
                        multimap<string, string>::value_type(sTmp, Vin_Tools::trim(sLine.substr(index + 1), " ")));
            }

            long offset = (long) (*ppChar - srcPtr);

            if (offset >= length) {
                break;
            }
        }

        return *ppChar;
    }

/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_HttpCookie

    bool Vin_HttpCookie::matchDomain(const string &sCookieDomain, const string &sDomain) {
        string sLowerCookieDomain = Vin_Tools::lower(sCookieDomain);

        string sFixedLowerCookieDomain;
        if (!fixDomain(sLowerCookieDomain, sFixedLowerCookieDomain)) {
            return false;
        }

        string sLowerDomain = Vin_Tools::lower(sDomain);

        if (sFixedLowerCookieDomain.substr(1) == sLowerDomain)
            return true;

        if ((sLowerDomain.length() > sFixedLowerCookieDomain.length()
             && (sLowerDomain.compare(sLowerDomain.find('.'),
                                      sFixedLowerCookieDomain.length(), sFixedLowerCookieDomain) == 0))) {
            return true;
        }

        return false;
    }

    void Vin_HttpCookie::clear() {
        _cookies.clear();
    }

    bool Vin_HttpCookie::fixDomain(const string &sDomain, string &sFixDomain) {
        if (sDomain.empty())
            return false;

        sFixDomain = sDomain;

        //自动加.
        if (sDomain.at(0) != '.') {
            sFixDomain = "." + sDomain;
        }

        //domain至少两段
        if (sFixDomain.find('.') == sFixDomain.rfind('.'))
            return false;

        return true;
    }

    size_t Vin_HttpCookie::matchPath(const string &sCookiePath, const string &sPath) {
        if (sCookiePath.empty() || sPath.empty())
            return 0;

        //都在最后加/再匹配
        string sCookiePath1 = (sCookiePath.at(sCookiePath.length() - 1) == '/') ? sCookiePath : sCookiePath + "/";

        string sPath1 = (sPath.at(sPath.length() - 1) == '/') ? sPath : sPath + "/";

        if (sPath1.find(sCookiePath1) == 0) {
            return sCookiePath1.length();
        }

        return 0;
    }

    void Vin_HttpCookie::addCookie(const Vin_HttpCookie::Cookie &cookie, list<Vin_HttpCookie::Cookie> &cookies) {
        string sDomain;

        Cookie cookieNew = cookie;

        if (!fixDomain(cookieNew._domain, sDomain))
            return;

        cookieNew._domain = sDomain;

        if (cookieNew._path.empty())
            return;

        auto it = cookies.begin();

        string sName;
        if (cookieNew._data.size() >= 1) {
            sName = cookieNew._data.begin()->first;
        }

        auto fnIsCookiesEqual = [](Cookie &ck1, Cookie &ck2) -> bool {
            return strcasecmp(ck1._domain.c_str(), ck2._domain.c_str()) == 0
                   && strcmp(ck1._path.c_str(), ck2._path.c_str()) == 0
                   && ck1._isSecure == ck2._isSecure;
        };

        while (it != cookies.end()) {
            //检查Cookie是否过期
            if (isCookieExpires(*it)) {
                cookies.erase(it++);
            } else if (fnIsCookiesEqual(*it, cookieNew)) {
                if (it->_expires == cookieNew._expires) {
                    //domain/path/expires都匹配的情况下, 覆盖老cookie的数据
                    cookieNew._data.insert(it->_data.begin(), it->_data.end());

                    cookies.erase(it++);
                } else {
                    //超时间不一样,但存在同样的key，需删除
                    if (it->_data.find(sName) != it->_data.end()) {
                        it->_data.erase(sName);
                    }
                    ++it;
                }
            } else {
                ++it;
            }
        }

        cookies.push_back(cookieNew);
    }

    bool Vin_HttpCookie::isCookieExpires(const Vin_HttpCookie::Cookie &cookie, time_t t) const {
        return cookie._expires != 0 && cookie._expires < t;
    }

    void Vin_HttpCookie::addCookie(const Vin_HttpCookie::Cookie &cookie) {
        addCookie(cookie, _cookies);
    }

    void Vin_HttpCookie::addCookie(const list<Vin_HttpCookie::Cookie> &cookie) {
        auto it = cookie.begin();

        while (it != cookie.end()) {
            addCookie(*it);
            ++it;
        }
    }

    void Vin_HttpCookie::addCookie(const string &sCookieRspURL, const vector<string> &vCookies) {
        Vin_URL cURL;

        cURL.parseURL(sCookieRspURL);

        string sRspURLDomain = Vin_Tools::lower(cURL.getDomain());

        string sRspURLPath = cURL.getPath();

        for (size_t i = 0; i < vCookies.size(); i++) {
            //处理一行SetCookie
            vector<string> v = Vin_Tools::sepstr<string>(vCookies[i], ";");

            Cookie cookie;

            cookie._isSecure = false;
            cookie._expires = 0;

            //解析Cookie数据
            for (size_t j = 0; j < v.size(); ++j) {
                string::size_type index = v[j].find("=");

                string name;
                string value;

                if (index != string::npos) {
                    name = Vin_Tools::trim(v[j].substr(0, index), " ");
                    value = Vin_Tools::trim(v[j].substr(index + 1));
                } else {
                    name = Vin_Tools::trim(v[j]);
                }

                if (strcasecmp(name.c_str(), "secure") == 0) {
                    cookie._isSecure = true;
                } else if (strcasecmp(name.c_str(), "expires") == 0) {
                    tm stTm;
                    //兼容时间格式
                    //expires=Mon, 09-May-41 08:39:32 GMT
                    //expires=Thu, 01-Jan-1970 01:00:00 GMT
                    value = Vin_Tools::replace(value, "-", " ");
                    if (value.length() == 27 && value.at(11) == ' ' && value.at(14) == ' ') {
                        int year = Vin_Tools::strto<int>(value.substr(12, 2));
                        if (year >= 69 && year <= 99)
                            value = value.substr(0, 12) + "19" + value.substr(12);
                        else
                            value = value.substr(0, 12) + "20" + value.substr(12);
                    }

                    Vin_Tools::strgmt2tm(value, stTm);

                    cookie._expires = timegm(&stTm);
                } else if (strcasecmp(name.c_str(), "path") == 0) {
                    cookie._path = value;
                } else if (strcasecmp(name.c_str(), "domain") == 0) {
                    cookie._domain = value;
                } else if (strcasecmp(name.c_str(), "httponly") == 0) {
                    //TODO
                    //cookie._isHttpOnly = true;
                } else
                    cookie._data.insert(http_cookie_data::value_type(name, value));
            }

            ///修正和匹配domain/////////////////////////////////////////
            if (cookie._domain.empty())
                cookie._domain = sRspURLDomain;

            if (!fixDomain(cookie._domain, cookie._domain))
                continue;

            //匹配域名
            if (!matchDomain(cookie._domain, sRspURLDomain))
                continue;

            //修改和匹配path/////////////////////////////
            if (cookie._path.empty()) {
                string sCookiePath;

                //缺省是全路径
                string sRequest = sRspURLPath;

                string::size_type pos = sRequest.rfind("/");

                if (pos == string::npos)
                    sCookiePath = "/";
                else
                    sCookiePath = sRequest.substr(0, pos + 1);

                cookie._path = sCookiePath;
            }

            //URL在Path范围内,Cookie 有效
            if (!matchPath(cookie._path, sRspURLPath))
                continue;

            //添加Cookie
            addCookie(cookie);
        }
    }

    size_t Vin_HttpCookie::lenCookieMatch(const Vin_HttpCookie::Cookie &cookie, const Vin_URL &tURL) const {
        //域名没有匹配
        if (!matchDomain(cookie._domain, tURL.getDomain()))
            return 0;

        //路径没有匹配
        size_t len = matchPath(cookie._path, tURL.getPath());
        if (len == 0)
            return 0;

        //安全的cookie,不安全的URL
        if (cookie._isSecure && (tURL.getType() != Vin_URL::HTTPS))
            return 0;

        return len;
    }

    void Vin_HttpCookie::getCookieForURL(const string &sReqURL, list<Vin_HttpCookie::Cookie> &cookies) {
        Vin_URL tURL;

        tURL.parseURL(sReqURL);

        cookies.clear();

        auto it = _cookies.begin();

        while (it != _cookies.end()) {
            //检查Cookie是否过期
            if (isCookieExpires(*it)) {
                _cookies.erase(it++);
                continue;
            }

            size_t len = lenCookieMatch(*it, tURL);
            if (len == 0) {
                ++it;
                continue;
            }

            cookies.push_back(*it);

            ++it;
        }
    }

    void Vin_HttpCookie::getCookieForURL(const string &sReqURL, string &sCookie) {
        list<Cookie> cookies;

        sCookie.clear();

        getCookieForURL(sReqURL, cookies);

        auto it = cookies.begin();
        while (it != cookies.end()) {
            auto itd = it->_data.begin();

            while (itd != it->_data.end()) {
                //被删除的cookie不输出
                if (itd->first != "" && itd->second != "" && Vin_Tools::lower(itd->second) != "null"
                    && Vin_Tools::lower(itd->second) != "deleted")
                    sCookie += itd->first + "=" + itd->second + "; ";

                ++itd;
            }

            ++it;
        }

        //去掉末尾的 "; "
        if (sCookie.length() >= 2)
            sCookie = sCookie.substr(0, sCookie.length() - 2);
    }

    void Vin_HttpCookie::deleteExpires(time_t t, bool bErase) {
        for (auto it = _cookies.begin(); it != _cookies.end(); it++) {
            if ((bErase && it->_expires == 0) || isCookieExpires(*it, t)) {
                _cookies.erase(it);
            } else {
                it;
            }
        }
    }

    list<Vin_HttpCookie::Cookie> Vin_HttpCookie::getAllCookie() {
        deleteExpires(time(NULL));

        return _cookies;
    }

    list<Vin_HttpCookie::Cookie> Vin_HttpCookie::getSerializeCookie(time_t t) {
        list<Cookie> cookies;

        deleteExpires(t);

        for (auto it = _cookies.begin(); it != _cookies.end(); it++) {
            if (it->_expires != 0) {
                cookies.push_back(*it);
            }
        }

        return cookies;
    }

/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_HttpResponse

    void Vin_HttpResponse::reset() {
        Vin_Http::reset();

        _status = 200;
        _about = "OK";
        _version = "HTTP/1.1";;

        _iTmpContentLength = 0;
    }

    void Vin_HttpResponse::parseResponseHeader(const char *szBuffer) {
        const char **ppChar = &szBuffer;

        _headerLine = Vin_Tools::trim(getLine(ppChar));

        string::size_type pos = _headerLine.find(' ');

        if (pos != string::npos) {
            _version = _headerLine.substr(0, pos);

            string left = Vin_Tools::trim(_headerLine.substr(pos));

            string::size_type pos1 = left.find(' ');

            if (pos1 != string::npos) {
                _status = Vin_Tools::strto<int>(left.substr(0, pos));

                _about = Vin_Tools::trim(left.substr(pos1 + 1));
            } else {
                _status = Vin_Tools::strto<int>(left);

                _about = "";
            }

            parseHeader(*ppChar, _headers);
            return;
        } else {
            _version = _headerLine;
            _status = 0;
            _about = "";
        }
    }

    bool Vin_HttpResponse::incrementDecode(string &sBuffer) {
        //解析头部
        if (_headLength == 0) {
            string::size_type pos = sBuffer.find("\r\n\r\n");

            if (pos == string::npos) {
                return false;
            }

            parseResponseHeader(sBuffer.c_str());

            if (_status == 204) {
                return true;
            }

            auto it = _headers.find("Content-Length");
            if (it != _headers.end()) {
                _iTmpContentLength = getContentLength();
            } else {
                //没有指明ContentLength, 接收到服务器关闭连接
                _iTmpContentLength = (size_t) -1;
            }

            _headLength = pos + 4;

            sBuffer = sBuffer.substr(_headLength);

            //重定向就认为成功了
            if ((_status == 301 || _status == 302) && !getHeader("Location").empty()) {
                return true;
            }

            //是否是chunk编码
            _bIsChunked = (getHeader("Transfer-Encoding") == "chunked");

            //删除头部里面
            eraseHeader("Transfer-Encoding");
        }

        if (_bIsChunked) {
            while (true) {
                string::size_type pos = sBuffer.find("\r\n");
                if (pos == string::npos)
                    return false;

                //查找当前chunk的大小
                string sChunkSize = sBuffer.substr(0, pos);
                long iChunkSize = strtol(sChunkSize.c_str(), NULL, 16);//chunk size是以十六进制的ASCII码表示

                if (iChunkSize <= 0)
                    break;      //所有chunk都接收完毕

                if (sBuffer.length() >= pos + 2 + (size_t) iChunkSize + 2)   //接收到一个完整的chunk了
                {
                    //获取一个chunk的内容
                    _content += sBuffer.substr(pos + 2, iChunkSize);

                    //删除一个chunk
                    sBuffer = sBuffer.substr(pos + 2 + iChunkSize + 2);
                } else {
                    //没有接收完整的chunk
                    return false;
                }

                setContentLength(getContent().length());
            }

            sBuffer = "";

            if (_iTmpContentLength == 0 || _iTmpContentLength == (size_t) -1) {
                setContentLength(getContent().length());
            }

            return true;
        } else {
            if (_iTmpContentLength == 0) {
                _content += sBuffer;
                sBuffer = "";

                //自动填写content-length
                setContentLength(getContent().length());

                return true;
            } else if (_iTmpContentLength == (size_t) -1) {
                //304的返回码中头没有Content-Length，不会有数据体
                if (_status == 304) {
                    return true;
                }

                _content += sBuffer;
                sBuffer = "";

                //自动填写content-length
                setContentLength(getContent().length());

                return false;
            } else {
                //chang连接模式, 接收到长度大于头部为止
                _content += sBuffer;
                sBuffer = "";

                size_t iNowLength = getContent().length();

                //头部的长度小于接收的内容, 还需要继续增加解析后续的buffer
                if (iNowLength < _iTmpContentLength)
                    return false;

                return true;
            }
        }
    }

    bool Vin_HttpResponse::decode(const string &sBuffer) {
        string::size_type pos = sBuffer.find("\r\n\r\n");

        if (pos == string::npos) {
            return false;
        }

        string tmp = sBuffer;

        incrementDecode(tmp);

        //body内容长度为0或者没有content-length  且非chunk模式, 则认为包收全了, 直接返回
        if ((_iTmpContentLength == 0 || _iTmpContentLength == (size_t) -1) && !_bIsChunked)
            return true;

        return getContentLength() + getHeadLength() <= sBuffer.length();
    }

    bool Vin_HttpResponse::decode(const char *sBuffer, size_t iLength) {
        assert(sBuffer != NULL);

        string tmp(sBuffer, iLength);

        return decode(tmp);
    }

    string Vin_HttpResponse::encode() const {
        string sRet;

        sRet += _version;
        sRet += " ";
        sRet += to_string(_status);
        sRet += " ";
        sRet += _about;
        sRet += "\r\n";

        sRet += genHeader();
        sRet += "\r\n";

        sRet += _content;

        return sRet;
    }

    void Vin_HttpResponse::encode(vector<char> &vBuffer) const {
        vBuffer.clear();

        string s = encode();

        vBuffer.resize(s.length());
        memcpy(&vBuffer[0], s.c_str(), s.length());
    }

    string Vin_HttpResponse::getResponseHeaderLine() const {
        return _headerLine;
    }

    int Vin_HttpResponse::getStatus() const {
        return _status;
    }

    void Vin_HttpResponse::setStatus(int status) {
        _status = status;
    }

    string Vin_HttpResponse::getAbout() const {
        return _about;
    }

    void Vin_HttpResponse::setAbout(const string &about) {
        _about = about;
    }

    string Vin_HttpResponse::getVersion() const {
        return _version;
    }

    void Vin_HttpResponse::setVersion(const string &sVersion) {
        _version = sVersion;
    }

    void Vin_HttpResponse::setResponse(int status, const string &about, const string &body) {
        _status = status;
        _about = about;
        _content = body;

        _headerLine = "";
        _headerLine += _version;
        _headerLine += " ";
        _headerLine += to_string(_status);
        _headerLine += " ";
        _headerLine += _about;

        setHeader("Content-Length", to_string(_content.length()));
    }

    void Vin_HttpResponse::setResponse(int status, const string &about, const char *sBuffer, size_t iLength) {
        string body(sBuffer, iLength);
        setResponse(status, about, body);
    }

    void Vin_HttpResponse::setResponse(const char *sBuffer, size_t iLength) {
        setResponse(200, "OK", sBuffer, iLength);
    }

    void Vin_HttpResponse::setServer(const string &sServer) {
        setHeader("Server", sServer);
    }

    void Vin_HttpResponse::setSetCookie(const string &sCookie) {
        setHeader("Set-Cookie", sCookie);
    }

    vector<string> Vin_HttpResponse::getSetCookie() const {
        return getHeaderMulti("Set-Cookie");
    }

/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_HttpRequest

    Vin_HttpRequest::Vin_HttpRequest() {
        Vin_HttpRequest::reset();
        setUserAgent("Vin_Http");
    }

    void Vin_HttpRequest::reset() {
        Vin_Http::reset();
        _httpURL.clear();
    }

    string Vin_HttpRequest::requestType2str(int iRequestType) const {
        string ret;
        switch (iRequestType) {
            case REQUEST_GET:
                ret = "GET";
                break;
            case REQUEST_POST:
                ret = "POST";
                break;
            case REQUEST_PUT:
                ret = "PUT";
                break;
            case REQUEST_HEAD:
                ret = "HEAD";
                break;
            case REQUEST_OPTIONS:
                ret = "OPTIONS";
                break;
            case REQUEST_DELETE:
                ret = "DELETE";
                break;
            default:
                ret = "";
        }
        return ret;
    }

    vector<string> Vin_HttpRequest::getCookie() {
        vector<string> v;

        for (auto it = _headers.begin(); it != _headers.end(); ++it) {
            if (it->first == "Cookie") {
                v.push_back(it->second);
            }
        }

        return v;
    }

    void Vin_HttpRequest::parseURL(const string &sUrl) {
        _httpURL.parseURL(sUrl);

        //设置Host
        if (getHeader("Host").empty()) {
            //缺省端口可以不用放进去
            string sPort = _httpURL.isDefaultPort() ? "" : ":" + _httpURL.getPort();
            setHost(_httpURL.getDomain() + sPort);
        }
    }

    void Vin_HttpRequest::encode(int iRequestType, ostream &os) {
        os << requestType2str(iRequestType) << " " << _httpURL.getRequest() << " HTTP/1.1\r\n";
        os << genHeader() << "\r\n";
    }

    void Vin_HttpRequest::setGetRequest(const string &sUrl, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_GET;
        _content = "";

        eraseHeader("Content-Length");
    }

    void Vin_HttpRequest::setHeadRequest(const string &sUrl, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_HEAD;
        _content = "";

        eraseHeader("Content-Length");
    }

    void Vin_HttpRequest::setPostRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_POST;
        _content = sPostBody;

        setHeader("Content-Length", to_string(_content.length()));
    }

    void Vin_HttpRequest::setPostRequest(const string &sUrl, const char *sBuffer, size_t iLength, bool bNewCreateHost) {
        string sPostBody(sBuffer, iLength);
        setPostRequest(sUrl, sPostBody, bNewCreateHost);
    }

    void Vin_HttpRequest::setPutRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_PUT;
        _content = sPostBody;

        setHeader("Content-Length", to_string(_content.length()));
    }

    void Vin_HttpRequest::setDeleteRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_DELETE;
        _content = sPostBody;

        setHeader("Content-Length", to_string(_content.length()));
    }

    void Vin_HttpRequest::setOptionsRequest(const string &sUrl, bool bNewCreateHost) {
        if (bNewCreateHost) {
            eraseHeader("Host");
        }

        parseURL(sUrl);
        _requestType = REQUEST_OPTIONS;
        _content = "";

        eraseHeader("Content-Length");
    }

    string Vin_HttpRequest::encode() {
        string sRet;

        if (_requestType == REQUEST_POST || _requestType == REQUEST_PUT || _requestType == REQUEST_DELETE)
            setContentLength(_content.length());

        sRet += requestType2str(_requestType);
        sRet += " ";
        sRet += _httpURL.getRequest();
        sRet += " HTTP/1.1\r\n";
        sRet += genHeader();
        sRet += "\r\n";

        return sRet;
    }

    void Vin_HttpRequest::encode(vector<char> &buffer) {
        buffer.clear();

        string s = encode();
        buffer.resize(s.length());
        memcpy(&buffer[0], s.c_str(), s.length());
    }

    bool Vin_HttpRequest::decode(const string &sBuffer) {
        return decode(sBuffer.c_str(), sBuffer.length());
    }

    bool Vin_HttpRequest::decode(const char *sBuffer, size_t iLength) {
        assert(sBuffer != nullptr);

        if (strncasecmp(sBuffer, "GET ", 4) != 0 && strncasecmp(sBuffer, "POST ", 5) != 0 &&
            strncasecmp(sBuffer, "OPTIONS ", 8) != 0 && strncasecmp(sBuffer, "HEAD ", 5) != 0 &&
            strncasecmp(sBuffer, "DELETE ", 7) != 0 && strncasecmp(sBuffer, "PUT ", 4) != 0) {
            throw Vin_HttpRequest_Exception(
                    "[TC_HttpRequest::checkRequest] protocol not support, only support GET HEAD POST and OPTIONS ");
        }

        if (strstr(sBuffer, "\r\n\r\n") == NULL) {
            return false;
        }

        _headLength = parseRequestHeader(sBuffer);

        bool bChunk = (getHeader("Transfer-Encoding") == "chunked");

        if (bChunk) {
            string sTmp(sBuffer + _headLength, iLength - _headLength);
            while (true) {
                string::size_type pos = sTmp.find("\r\n");
                if (pos == string::npos)
                    return false;

                //查找当前chunk的大小
                string sChunkSize = sTmp.substr(0, pos);
                long iChunkSize = strtol(sChunkSize.c_str(), NULL, 16);

                if (iChunkSize <= 0)
                    break;      //所有chunk都接收完毕

                if (sTmp.length() >= pos + 2 + (size_t) iChunkSize + 2)   //接收到一个完整的chunk了
                {
                    //获取一个chunk的内容
                    _content += sTmp.substr(pos + 2, iChunkSize);

                    //删除一个chunk
                    sTmp = sTmp.substr(pos + 2 + iChunkSize + 2);
                } else {
                    //没有接收完整的chunk
                    return false;
                }

                setContentLength(getContent().length());
            }
        } else {
            _content.assign((sBuffer + _headLength), iLength - _headLength);
        }

        return (getContentLength() + getHeadLength() == iLength);
    }

    size_t Vin_HttpRequest::parseRequestHeader(const char *szBuffer) {
        const char *szBuffer_copy = szBuffer;
        const char **ppChar = &szBuffer;

        /* parse the first line and get status */
        string sLine = getLine(ppChar);

        string::size_type pos = sLine.find(" ");
        if (pos == string::npos) {
            throw Vin_HttpRequest_Exception(
                    "[Vin_HttpRequest::parseRequestHeader] http request format error: " + sLine);
        }

        string sMethod = Vin_Tools::trim(sLine.substr(0, pos));
        //解析请求类型
        if (strncasecmp(sMethod.c_str(), "GET", 3) == 0)    //if(sMethod == "GET")
        {
            _requestType = REQUEST_GET;
        } else if (strncasecmp(sMethod.c_str(), "POST", 4) == 0)    //else if(sMethod == "POST")
        {
            _requestType = REQUEST_POST;
        } else if (strncasecmp(sMethod.c_str(), "OPTIONS", 7) == 0)    //else if(sMethod == "OPTIONS")
        {
            _requestType = REQUEST_OPTIONS;
        } else if (strncasecmp(sMethod.c_str(), "HEAD", 4) == 0) {
            _requestType = REQUEST_HEAD;
        } else {
            throw Vin_HttpRequest_Exception(
                    "[Vin_HttpRequest::parseRequestHeader] http request command error: " + sMethod);
        }

        string::size_type pos1 = sLine.rfind(" ");
        if (pos1 == string::npos || pos1 <= pos) {
            throw Vin_HttpRequest_Exception(
                    "[Vin_HttpRequest::parseRequestHeader] http request format error: " + sLine);
        }

        //URL地址
        string sURL = Vin_Tools::trim(sLine.substr(pos + 1, pos1 - pos));

        //HTTP协议版本
        string sVersion = Vin_Tools::upper(Vin_Tools::trim(sLine.substr(pos1 + 1)));

        if (sVersion != "HTTP/1.1" && sVersion != "HTTP/1.0") {
            sVersion = "HTTP/1.1";
        }

        size_t n = parseHeader(*ppChar, _headers) - szBuffer_copy;

        if (strncasecmp(sURL.c_str(), "https://", 8) != 0) {
            if (strncasecmp(sURL.c_str(), "http://", 7) != 0) {
                sURL = "http://" + getHost() + sURL;
            }
        }

        parseURL(sURL);

        return n;
    }

    bool Vin_HttpRequest::checkRequest(const char *sBuffer, size_t iLen) {
        assert(sBuffer != nullptr);

        if (strncasecmp(sBuffer, "GET ", 4) != 0 && strncasecmp(sBuffer, "POST ", 5) != 0 &&
            strncasecmp(sBuffer, "OPTIONS ", 8) != 0 && strncasecmp(sBuffer, "HEAD ", 5) != 0 &&
            strncasecmp(sBuffer, "DELETE ", 7) != 0 && strncasecmp(sBuffer, "PUT ", 4) != 0) {
            throw Vin_HttpRequest_Exception(
                    "[TC_HttpRequest::checkRequest] protocol not support, only support GET HEAD POST and OPTIONS ");
        }

        const char *p = strstr(sBuffer, "\r\n\r\n");
        if (p == nullptr) {
            return false;
        }

        size_t pos = p - sBuffer;

        size_t iHeadLen = pos + 4;

        const char **ppChar = &sBuffer;

        bool bChunk = false;

        //找到\r\n\r\n之前的长度表示
        while (true) {
            size_t iMoveLen = (*ppChar) - sBuffer;
            if (iMoveLen >= iHeadLen) {
                break;
            }

            size_t len = 0;
            string sLine = getLine(ppChar, iHeadLen - iMoveLen);
            if (sLine == "") {
                len = 0;
            } else if (strncasecmp(sLine.c_str(), "Transfer-Encoding:", 18) == 0) {
                bChunk = (Vin_Tools::trim(sLine.substr(18)) == "chunked");
                if (bChunk)
                    break;
            } else if (strncasecmp(sLine.c_str(), "Content-Length:", 15) != 0) {
                continue;
            } else {
                len = Vin_Tools::strto<size_t>(Vin_Tools::trim(sLine.substr(15), " "));
            }

            if (len + pos + 4 <= iLen) {
                return true;
            } else {
                break;

            }
        }

        if (bChunk) {
            size_t remain_len = iLen - iHeadLen;
            size_t move_len = 0;
            const char *pCur = p + 4;
            while (true) {
                p = strstr(pCur, "\r\n");
                if (p == NULL) {
                    return false;
                }

                //查找当前chunk的大小
                long iChunkSize = strtol(string(pCur, p - pCur).c_str(), NULL, 16);
                if (iChunkSize <= 0) {
                    return true; //所有chunk都接收完毕
                }

                move_len = (p - pCur) + 2 + iChunkSize + 2;
                if (remain_len >= move_len)   //接收到一个完整的chunk了
                {
                    //移动到下一个chunk
                    remain_len -= move_len;
                    pCur = p + 2 + iChunkSize + 2;
                } else {
                    return false;
                }
            }
        }

        return false;
    }

    void Vin_HttpRequest::getHostPort(string &sDomain, uint32_t &iPort) {
        sDomain = _httpURL.getDomain();
        iPort = Vin_Tools::strto<uint32_t>(_httpURL.getPort());
    }

    int Vin_HttpRequest::doRequest(Vin_HttpResponse &stHttpRsp, int iTimeout) {
        //只支持短连接模式
        setConnection("close");

        string sSendBuffer = encode();

        string sHost;
        uint32_t iPort;

        getHostPort(sHost, iPort);

        Vin_TCPClient tcpClient;
        tcpClient.init(sHost, iPort, iTimeout);

        int iRet = tcpClient.send(sSendBuffer.c_str(), sSendBuffer.length());
        if (iRet != Vin_TCPClient::EM_SUCCESS) {
            return iRet;
        }

        stHttpRsp.reset();

        string sBuffer;

        size_t iRecvLen = 1024 * 10;
        char *sTmpBuffer = new char[iRecvLen];

        while (true) {
            iRecvLen = 1024 * 10;

            iRet = tcpClient.recv(sTmpBuffer, iRecvLen);

            if (iRet == Vin_TCPClient::EM_SUCCESS)
                sBuffer.append(sTmpBuffer, iRecvLen);

            switch (iRet) {
                case Vin_TCPClient::EM_SUCCESS:
                    if (stHttpRsp.incrementDecode(sBuffer)) {
                        delete[]sTmpBuffer;
                        return Vin_TCPClient::EM_SUCCESS;
                    }
                    continue;
                case Vin_TCPClient::EM_CLOSE:
                    stHttpRsp.incrementDecode(sBuffer);
                    delete[]sTmpBuffer;
                    return Vin_TCPClient::EM_SUCCESS;
                default:
                    delete[]sTmpBuffer;
                    return iRet;
            }
        }
    }
}