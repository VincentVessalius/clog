//
// Created by cyz on 5/16/18.
//

#ifndef CLOG_VIN_ENDPOINT_H
#define CLOG_VIN_ENDPOINT_H

#include <sstream>
#include "Vin_Exception.h"

namespace vince{
    struct Vin_Endpoint_Exception : public Vin_Exception
    {
        explicit Vin_Endpoint_Exception(const std::string &buffer) : Vin_Exception(buffer){};
        ~Vin_Endpoint_Exception() noexcept {};
    };

    /**
     * 表示一个网络端口,支持以下格式:
     *
     * 1:tcp -h 127.0.0.1 -p 2345 -t 10000
     *
     * 2:tcp -h /tmp/sock.sock -p 0 -t 10000
     *
     * 3:udp -h 127.0.0.1 -p 2345 -t 10000
     *
     * -p 0:表示本地套接字
     *
     * -q 0:表示qos的dscp值
     *
     * 此时-h表示的文件路径
     */
    class Vin_Endpoint {
    public:
        enum EType {UDP=0,TCP=1,SSL=2};

        Vin_Endpoint();

        Vin_Endpoint(const std::string& host, int port, int timeout, int type = TCP,
                int grid = 0, int qos = 0, int weight = -1, unsigned int weighttype = 0, int authType = 0);

        explicit Vin_Endpoint(std::string& desc);

        Vin_Endpoint(const Vin_Endpoint& l);

        Vin_Endpoint& operator = (const Vin_Endpoint& l);

        bool operator == (const Vin_Endpoint& l);

    public:
        /**
         * @brief 设置ip
         * @param str
         */
        void setHost(const std::string& host);

        /**
         * @brief 获取ip
         *
         * @return const string&
         */
        std::string getHost() const;

        /**
         * @brief 设置端口
         * @param port
         */
        void setPort(int port);

        /**
         * @brief 获取端口
         *
         * @return int
         */
        int getPort() const;

        /**
         * @brief 设置超时时间
         * @param timeout
         */
        void setTimeout(int timeout);

        /**
         * @brief 获取超时时间
         *
         * @return int
         */
        int getTimeout() const;

        /**
         * @brief  是否是TCP, 否则则为UDP
         *
         * @return bool
         */
        int  isTcp() const;
        /**
         * @brief  是否是SSL
         *
         * @return int
         */
        int isSSL() const;

        /**
         * @brief 设置为TCP或UDP
         * @param bTcp
         */
        void setTcp(bool bTcp);

        /**
         * @brief 设置为TCP/UDP/SSL
         * @param type
         */
        void setType(int type);
        /**
         * @brief 获取协议类型
         */
        int getType() const;
        /**
         * @brief 获取路由状态
         * @param grid
         */
        int getGrid() const;

        /**
         * @brief 设置路由状态
         * @param grid
         */
        void setGrid(int grid);

        /**
         * @brief 获取路由状态
         * @param grid
         */
        int getQos() const;

        /**
         * @brief 设置路由状态
         * @param grid
         */
        void setQos(int qos);

        /**
         * @brief 获取节点的静态权重值
         */
        int getWeight() const;

        /**
         * @brief 设置节点的静态权重值
         * @param weight
         */
        void setWeight(int weight);

        /**
         * @brief 获取节点的权重使用方式
         */
        unsigned int getWeightType() const;

        /**
         * @brief 设置节点的权重使用方式
         * @param weightway
         */
        void setWeightType(unsigned int weighttype);

        /**
         * @brief 是否是本地套接字
         *
         * @return bool
         */
        bool isUnixLocal() const;

        /**
         * @brief 获取认证类型
         */
        int getAuthType() const;

        /**
         * @brief 设置认证类型
         */
        void setAuthType(int type);

        /**
         * @brief 字符串描述
         *
         * @return string
         */
        std::string toString();

        /**
         * @brief  字符串形式的端口
         * tcp:SOCK_STREAM
         *
         * udp:SOCK_DGRAM
         *
         * -h: ip
         *
         * -p: 端口
         *
         * -t: 超时时间, 毫秒
         *
         * -p 和 -t可以省略, -t默认10s
         *
         * tcp -h 127.0.0.1 -p 2345 -t 10000
         *
         * @param desc
         */
        void parse(const std::string &desc);


    private:
        void init(const std::string& host, int port, int timeout, int type, int grid, int qos, int weight, unsigned int weighttype, int authType);

    protected:
        /**
         * ip
         */
        std::string _host;

        /**
         * 端口
         */
        int         _port;

        /**
         * 超时时间
         */
        int         _timeout;

        /**
         * 类型
         */
        int         _type;

        /**
         * 路由状态
         */
        int         _grid;

        /**
         *  网络Qos的dscp值
         */
        int         _qos;

        /**
         *  节点的静态权重值
         */
        int            _weight;

        /**
         *  节点的权重使用方式
         */
        unsigned int    _weightType;
        /**
         *  鉴权类型
         */
        int         _authType;
    };
}



#endif //CLOG_VIN_ENDPOINT_H
