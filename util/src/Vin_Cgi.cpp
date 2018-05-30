//
// Created by cyz on 5/22/18.
//

#include <unistd.h>
#include <iostream>
#include "util/include/Vin_Tools.h"
#include "util/include/Vin_Cgi.h"
#include "util/include/Vin_Http.h"

namespace vince {
/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_Cgi_Upload

    ostream &operator<<(ostream &os, const Vin_Cgi_Upload &vinCgiUpload) {
        os << vinCgiUpload.tostr();
        return os;
    }

    Vin_Cgi_Upload::Vin_Cgi_Upload()
            : _sFileName(""), _sRealFileName(""), _sServerFileName(""), _iSize(0), _bOverSize(false) {

    }

    string Vin_Cgi_Upload::tostr() const {
        string sBuffer;

        sBuffer = "[file控件名称:" + _sFileName + "] ";
        sBuffer += " [真实文件名称:" + _sRealFileName + "] ";
        sBuffer += " [服务器端文件名称:" + _sServerFileName + "] ";
        sBuffer += " [大小(字节):" + to_string(_iSize) + "] ";
        sBuffer += " [超过大小限制:" + to_string(_bOverSize) + "] ";

        return sBuffer;
    }

    string Vin_Cgi_Upload::getFormFileName() const {
        return _sFileName;
    }

    string Vin_Cgi_Upload::retRealFileName() const {
        return _sRealFileName;
    }

    string Vin_Cgi_Upload::getSeverFileName() const {
        return _sServerFileName;
    }

    size_t Vin_Cgi_Upload::getFileSize() const {
        return _iSize;
    }

    bool Vin_Cgi_Upload::isOverSize() const {
        return _bOverSize;
    }

/////////////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_Cgi
    Vin_Cgi::Vin_Cgi() : _is(NULL), _iMaxUploadFiles(1), _iUploadMaxSize(20971520), _bOverUploadFiles(false),
                         _iMaxContentLength(20971520), _bUploadFileOverSize(false) {

    }

    Vin_Cgi::~Vin_Cgi() {

    }

    void Vin_Cgi::setUpload(const string &sUploadFilePrefix, int iMaxUploadFiles, size_t iUploadMaxSize,
                            size_t iMaxContentLength) {
        _sUploadFilePrefix = sUploadFilePrefix;
        _iMaxUploadFiles = iMaxUploadFiles;
        _iUploadMaxSize = iUploadMaxSize;
        _iMaxContentLength = iMaxContentLength;
    }

    void Vin_Cgi::parseCgi() {
        char **env = environ;
        while (*env != nullptr) {
            string s(*env);
            string::size_type pos = s.find('=');
            if (pos != string::npos) {
                _env[s.substr(0, pos)] = s.substr(pos + 1);
            }
            ++env;
        }

        _is = &cin;

        readCgiInput(_mmpParams, _mpCookies);
    }

    void Vin_Cgi::parseCgi(const Vin_HttpRequest &request) {
        setCgiEnv("QUERY_STRING", request.getRequestParam());
        setCgiEnv("CONTENT_LENGTH", to_string(request.getContentLength()));
        setCgiEnv("HTTP_COOKIE", request.getHeader("Cookie"));
        setCgiEnv("CONTENT_TYPE", request.getHeader("Content-Type"));
        setCgiEnv("REQUEST_METHOD", request.isGET() ? "GET" : "POST");

        _buffer = request.getContent();
        _iss.str(_buffer);
        _is = &_iss;

        readCgiInput(_mmpParams, _mpCookies);
    }

    const multimap<string, string> &Vin_Cgi::getParamMap() const {
        return _mmpParams;
    }

    map<string, string> Vin_Cgi::getEnvMap() const {
        return _env;
    }

    map<string, string> Vin_Cgi::getParamMapEx() const {
        map<string, string> retMap;
        auto it = _mmpParams.begin();
        while (it != _mmpParams.end()) {
            retMap[it->first] = it->second;
            it++;
        }

        return retMap;
    }

