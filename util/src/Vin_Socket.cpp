//
// Created by cyz on 5/16/18.
//

#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "util/include/Vin_Socket.h"

namespace vince {

    Vin_Socket::Vin_Socket() : _sock(INVALID_SOCKET), _bOwner(true), _iDomain(AF_INET) {

    }

    Vin_Socket::~Vin_Socket() {
        if (_bOwner)
            close();
    }

    void Vin_Socket::init(int fd, bool bOwner, int iDomain) {
        if (_bOwner)
            close();
        _sock = fd;
        _bOwner = bOwner;
        _iDomain = iDomain;
    }

    void Vin_Socket::setOwner(bool bOwner) {
        _bOwner = bOwner;
    }

    void Vin_Socket::setDomain(int iDomain) {
        _iDomain = iDomain;
    }

    void Vin_Socket::createSocket(int iSocketType, int iDomain) {
        assert(iSocketType == SOCK_STREAM || iSocketType == SOCK_DGRAM);
        if (_bOwner)
            close();

        _iDomain = iDomain;
        _sock = socket(iDomain, iSocketType, 0);
        _bOwner = true;

        if (_sock < 0) {
            _sock = INVALID_SOCKET;
            throw Vin_Socket_Exception(
                    "[Vin_Socket::createSocket] create socket error! :" + std::string(strerror(errno)));
        }

    }

    int Vin_Socket::getfd() const {
        return _sock;
    }

    bool Vin_Socket::isValid() const {
        return _sock != INVALID_SOCKET;
    }

    void Vin_Socket::close() {
        if (_sock != INVALID_SOCKET) {
            ::close(_sock);
            _sock = INVALID_SOCKET;
        }
    }

    void Vin_Socket::getPeerName(std::string &sPeerAddress, uint16_t &iPeerPort) {
        assert(_iDomain == AF_INET);
        sockaddr_in stPeer;
        socklen_t iPeerLen = sizeof(sockaddr_in);
        bzero(&stPeer, iPeerLen);

        _getPeerName((sockaddr *) &stPeer, iPeerLen);

        char sAddr[INET_ADDRSTRLEN] = "\0";

        inet_ntop(_iDomain, &stPeer.sin_addr, sAddr, sizeof(sAddr));

        sPeerAddress = sAddr;
        iPeerPort = ntohs(stPeer.sin_port);
    }

    void Vin_Socket::getPeerName(std::string &sPathName) {
        assert(_iDomain == AF_LOCAL);
        sockaddr_un stPeer;
        socklen_t iPeerLen = sizeof(sockaddr_un);
        bzero(&stPeer, iPeerLen);

        _getPeerName((sockaddr *) &stPeer, iPeerLen);

        sPathName = stPeer.sun_path;
    }

