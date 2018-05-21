//
// Created by cyz on 5/16/18.
//

#ifndef CLOG_VIN_CLIENTSOCKET_H
#define CLOG_VIN_CLIENTSOCKET_H

#include "util/include/Vin_Endpoint.h"
#include "Vin_Socket.h"

namespace vince {
    class Vin_ClientSocket {
    public:
        Vin_ClientSocket();

        virtual ~Vin_ClientSocket() = default;

        /**
        * @brief 构造函数
        * @param sIP      服务器IP
        * @param iPort    端口, port为0时:表示本地套接字此时ip为文件路径
        * @param iTimeout 超时时间, 毫秒
        */
        Vin_ClientSocket(const std::string &sIp, int iPort, int iTimeout);

        /**
        * @brief 初始化函数
        * @param sIP      服务器IP
        * @param iPort    端口, port为0时:表示本地套接字此时ip为文件路径
        * @param iTimeout 超时时间, 毫秒
        */
        void init(const std::string &sIp, int iPort, int iTimeout);

        /**
        * @brief 发送到服务器
        * @param sSendBuffer 发送buffer
        * @param iSendLen    发送buffer的长度
        * @return            int 0 成功,<0 失败
        */
        virtual int send(const char *sSendBuffer, size_t iSendLen) = 0;

        /**
        * @brief 从服务器返回不超过iRecvLen的字节
        * @param sRecvBuffer 接收buffer
        * @param iRecvLen    指定接收多少个字符才返回,输出接收数据的长度
        * @return            int 0 成功,<0 失败
        */
        virtual int recv(char *sRecvBuffer, size_t &iRecvLen) = 0;

        /**
        * @brief  定义发送的错误
        */
        enum {
            EM_SUCCESS = 0,          /** EM_SUCCESS:发送成功*/
            EM_SEND = -1,        /** EM_SEND:发送错误*/
            EM_SELECT = -2,        /** EM_SELECT:select 错误*/
            EM_TIMEOUT = -3,        /** EM_TIMEOUT:select超时*/
            EM_RECV = -4,        /** EM_RECV: 接受错误*/
            EM_CLOSE = -5,        /**EM_CLOSE: 服务器主动关闭*/
            EM_CONNECT = -6,        /** EM_CONNECT : 服务器连接失败*/
            EM_SOCKET = -7        /**EM_SOCKET : SOCKET初始化失败*/
        };

    protected:
        /**
         * 套接字句柄
         */
        Vin_Socket _socket;

        /**
         * ip或文件路径
         */
        std::string _ip;

        /**
         * 端口或-1:标示是本地套接字
         */
        int _port;

        /**
         * 超时时间, 毫秒
         */
        int _timeout;
    };

    class Vin_TCPClient : public Vin_ClientSocket {

    public:
        Vin_TCPClient() = default;

        Vin_TCPClient(const std::string &sIp, int iPort, int iTimeout);

    public:

        /**
        * @brief  发送到服务器
        * @param sSendBuffer  发送buffer
        * @param iSendLen     发送buffer的长度
        * @return             int 0 成功,<0 失败
        */
        int send(const char *sSendBuffer, size_t iSendLen) override;

        /**
        * @brief  从服务器返回不超过iRecvLen的字节
        * @param sRecvBuffer 接收buffer
        * @param iRecvLen    指定接收多少个字符才返回,输出接收数据的长度
        * @return            int 0 成功,<0 失败
        */
        int recv(char *sRecvBuffer, size_t &iRecvLen) override;

        /**
        *  @brief 从服务器直到结束符(注意必须是服务器返回的结束符,
        *         而不是中间的符号 ) 只能是同步调用
        * @param sRecvBuffer 接收buffer, 包含分隔符
        * @param sSep        分隔符
        * @return            int 0 成功,<0 失败
        */
        int recvBySep(std::string &sRecvBuffer, const std::string &sSep);

        /**
         * @brief 接收倒服务器关闭连接为止
         * @param recvBuffer
         *
         * @return int 0 成功,<0 失败
         */
        int recvAll(std::string &sRecvBuffer);

        /**
         * @brief  从服务器返回iRecvLen的字节
         * @param sRecvBuffer, sRecvBuffer的buffer长度必须大于等于iRecvLen
         * @param iRecvLen
         * @return int 0 成功,<0 失败
         */
        int recvLength(char *sRecvBuffer, size_t iRecvLen);