    const map<string, string> &Vin_Cgi::getCookiesMap() const {
        return _mpCookies;
    }

    string &Vin_Cgi::operator[](const string &sName) {
        auto it = _mmpParams.find(sName);
        if (it != _mmpParams.end()) {
            return it->second;
        }
        return _mmpParams.insert(multimap<string, string>::value_type(sName, ""))->second;
    }

    string Vin_Cgi::getValue(const string &sName) const {
        auto it = _mmpParams.find(sName);
        if (it != _mmpParams.end()) {
            return it->second;
        }
        return "";
    }

    const vector<string> &Vin_Cgi::getMultiValue(const string &sName, vector<string> &vtValue) const {
        vtValue.clear();
        auto beg = _mmpParams.equal_range(sName).first;
        auto end = _mmpParams.equal_range(sName).second;
        while (beg != _mmpParams.end() && beg != end) {
            vtValue.push_back(beg->second);
            beg++;
        }
        return vtValue;
    }

    string Vin_Cgi::getCookie(const string &sName) const {
        auto it = _mpCookies.find(sName);
        if (it != _mpCookies.end()) {
            return it->second;
        }
        return "";
    }

    string Vin_Cgi::setCookie(const string &sName, const string &sValue, const string &sExpires, const string &sPath,
                              const string &sDomain, bool bSecure) {
        if (sName.empty())
            return "";

        ostringstream os;

        os << "Set-Cookie: %s=%s;", sName.c_str(), sValue.c_str();
        if (sExpires.length() > 0) {
            os << "EXPIRES=%s;", sExpires.c_str();
        }

        if (sPath.length() > 0) {
            os << "PATH=%s;", sPath.c_str();
        }

        if (sDomain.length() > 0) {
            os << "DOMAIN=%s;", sDomain.c_str();
        }

        if (bSecure) {
            os << "SECURE";
        }
        os << endl;

        _mpCookies[sName] = sValue;

        return os.str();
    }

    bool Vin_Cgi::isParamEmpty() const {
        return _mmpParams.empty();;
    }

    bool Vin_Cgi::isParamExist(const string &sName) const {
        return _mmpParams.find(sName) != _mmpParams.end();
    }

    bool Vin_Cgi::isUploadOverSize() const {
        return _bUploadFileOverSize;
    }

    bool Vin_Cgi::isUploadOverSize(vector<Vin_Cgi_Upload> &vtUploads) const {
        vtUploads.clear();

        auto itEnd = _mpUpload.end();

        for (auto it = _mpUpload.begin(); it != itEnd; ++it) {
            if (it->second._bOverSize) {
                vtUploads.push_back(it->second);
            }
        }

        return !vtUploads.empty();
    }

    bool Vin_Cgi::isOverUploadFiles() const {
        return _bOverUploadFiles;
    }

    size_t Vin_Cgi::getUploadFilesCount() const {
        return _mpUpload.size();
    }

    const map<string, Vin_Cgi_Upload> &Vin_Cgi::getUploadFilesMap() const {
        return _mpUpload;
    }

