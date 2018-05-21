//
// Created by cyz on 5/16/18.
//

#include "util/include/Vin_ClientSocket.h"
#include "util/include/Vin_Epoller.h"

namespace vince {
///////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_ClientSocket
    Vin_ClientSocket::Vin_ClientSocket() : _port(0), _timeout(3000) {

    }

    Vin_ClientSocket::Vin_ClientSocket(const std::string &sIp, int iPort, int iTimeout) {
        init(sIp, iPort, iTimeout);
    }

    void Vin_ClientSocket::init(const std::string &sIp, int iPort, int iTimeout) {
        _socket.close();
        _ip = sIp;
        _port = iPort;
        _timeout = iTimeout;
    }

///////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_TCPClient
    Vin_TCPClient::Vin_TCPClient(const std::string &sIp, int iPort, int iTimeout) : Vin_ClientSocket(sIp, iPort,
                                                                                                     iTimeout) {

    }

    int Vin_TCPClient::checkSocket() {
        if (!_socket.isValid()) {
            try {
                if (_port == 0) {
                    _socket.createSocket(SOCK_STREAM, AF_LOCAL);
                } else {
                    _socket.createSocket(SOCK_STREAM, AF_INET);
                }

                _socket.setblock(false);

                try {
                    if (_port == 0) {
                        _socket.connect(_ip.c_str());
                    } else {
                        _socket.connect(_ip, _port);
                    }
                }
                catch (Vin_Socket_Exception &ex) {
                    if (errno != EINPROGRESS) {
                        _socket.close();
                        return EM_CONNECT;
                    }
                }

                Vin_Epoller ep;
                ep.create(1);
                ep.add(_socket.getfd(), 0, EPOLLOUT);
                int iRetCode = ep.wait(_timeout);
                if (iRetCode < 0) {
                    _socket.close();
                    return EM_SELECT;
                } else if (iRetCode == 0) {
                    _socket.close();
                    return EM_TIMEOUT;
                } else {
                    for (int i = 0; i < iRetCode; ++i) {
                        const epoll_event &ev = ep.get(i);
                        if (ev.events & EPOLLERR || ev.events & EPOLLHUP) {
                            _socket.close();
                            return EM_CONNECT;
                        } else {
                            int iVal = 0;
                            socklen_t iLen = sizeof(int);
                            if (::getsockopt(_socket.getfd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&iVal),
                                             &iLen) == -1 || iVal) {
                                _socket.close();
                                return EM_CONNECT;
                            }
                        }
                    }
                }

                //设置为阻塞模式
                _socket.setblock(true);
            } catch (Vin_Socket_Exception &ex) {
                _socket.close();
                return EM_SOCKET;
            }
        }
        return EM_SUCCESS;
    }

    int Vin_TCPClient::send(const char *sSendBuffer, size_t iSendLen) {
        int iRet = checkSocket();
        if (iRet != EM_SUCCESS) {
            return iRet;
        }

        iRet = _socket.send(sSendBuffer, iSendLen);
        if (iRet < 0) {
            _socket.close();
            return EM_SEND;
        }

        return EM_SUCCESS;
    }

    int Vin_TCPClient::recv(char *sRecvBuffer, size_t &iRecvLen) {
        int iRet = checkSocket();
        if (iRet != EM_SUCCESS) {
            return iRet;
        }

        Vin_Epoller epoller;
        epoller.create(1);
        epoller.add(_socket.getfd(), 0, EPOLLIN);

        int iRetCode = epoller.wait(_timeout);
        if (iRetCode < 0) {
            _socket.close();
            return EM_SELECT;
        } else if (iRetCode == 0) {
            _socket.close();
            return EM_TIMEOUT;
        }

        epoll_event &ev = epoller.get(0);

        if (ev.events & EPOLLIN) {
            int iLen = _socket.recv(sRecvBuffer, iRecvLen);
            if (iLen < 0) {
                _socket.close();
                return EM_RECV;
            } else if (iLen == 0) {
                _socket.close();
                return EM_CLOSE;
            }

            iRecvLen = iLen;
            return EM_SUCCESS;
        } else {
            _socket.close();
            return EM_SELECT;
        }

    }

    //TODO need be tested, because vince
    int Vin_TCPClient::recvBySep(std::string &sRecvBuffer, const std::string &sSep) {
        sRecvBuffer.clear();

        int iRet = checkSocket();
        if (iRet != EM_SUCCESS) {
            return iRet;
        }

        Vin_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_socket.getfd(), 0, EPOLLIN);

        bool toggle = false;

        while (true) {

            int iRetCode = epoller.wait(_timeout);
            if (iRetCode < 0) {
                _socket.close();
                if (toggle)
                    return EM_SUCCESS;
                return EM_SELECT;
            } else if (iRetCode == 0) {
                _socket.close();
                if (toggle)
                    return EM_SUCCESS;
                return EM_TIMEOUT;
            }

            epoll_event &ev = epoller.get(0);

            if (ev.events & EPOLLIN) {
                toggle = false;

                char *tmpRecv = new char[LEN_MAXRECV];
                int iLen = _socket.recv(tmpRecv, LEN_MAXRECV);

                if (iLen < 0) {
                    _socket.close();
                    return EM_RECV;
                } else if (iLen == 0) {
                    _socket.close();
                    return EM_CLOSE;
                }

                sRecvBuffer.append(tmpRecv, iLen);

                if (iLen < LEN_MAXRECV) {
                    if (sRecvBuffer.length() >= sSep.length() &&
                        sRecvBuffer.compare(sRecvBuffer.length() - sSep.length(), sSep.length(), sSep) == 0) {
                        break;
                    }
                } else if (iLen == LEN_MAXRECV) {
                    toggle = true;
                }

            } else {
                _socket.close();
                return EM_SELECT;
            }
        }

        return EM_SUCCESS;
    }

    int Vin_TCPClient::recvAll(std::string &sRecvBuffer) {
        sRecvBuffer.clear();

        int iRet = checkSocket();
        if (iRet != EM_SUCCESS) {
            return iRet;
        }

        Vin_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_socket.getfd(), 0, EPOLLIN);

        while (true) {

            int iRetCode = epoller.wait(_timeout);
            if (iRetCode < 0) {
                _socket.close();
                return EM_SELECT;
            } else if (iRetCode == 0) {
                _socket.close();
                return EM_TIMEOUT;
            }

            epoll_event &ev = epoller.get(0);

            if (ev.events & EPOLLIN) {

                char *tmpRecv = new char[LEN_MAXRECV];
                int iLen = _socket.recv(tmpRecv, LEN_MAXRECV);

                if (iLen < 0) {
                    _socket.close();
                    return EM_RECV;
                } else if (iLen == 0) {
                    _socket.close();
                    return EM_SUCCESS;
                }

                sRecvBuffer.append(tmpRecv, iLen);

            } else {
                _socket.close();
                return EM_SELECT;
            }
        }
    }

    int Vin_TCPClient::recvLength(char *sRecvBuffer, size_t iRecvLen) {
        int iRet = checkSocket();
        if (iRet != EM_SUCCESS) {
            return iRet;
        }

        Vin_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_socket.getfd(), 0, EPOLLIN);

        size_t iRecvLeft = iRecvLen;
        size_t pos = 0;

        while (iRecvLeft != 0) {
            int iRetCode = epoller.wait(_timeout);
            if (iRetCode < 0) {
                _socket.close();
                return EM_SELECT;
            } else if (iRetCode == 0) {
                _socket.close();
                return EM_TIMEOUT;
            }

            epoll_event &ev = epoller.get(0);

            if (ev.events & EPOLLIN) {

                int iLen = _socket.recv(sRecvBuffer + pos, iRecvLeft);

                if (iLen < 0) {
                    _socket.close();
                    return EM_RECV;
                } else if (iLen == 0) {
                    _socket.close();
                    return EM_CLOSE;
                }

                iRecvLeft -= iLen;
                pos += iLen;

            } else {
                _socket.close();
                return EM_SELECT;
            }
        }
        return EM_SUCCESS;
    }

    int Vin_TCPClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen) {
        int iRet = send(sSendBuffer, iSendLen);
        if (iRet != 0) {
            return iRet;
        }

        return recv(sRecvBuffer, iRecvLen);
    }

    int Vin_TCPClient::sendRecvBySep(const char *sSendBuffer, size_t iSendLen, std::string &sRecvBuffer,
                                     const std::string &sSep) {
        int iRet = send(sSendBuffer, iSendLen);
        if (iRet != 0) {
            return iRet;
        }

        return recvBySep(sRecvBuffer, sSep);
    }

    int Vin_TCPClient::sendRecvLine(const char *sSendBuffer, size_t iSendLen, std::string &sRecvBuffer) {
        return sendRecvBySep(sSendBuffer, iSendLen, sRecvBuffer, "\r\n");
    }

    int Vin_TCPClient::sendRecvAll(const char *sSendBuffer, size_t iSendLen, std::string &sRecvBuffer) {
        int iRet = send(sSendBuffer, iSendLen);
        if (iRet != 0) {
            return iRet;
        }

        return recvAll(sRecvBuffer);
    }