        /**
        * @brief  发送到服务器, 从服务器返回不超过iRecvLen的字节
        * @param sSendBuffer  发送buffer
        * @param iSendLen     发送buffer的长度
        * @param sRecvBuffer  接收buffer
        * @param iRecvLen     接收buffer的长度指针[in/out],
        *                     输入时表示接收buffer的大小,返回时表示接收了多少个字符
        * @return             int 0 成功,<0 失败
        */
        int sendRecv(const char* sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

        /**
        * @brief  发送倒服务器, 并等待服务器直到结尾字符, 包含结尾字符
        * sSep必须是服务器返回的结束符,而不是中间的符号，只能是同步调用
        * (一次接收一定长度的buffer,如果末尾是sSep则返回,
        * 否则继续等待接收)
        *
        * @param sSendBuffer  发送buffer
        * @param iSendLen     发送buffer的长度
        * @param sRecvBuffer  接收buffer
        * @param sSep         结尾字符
        * @return             int 0 成功,<0 失败
        */
        int sendRecvBySep(const char* sSendBuffer, size_t iSendLen, std::string &sRecvBuffer, const std::string &sSep);

        /**
        * @brief  发送倒服务器, 并等待服务器直到结尾字符(\r\n), 包含\r\n
        * 注意必须是服务器返回的结束符,而不是中间的符号
        * 只能是同步调用
        * (一次接收一定长度的buffer,如果末尾是\r\n则返回,否则继续等待接收)
        *
        * @param sSendBuffer  发送buffer
        * @param iSendLen     发送buffer的长度
        * @param sRecvBuffer  接收buffer
        * @param sSep         结尾字符
        * @return             int 0 成功,<0 失败
        */
        int sendRecvLine(const char* sSendBuffer, size_t iSendLen, std::string &sRecvBuffer);

        /**
         * @brief  发送到服务器, 接收直到服务器关闭连接为止
         * 此时服务器关闭连接不作为错误
         * @param sSendBuffer
         * @param iSendLen
         * @param sRecvBuffer
         *
         * @return int
         */
        int sendRecvAll(const char* sSendBuffer, size_t iSendLen, std::string &sRecvBuffer);

    protected:

        static const int LEN_MAXRECV=8192;
        /**
         * @brief  获取socket
         *
         * @return int
         */
        int checkSocket();
    };

    class Vin_UDPClient : public Vin_ClientSocket {
    public:
        Vin_UDPClient()= default;

        Vin_UDPClient(const std::string &sIp, int iPort, int iTimeout);

    public:
        /**
     * @brief  发送数据
     * @param sSendBuffer 发送buffer
     * @param iSendLen    发送buffer的长度
     *
     * @return            int 0 成功,<0 失败
     */
        int send(const char *sSendBuffer, size_t iSendLen) override;

        /**
         * @brief  接收数据
         * @param sRecvBuffer  接收buffer
         * @param iRecvLen     输入/输出字段
         * @return             int 0 成功,<0 失败
         */
        int recv(char *sRecvBuffer, size_t &iRecvLen) override;

        /**
         * @brief  接收数据, 并返回远程的端口和ip
         * @param sRecvBuffer 接收buffer
         * @param iRecvLen    输入/输出字段
         * @param sRemoteIp   输出字段, 远程的ip
         * @param iRemotePort 输出字段, 远程端口
         *
         * @return int 0 成功,<0 失败
         */
        int recvfrom(char *sRecvBuffer, size_t &iRecvLen, std::string &sRemoteIp, uint16_t &iRemotePort);

        /**
         * @brief  发送并接收数据
         * @param sSendBuffer 发送buffer
         * @param iSendLen    发送buffer的长度
         * @param sRecvBuffer 输入/输出字段
         * @param iRecvLen    输入/输出字段
         *
         * @return int 0 成功,<0 失败
         */
        int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen);

        /**
         * @brief  发送并接收数据, 同时获取远程的ip和端口
         * @param sSendBuffer  发送buffer
         * @param iSendLen     发送buffer的长度
         * @param sRecvBuffer  输入/输出字段
         * @param iRecvLen     输入/输出字段
         * @param sRemoteIp    输出字段, 远程的ip
         * @param iRemotePort  输出字段, 远程端口
         *
         * @return int 0 成功,<0 失败
         */
        int sendRecv(const char *sSendBuffer, size_t iSendLen, char *sRecvBuffer, size_t &iRecvLen, std::string &sRemoteIp, uint16_t &iRemotePort);
    protected:
        /**
         * @brief  获取socket
         *
         * @return TC_Socket&
         */
        int checkSocket();

    };
}


#endif //CLOG_VIN_CLIENTSOCKET_H