    string Vin_Cgi::getCgiEnv(int iEnv) {
        switch (iEnv) {
            case ENM_SERVER_SOFTWARE:
                return getCgiEnv("SERVER_SOFTWARE");
            case ENM_SERVER_NAME:
                return getCgiEnv("SERVER_NAME");
            case ENM_GATEWAY_INTERFACE:
                return getCgiEnv("GATEWAY_INTERFACE");
            case ENM_SERVER_PROTOCOL:
                return getCgiEnv("SERVER_PROTOCOL");
            case ENM_SERVER_PORT:
                return getCgiEnv("SERVER_PORT");
            case ENM_REQUEST_METHOD:
                return getCgiEnv("REQUEST_METHOD");
            case ENM_PATH_INFO:
                return getCgiEnv("PATH_INFO");
            case ENM_PATH_TRANSLATED:
                return getCgiEnv("PATH_TRANSLATED");
            case ENM_SCRIPT_NAME:
                return getCgiEnv("SCRIPT_NAME");
            case ENM_HTTP_COOKIE:
                return getCgiEnv("HTTP_COOKIE");;
            case ENM_QUERY_STRING:
                return getCgiEnv("QUERY_STRING");
            case ENM_REMOTE_HOST:
                return getCgiEnv("REMOTE_HOST");
            case ENM_REMOTE_ADDR:
                return getCgiEnv("REMOTE_ADDR");
            case ENM_AUTH_TYPE:
                return getCgiEnv("AUTH_TYPE");
            case ENM_REMOTE_USER:
                return getCgiEnv("REMOTE_USER");
            case ENM_REMOTE_IDENT:
                return getCgiEnv("REMOTE_IDENT");
            case ENM_CONTENT_TYPE:
                return getCgiEnv("CONTENT_TYPE");
            case ENM_CONTENT_LENGTH:
                return getCgiEnv("CONTENT_LENGTH");
            case ENM_HTTP_USER_AGENT:
                return getCgiEnv("HTTP_USER_AGENT");
            default:
                return "";
        }
    }

    string Vin_Cgi::getCgiEnv(const string &sEnv) {
        auto it = _env.find(sEnv);
        if (it != _env.end()) {
            return it->second;
        }
        return "";
    }

    void Vin_Cgi::setCgiEnv(const string &sName, const string &sValue) {
        if (sName.empty())
            return;
        _env[sName] = sValue;
    }

    string Vin_Cgi::retHtmlHeader_(const string &sHeader) {
        return "Content-type: " + sHeader + "\n\n";
    }

    string Vin_Cgi::decodeURL_(const string &sUrl) {
        string sDecodeUrl;
        string::size_type pos = 0;
        string::size_type len = sUrl.length();

        sDecodeUrl = "";

        while (pos < len) {
            if (sUrl[pos] == '+') {
                sDecodeUrl += ' ';
                ++pos;
            } else if (sUrl[pos] == '%') {
                sDecodeUrl += Vin_Tools::x2c(sUrl.substr(pos + 1));
                pos += 3;
            } else {
                sDecodeUrl += sUrl[pos];
                ++pos;
            }
        }

        return sDecodeUrl;
    }

    string Vin_Cgi::encodeURL_(const string &sUrl) {
        string sEncodeUrl;
        string::size_type pos = 0;
        string::size_type len = sUrl.length();

        sEncodeUrl = "";

        while (pos < len) {
            if (sUrl[pos] == ' ') {
                sEncodeUrl += '+';
                ++pos;
            } else if ((sUrl[pos] > 'A' && sUrl[pos] < 'Z') || (sUrl[pos] > 'a' && sUrl[pos] < 'z') ||
                       (sUrl[pos] > '0' && sUrl[pos] < '9')) {
                sEncodeUrl += sUrl[pos];
                ++pos;
            } else {
                sEncodeUrl += Vin_Tools::c2x(sUrl[pos]);
                ++pos;
            }
        }

        return sEncodeUrl;
    }

    string Vin_Cgi::encodeHTML_(const string &src, bool blankEncode) {
        if (src.empty())
            return "";

        string result = src;
        result = Vin_Tools::replace(result, "&", "&amp;");
        result = Vin_Tools::replace(result, "<", "&lt;");
        result = Vin_Tools::replace(result, ">", "&gt;");
        result = Vin_Tools::replace(result, "\"", "&quot;");

        if (blankEncode) {
            result = Vin_Tools::replace(result, "\t", "    ");
            result = Vin_Tools::replace(result, "  ", "&nbsp; ");
            result = Vin_Tools::replace(result, "\r\n", "<br>");
            result = Vin_Tools::replace(result, "\n", "<br>");
        }
        return result;
    }

    string Vin_Cgi::encodeXML_(const string &src) {
        if (src.empty())
            return "";

        string result = src;
        result = Vin_Tools::replace(result, "&", "&amp;");
        result = Vin_Tools::replace(result, "<", "&lt;");
        result = Vin_Tools::replace(result, ">", "&gt;");
        result = Vin_Tools::replace(result, "\"", "&quot;");
        result = Vin_Tools::replace(result, "'", "&apos;");

        return result;
    }