    void Vin_Socket::_getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen) {
        if (getpeername(_sock, pstPeerAddr, &iPeerLen) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::_getPeerName] getpeername error", errno);
        }
    }

    void Vin_Socket::getSockName(std::string &sSockAddress, uint16_t &iSockPort) {
        assert(_iDomain == AF_INET);
        sockaddr_in stSock;
        socklen_t iPeerLen = sizeof(sockaddr_in);
        bzero(&stSock, iPeerLen);

        _getSockName((sockaddr *) &stSock, iPeerLen);

        char sAddr[INET_ADDRSTRLEN] = "\0";

        inet_ntop(_iDomain, &stSock.sin_addr, sAddr, sizeof(sAddr));

        sSockAddress = sAddr;
        iSockPort = ntohs(stSock.sin_port);
    }

    void Vin_Socket::getSockName(std::string &sPathName) {
        assert(_iDomain == AF_LOCAL);
        sockaddr_un stSock;
        socklen_t iPeerLen = sizeof(sockaddr_un);
        bzero(&stSock, iPeerLen);

        _getSockName((sockaddr *) &stSock, iPeerLen);

        sPathName = stSock.sun_path;
    }

    void Vin_Socket::_getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen) {
        if (getsockname(_sock, pstSockAddr, &iSockLen) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::_getSockName] getsockname error", errno);
        }
    }

    int Vin_Socket::setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level) {
        return setsockopt(_sock, level, opt, pvOptVal, optLen);
    }

    int Vin_Socket::getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level) {
        return getsockopt(_sock, level, opt, pvOptVal, &optLen);
    }

    void Vin_Socket::parseAddr(const std::string &sAddr, in_addr &stAddr) {
        int iRet = inet_pton(AF_INET, sAddr.c_str(), &stAddr);
        if (iRet < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::parseAddr] inet_pton error", errno);
        } else if (iRet == 0) {///@cyz 返回值为0表示:对指定的协议来说,网络地址不是有效的
            hostent stHostent;
            hostent *pstHostent;
            char buf[2048] = "\0";
            int iError;

            gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);

            if (pstHostent == NULL) {
                throw Vin_Socket_Exception(
                        "[Vin_Socket::parseAddr] gethostbyname_r error! :" + std::string(hstrerror(iError)));
            } else {
                stAddr = *(in_addr *) pstHostent->h_addr;
            }
        }
    }

    void Vin_Socket::bind(const std::string &sServerAddr, int port) {
        assert(_iDomain == AF_INET);
        sockaddr_in bindAddr;
        socklen_t iBindLen = sizeof(sockaddr_in);
        bzero(&bindAddr, iBindLen);
        bindAddr.sin_family = _iDomain;
        bindAddr.sin_port = htons(port);

        if (sServerAddr == "") {
            bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            parseAddr(sServerAddr, bindAddr.sin_addr);
        }

        bind((sockaddr *) &bindAddr, sizeof(bindAddr));
    }

    void Vin_Socket::bind(const char *sPathName) {
        assert(_iDomain == AF_LOCAL);

        unlink(sPathName);

        sockaddr_un bindAddr;
        socklen_t iBindLen = sizeof(sockaddr_un);
        bzero(&bindAddr, iBindLen);
        bindAddr.sun_family = _iDomain;
        strncpy(bindAddr.sun_path, sPathName, sizeof(bindAddr.sun_path));

        bind((sockaddr *) &bindAddr, sizeof(bindAddr));
    }

    void Vin_Socket::bind(sockaddr *pstBindAddr, socklen_t iAddrLen) {
        //如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间
        int iReuseAddr = 1;

        //设置
        setSockOpt(SO_REUSEADDR, (const void *) &iReuseAddr, sizeof(int), SOL_SOCKET);

        if (::bind(_sock, pstBindAddr, iAddrLen) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::bind] bind error", errno);
        }
    }

    Vin_Socket &Vin_Socket::accept(sockaddr *pstSockAddr, socklen_t &iSockLen) {
        Vin_Socket *vinSock = new Vin_Socket;

        int ifd;

        while ((ifd = ::accept(_sock, pstSockAddr, &iSockLen) < 0) && errno == EINTR);

        vinSock->_sock = ifd;
        vinSock->_iDomain = _iDomain;

        return *vinSock;
    }

    void Vin_Socket::connect(const std::string &sServerAddr, uint16_t port) {
        assert(_iDomain == AF_INET);

        if (sServerAddr == "") {
            throw Vin_Socket_Exception("[Vin_Socket::connect] server address is empty error!");
        }

        sockaddr_in serverAddr;
        bzero(&serverAddr, sizeof(serverAddr));

        parseAddr(sServerAddr, serverAddr.sin_addr);
        serverAddr.sin_family = _iDomain;
        serverAddr.sin_port = port;

        if (_connect((sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::connect] connect error", errno);
        }

    }

    void Vin_Socket::connect(const char *sPathName) {
        assert(_iDomain == AF_LOCAL);

        if (strlen(sPathName) == 0) {
            throw Vin_Socket_Exception("[Vin_Socket::connect] server address is empty error!");
        }

        sockaddr_un serverAddr;
        bzero(&serverAddr, sizeof(serverAddr));

        strncpy(serverAddr.sun_path, sPathName, sizeof(serverAddr.sun_path));
        serverAddr.sun_family = _iDomain;

        if (_connect((sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::connect] connect error", errno);
        }
    }

    void Vin_Socket::connect(sockaddr *pstServerAddr) {
        if (_connect(pstServerAddr, sizeof(pstServerAddr)) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::connect] connect error", errno);
        }
    }

    int Vin_Socket::_connect(sockaddr *pstServerAddr, socklen_t serverLen) {
        return ::connect(_sock, pstServerAddr, serverLen);;
    }

    void Vin_Socket::listen(int connBackLog) {
        if (::listen(_sock, connBackLog) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::listen] listen error", errno);
        }
    }

    int Vin_Socket::recv(void *pvBuf, size_t iLen, int iFlag) {
        return ::recv(_sock, pvBuf, iLen, iFlag);
    }

    int Vin_Socket::send(const void *pvBuf, size_t iLen, int iFlag) {
        return ::send(_sock, pvBuf, iLen, iFlag);
    }

    int Vin_Socket::recvfrom(void *pvBuf, size_t iLen, std::string &sFromAddr, uint16_t &iFromPort, int iFlags) {
        assert(_iDomain == AF_INET);

        sockaddr_in fromAddr;
        bzero(&fromAddr, sizeof(fromAddr));

        socklen_t iFromlen = sizeof(fromAddr);

        int iBytes = recvfrom(pvBuf, iLen, (sockaddr *) &fromAddr, iFromlen, iFlags);

        if (iBytes >= 0) {
            char sAddr[INET_ADDRSTRLEN] = "\0";

            inet_ntop(_iDomain, &fromAddr.sin_addr, sAddr, sizeof(sAddr));

            sFromAddr = sAddr;
            iFromPort = ntohs(fromAddr.sin_port);
        }

        return iBytes;
    }

    int Vin_Socket::recvfrom(void *pvBuf, size_t iLen, sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags) {
        return ::recvfrom(_sock, pvBuf, iLen, iFlags, pstFromAddr, &iFromLen);
    }

    int Vin_Socket::sendto(const void *pvBuf, size_t iLen, const std::string &sToAddr, uint16_t iToPort, int iFlags) {
        assert(_iDomain == AF_INET);

        sockaddr_in toAddr;
        bzero(&toAddr, sizeof(toAddr));
        toAddr.sin_family = _iDomain;
        toAddr.sin_port = iToPort;

        if (sToAddr == "") {
            toAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        } else {
            parseAddr(sToAddr, toAddr.sin_addr);
        }

        return sendto(pvBuf, iLen, (sockaddr *) &toAddr, sizeof(toAddr), iFlags);
    }

    int Vin_Socket::sendto(const void *pvBuf, size_t iLen, sockaddr *pstToAddr, socklen_t iToLen, int iFlags) {
        return ::sendto(_sock, pvBuf, iLen, iFlags, pstToAddr, iToLen);
    }

    void Vin_Socket::shutdown(int iHow) {
        if (::shutdown(_sock, iHow) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::shutdown] shutdown error", errno);
        }
    }

    void Vin_Socket::setblock(bool bBlock) {
        assert(_sock != INVALID_SOCKET);

        setblock(_sock, bBlock);
    }

    void Vin_Socket::setblock(int fd, bool bBlock) {
        int flags;

        if (flags = fcntl(fd, F_GETFL, 0) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::setblock] F_GETFL error");
        }
        if (bBlock) {
            flags &= ~O_NONBLOCK;
        } else {
            flags |= O_NONBLOCK;
        }

        if (flags = fcntl(fd, F_SETFL, flags) < 0) {
            throw Vin_Socket_Exception("[Vin_Socket::setblock] F_SETFL error");
        }
    }

    void Vin_Socket::setNoCloseWait() {
        setCloseWait(0);
    }

    void Vin_Socket::setCloseWait(int delay) {
        linger l;
        l.l_onoff = 1;//在close socket调用后, 但是还有数据没发送完毕的时候容许逗留
        l.l_linger = delay;//容许逗留的时间

        if (setSockOpt(SO_LINGER, &l, sizeof(l), SOL_SOCKET) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setCloseWait] error", errno);
        }
    }

    void Vin_Socket::setCloseWaitDefault() {
        linger l;
        l.l_onoff = 0;
        l.l_linger = 0;

        if (setSockOpt(SO_LINGER, &l, sizeof(l), SOL_SOCKET) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setCloseWaitDefault] error", errno);
        }

    }

    void Vin_Socket::setTcpNoDelay() {
        int flag = 1;

        if (setSockOpt(TCP_NODELAY, (char *) &flag, sizeof(int), IPPROTO_TCP) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setTcpNoDelay] error", errno);
        }
    }

    void Vin_Socket::setKeepAlive() {
        int flag = 1;

        if (setSockOpt(SO_KEEPALIVE, (char *) &flag, sizeof(int), SOL_SOCKET) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setKeepAlive] error", errno);
        }
    }

    int Vin_Socket::getRecvBufferSize() {
        int sz;
        socklen_t len = sizeof(sz);
        if (getSockOpt(SO_RCVBUF, (void *) &sz, len, SOL_SOCKET) == -1 || len != sizeof(sz)) {
            throw Vin_Socket_Exception("[Vin_Socket::getRecvBufferSize] error", errno);
        }

        return sz;
    }

    void Vin_Socket::setRecvBufferSize(int sz) {

        socklen_t len = sizeof(sz);
        if (setSockOpt(SO_RCVBUF, (void *) &sz, len, SOL_SOCKET) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setRecvBufferSize] error", errno);
        }

    }

    int Vin_Socket::getSendBufferSize() {
        int sz;
        socklen_t len = sizeof(sz);
        if (getSockOpt(SO_SNDBUF, (void *) &sz, len, SOL_SOCKET) == -1 || len != sizeof(sz)) {
            throw Vin_Socket_Exception("[Vin_Socket::getSendBufferSize] error", errno);
        }

        return sz;
    }

    void Vin_Socket::setSendBufferSize(int sz) {

        socklen_t len = sizeof(sz);
        if (setSockOpt(SO_SNDBUF, (void *) &sz, len, SOL_SOCKET) == -1) {
            throw Vin_Socket_Exception("[Vin_Socket::setSendBufferSize] error", errno);
        }

    }

    std::vector<std::string> Vin_Socket::getLocalHosts() {
        Vin_Socket vs;
        vs.createSocket(SOCK_STREAM,AF_INET);

        ifconf ifcf;

        int iNumOfIps=10;
        int bufsize=iNumOfIps*sizeof(ifreq);
        ifcf.ifc_len =bufsize;
        ifcf.ifc_buf = new char[bufsize];
        ioctl(vs._sock, SIOCGIFCONF, &ifcf); //获取所有接口信息
        int iOldBufSize=ifcf.ifc_len;

        while(true){
            iNumOfIps+=10;
            bufsize=iNumOfIps*sizeof(ifreq);
            ifcf.ifc_len =bufsize;
            ifcf.ifc_buf = new char[bufsize];
            if(ioctl(vs._sock, SIOCGIFCONF, &ifcf)<0){
                throw Vin_Socket_Exception("[Vin_Socket::getLocalHosts] error", errno);
            }
            if(iOldBufSize==ifcf.ifc_len){
                break;
            }else{
                iOldBufSize=bufsize;
            }
        }

        std::vector<std::string> rlt;

        ifreq *ifq=(ifreq*)ifcf.ifc_buf;

        iNumOfIps=iOldBufSize/sizeof(ifreq);
        for (int i=0; i<iNumOfIps;i++)
        {
            if(ifq[i].ifr_ifru.ifru_addr.sa_family == AF_INET){ //for ipv4
                sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&ifq[i].ifr_ifru.ifru_addr);
                if(addr->sin_addr.s_addr != 0)
                {
                    char sAddr[INET_ADDRSTRLEN] = "\0";
                    inet_ntop(AF_INET, &(*addr).sin_addr, sAddr, sizeof(sAddr));
                    rlt.push_back(std::string(sAddr));
                }
            }
        }

        delete[] ifcf.ifc_buf;

        return rlt;
    }

    void Vin_Socket::createPipe(int *fds, bool bBlock) {
        if(::pipe(fds)!=0){
            throw Vin_Socket_Exception("[Vin_Socket::createPipe] error", errno);
        }

        try {
            setblock(fds[0],bBlock);
            setblock(fds[1],bBlock);
        }catch (...){
            ::close(fds[0]);
            ::close(fds[1]);
            throw;
        }

    }


}