///////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_UDPClient
    Vin_UDPClient::Vin_UDPClient(const std::string &sIp, int iPort, int iTimeout) : Vin_ClientSocket(sIp, iPort,
                                                                                                     iTimeout) {

    }

    int Vin_UDPClient::checkSocket() {
        if(!_socket.isValid()){
            try {
                if(_port==0){
                    _socket.createSocket(SOCK_DGRAM,AF_LOCAL);
                }else{
                    _socket.createSocket(SOCK_DGRAM,AF_INET);
                }
            }catch (Vin_Socket_Exception& ex){
                _socket.close();
                return EM_SOCKET;
            }

            try {
                if(_port==0){
                    _socket.connect(_ip.c_str());
                    _socket.bind(_ip.c_str());///@cyz limit the recv addr to _ip
                }else{
                    _socket.connect(_ip.c_str(),_port);
                }
            }catch (Vin_Socket_Exception& ex){
                _socket.close();
                return EM_CONNECT;
            }

        }
        return EM_SUCCESS;
    }

    int Vin_UDPClient::send(const char *sSendBuffer, size_t iSendLen) {
        int iRet=checkSocket();
        if(iRet<0){
            return iRet;
        }

        iRet=_socket.send(sSendBuffer,iSendLen);
        if(iRet<0){
            return EM_SEND;
        }
        return EM_SUCCESS;
    }

    int Vin_UDPClient::recv(char *sRecvBuffer, size_t &iRecvLen) {
        int iRet=checkSocket();
        if(iRet<0){
            return iRet;
        }

        iRet=_socket.recv(sRecvBuffer,iRecvLen);
        if(iRet<0){
            return EM_RECV;
        }
        return EM_SUCCESS;
    }

    int Vin_UDPClient::recvfrom(char *sRecvBuffer, size_t &iRecvLen, std::string &sRemoteIp, uint16_t &iRemotePort) {
        int iRet=checkSocket();
        if(iRet<0){
            return iRet;
        }

        Vin_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_socket.getfd(),0,EPOLLIN);
        int iRetCode = epoller.wait(_timeout);
        if (iRetCode < 0)
        {
            return EM_SELECT;
        }
        else if (iRetCode == 0)
        {
            return EM_TIMEOUT;
        }

        epoll_event ev  = epoller.get(0);
        if(ev.events & EPOLLIN)
        {
            iRet = _socket.recvfrom(sRecvBuffer, iRecvLen, sRemoteIp, iRemotePort);
            if(iRet <0 )
            {
                return EM_SEND;
            }

            iRecvLen = iRet;
            return EM_SUCCESS;
        }

        return EM_SELECT;
    }

    int Vin_UDPClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen) {
        int iRet=send(sSendBuffer,iSendLen);
        if(iRet<0)
            return iRet;

        return recv(sRecvBuffer,iRecvLen);
    }

    int Vin_UDPClient::sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen,
                                std::string &sRemoteIp, uint16_t &iRemotePort) {
        int iRet=send(sSendBuffer,iSendLen);
        if(iRet<0)
            return iRet;

        return recvfrom(sRecvBuffer,iRecvLen,sRemoteIp,iRemotePort);
    }


}