    void Vin_Cgi::readCgiInput(multimap<string, string> &mmpParams, map<string, string> &mpCooies) {
        string sBuffer = getCgiEnv(ENM_HTTP_COOKIE);

        parseCookies(mpCooies, sBuffer);

        if (getCgiEnv(ENM_REQUEST_METHOD).find("POST") != string::npos) {

            if (getCgiEnv(ENM_CONTENT_TYPE).find("multipart/form-data") != string::npos) {            //文件上传模式
                parseUpload(mmpParams);
            } else if (getCgiEnv(ENM_CONTENT_TYPE).find("application/x-www-form-urlencoded") != string::npos) {
                getPOST(sBuffer);
                parseNormal(mmpParams, sBuffer);
            }
            getGET(sBuffer);  ///@cyz Although is POST, maybe somedata in QUERY_STRING
        } else if (getCgiEnv(ENM_REQUEST_METHOD).find("GET") != string::npos) {
            getGET(sBuffer);
        } else {               //default is GET
            getGET(sBuffer);
        }

        if (sBuffer.length() == 0) {
            return;
        }

        parseNormal(mmpParams, sBuffer);

    }

    void Vin_Cgi::getGET(string &sBuffer) {
        sBuffer = getCgiEnv(ENM_QUERY_STRING);
    }

    void Vin_Cgi::getPOST(string &sBuffer) {
        string sTmp = getCgiEnv(Vin_Cgi::ENM_CONTENT_LENGTH);
        string::size_type iSize = strtol(sTmp.c_str(), nullptr, 10);

        if (iSize > _iMaxContentLength) {
            throw Vin_Cgi_Exception("[Vin_Cgi::getPOST] : CONTENT_LENGTH is too large!");
        }

        if (iSize <= 0) {
            return;
        }

        ///@cyz TODO need to be tested
        char *pcTmp = new char[iSize];
        memset(pcTmp, '\0', iSize);
        _is->read(pcTmp, iSize);

        sBuffer = pcTmp;
        if (sBuffer.length() < iSize) {
            throw Vin_Cgi_Exception(
                    "[Vin_Cgi::getPOST] : Read CONTENT error: content size " + to_string(sBuffer.length()) + "<" +
                    to_string(iSize));
        }
    }

    void Vin_Cgi::parseCookies(map<string, string> &mpCookies, const string &sBuffer) {
        string::size_type len = sBuffer.length(), pos = 0;

        if (len == 0)
            return;

        string sName, sValue;
        bool bName = true;

        while (pos < len) {
            if (sBuffer[pos] == '=') {
                if (pos == len - 1)                     //最后一个参数
                {
                    sName = decodeURL_(Vin_Tools::trim(sName, " "));
                    sValue = decodeURL_(sValue);

                    mpCookies[sName] = sValue;

                    sName = sValue = "";
                }

                bName = false;                          //取值
            } else if ((sBuffer[pos] == ';') || (pos == len - 1)) {
                if (!bName) {
                    if ((pos == len - 1) && (sBuffer[pos] != ';')) {
                        sValue += sBuffer[pos];
                    }
                    sName = decodeURL_(Vin_Tools::trim(sName, " "));
                    sValue = decodeURL_(sValue);

                    mpCookies[sName] = sValue;

                    sName = sValue = "";
                }
                bName = true;                           //取名称
            } else {
                if (bName)
                    sName += sBuffer[pos++];
                else
                    sValue += sBuffer[pos++];
            }
        }
    }

    void Vin_Cgi::parseNormal(multimap<string, string> &mmpParams, const string &sBuffer) {

        string sName, sValue, sTmp;

        bool bName = true;

        string::size_type len = sBuffer.length(), pos = 0;

        while (pos < len) {
            sTmp = "";

            if (bName) {
                while ((sBuffer[pos] != '=') && (pos < len)) {
                    sTmp += sBuffer[pos];

                    ++pos;
                }
            } else {
                while ((sBuffer[pos] != '&') && (pos < len)) {
                    sTmp += sBuffer[pos];

                    ++pos;
                }
            }

            if (bName)                         //param name
            {
                sName = Vin_Cgi::decodeURL_(sTmp);

                if ((sBuffer[pos] != '=') || (pos == len - 1)) {
                    sValue = "";

                    mmpParams.insert(multimap<string, string>::value_type(sName, sValue));
                } else {
                    bName = false;
                }
            } else {
                sValue = Vin_Cgi::decodeURL_(sTmp);

                mmpParams.insert(multimap<string, string>::value_type(sName, sValue));

                bName = true;
            }

            ++pos;
        }
    }

    void Vin_Cgi::parseUpload(multimap<string, string> &mmpParams) {
        string sTmp = getCgiEnv(Vin_Cgi::ENM_CONTENT_LENGTH);
        string::size_type iSize = strtol(sTmp.c_str(), nullptr, 10);

        if (iSize > _iMaxContentLength) {
            throw Vin_Cgi_Exception("[Vin_Cgi::parseUpload] : CONTENT_LENGTH is too large!");
        }

        //get content type
        string sContentType = getCgiEnv(ENM_CONTENT_TYPE);

        string::size_type bPos = sContentType.find("boundary=");
        if (bPos == string::npos)
            return;

        //get boundary, boundary separete params
        string sBoundary(sContentType.substr(bPos + 9));
        sBoundary = "--" + sBoundary;

        string sBuffer;

        if (!getline(*_is, sBuffer)) {
            throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
        }

        sBuffer = Vin_Tools::trim(sBuffer);

        if (sBuffer == sBoundary) {
            parseFormData(mmpParams, sBoundary);
        } else {
            throw Vin_Cgi_Exception("[Vin_Cgi::parseUpload] 'multipart/form-data' Format is error");
        }
    }

    void Vin_Cgi::ignoreLine() {
        string sBuffer;

        //读取直到有一个空行
        while (true) {
            if (!getline(*_is, sBuffer)) {
                throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
            }
            sBuffer = Vin_Tools::trim(sBuffer, "\r\n");

            if (sBuffer.empty()) {
                break;
            }
        }
    }

    bool Vin_Cgi::writeFile(FILE *fp, const string &sFileName, const string &sBuffer, size_t &iTotalWrite) {
        if (fp == nullptr) {
            return false;
        }

        if (_mpUpload[sFileName]._bOverSize
            || iTotalWrite > _iUploadMaxSize
            || (iTotalWrite + sBuffer.length()) > _iUploadMaxSize) {
            _mpUpload[sFileName]._bOverSize = true;
            _bUploadFileOverSize = true;
            return false;
        }

        //内容写入文件
        int ret = fwrite(sBuffer.c_str(), 1, sBuffer.length(), fp);
        if (ret != sBuffer.length()) {
            fclose(fp);
            throw Vin_Cgi_Exception(
                    "[Vin_Cgi::parseFormData] upload file '" + _mpUpload[sFileName]._sServerFileName + "' error:" +
                    string(strerror(errno)));
        }
        iTotalWrite += sBuffer.length();
        _mpUpload[sFileName]._iSize = iTotalWrite;

        return true;
    }

    void Vin_Cgi::parseFormData(multimap<string, string> &mmpParams, const string &sBoundary) {
        string sBuffer;
        if (!getline(*_is, sBuffer)) {
            throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
        }

        string sName;
        string sValue;

        string::size_type pos1 = sBuffer.find('"', 0);
        string::size_type pos2 = sBuffer.find('"', pos1 + 1);
        if (pos2 == string::npos) {
            throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
        }

        //控件名称
        sName = sBuffer.substr(pos1 + 1, pos2 - pos1 - 1);

        pos1 = sBuffer.find("filename=\"");
        if (pos1 != string::npos) {
            FILE *fp = nullptr;
            string sTheName;

            //上传文件的form data
            pos1 += string("filename=\"").length();
            pos2 = sBuffer.find('"', pos1);

            if (pos2 == string::npos) {
                throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
            }

            //获取文件名
            sValue = sBuffer.substr(pos1, pos2 - pos1);

            if (sValue.length() > 0) {
                if (_iMaxUploadFiles < 0 || _mpUpload.size() < _iMaxUploadFiles) {
                    mmpParams.insert(multimap<string, string>::value_type(sName, sValue));

                    string sUploadFileName = _sUploadFilePrefix + "_" + to_string(_mpUpload.size());

                    //记录上传文件路径
                    _mpUpload[sName]._sFileName = sName;
                    _mpUpload[sName]._sRealFileName = sValue;
                    _mpUpload[sName]._sServerFileName = sUploadFileName;

                    sTheName = sName;

                    //打开文件
                    if ((fp = fopen(sUploadFileName.c_str(), "w")) == nullptr) {
                        mmpParams.clear();          //clear , exception safe
                        throw Vin_Cgi_Exception(
                                "[Vin_Cgi::parseFormData] Upload File '" + sValue + "' to '" + sUploadFileName +
                                "' error! " + string(strerror(errno)));
                    }
                } else {
                    _bOverUploadFiles = true;
                }
            }

            ignoreLine();

            string sLastBuffer;
            size_t iTotalWrite = 0;
            ///@cyz getline will keep \r\n in the buffer
            while (getline(*_is, sBuffer)) {                          //读取内容到文件
                string sTmp = Vin_Tools::trimright(sBuffer);

                if (sTmp.find(sBoundary) != string::npos) {
                    /*** @cyz TODO TC DO this, why?
                     * if(sLastBuffer.length() < 2)
                        {
                            if(fp)
                            {
                                fclose(fp);
                            }
                            throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
                        }

                        sLastBuffer = sLastBuffer.substr(0, sLastBuffer.length() - 2);

                        writeFile(fp, sTheFile, sLastBuffer, iTotalWrite);
                        if(fp)
                        {
                            fclose(fp);
                            fp = NULL;
                        }
                     *
                     **/
                    if (!sLastBuffer.empty()) {
                        writeFile(fp, sTheName, sLastBuffer, iTotalWrite);
                        fclose(fp);
                        fp = nullptr;
                    }

                    //新的一个form data
                    if (sTmp == sBoundary) {
                        parseFormData(mmpParams, sBoundary);
                        return;
                    } else if (sTmp == sBoundary + "--") {
                        return;
                    }
                    {
                        throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
                    }
                }

                sLastBuffer = sBuffer + "\n";
            }

            if (!sLastBuffer.empty()) {
                writeFile(fp, sTheName, sLastBuffer, iTotalWrite);
                fclose(fp);
                fp = nullptr;
            }
        }else{
            ignoreLine();

            string sLastBuffer;

            while(getline(*_is, sBuffer))
            {
                string sTmp = Vin_Tools::trimright(sBuffer);
                if(sTmp.find(sBoundary) != string::npos)
                {
                    /*** @cyz TODO TC DO this, why?
                    if(sLastBuffer.length() < 2)
                    {
                        throw TC_Cgi_Exception("[TC_Cgi::parseFormData] 'multipart/form-data' Format is error");
                    }
                    sLastBuffer = sLastBuffer.substr(0, sLastBuffer.length() - 2);
                    **/

                    mmpParams.insert(multimap<string, string>::value_type(sName, sLastBuffer));

                    //新的一个form data
                    if(sTmp == sBoundary)
                    {
                        parseFormData(mmpParams, sBoundary);
                        return;
                    }
                    else if(sTmp == sBoundary + "--")
                    {
                        return;
                    }
                    throw Vin_Cgi_Exception("[Vin_Cgi::parseFormData] 'multipart/form-data' Format is error");
                }
                sLastBuffer += sBuffer + "\n";
            }
        }

    }

}