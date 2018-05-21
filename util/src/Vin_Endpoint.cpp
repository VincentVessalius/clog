//
// Created by cyz on 5/16/18.
//

#include "util/include/Vin_Endpoint.h"


namespace vince {

////////////////////////////////////////////////////////////////
///@cyz Implementation of Vin_Endpoint

    Vin_Endpoint::Vin_Endpoint() {
        _host = "0.0.0.0";
        _port = 0;
        _timeout = 3000;
        _type = TCP;
        _grid = 0;
        _qos = 0;
        _weight = -1;
        _weightType = 0;
        _authType = 0;
    }

    Vin_Endpoint::Vin_Endpoint(const std::string &host, int port, int timeout, int type, int grid, int qos, int weight,
                               unsigned int weighttype, int authType) {
        _host = host;
        _port = port;
        _timeout = timeout;
        _type = type;
        _grid = grid;
        _qos = qos;
        _weight = weight;
        _weightType = weighttype;
        _authType = authType;
    }

    Vin_Endpoint::Vin_Endpoint(std::string &desc) {
        parse(desc);
    }

    Vin_Endpoint::Vin_Endpoint(const Vin_Endpoint &l) {
        _host = l._host;
        _port = l._port;
        _timeout = l._timeout;
        _type = l._type;
        _grid = l._grid;
        _qos = l._qos;
        _weight = l._weight;
        _weightType = l._weightType;
        _authType = l._authType;
    }

    Vin_Endpoint &Vin_Endpoint::operator=(const Vin_Endpoint &l) {
        if (this != &l) {
            _host = l._host;
            _port = l._port;
            _timeout = l._timeout;
            _type = l._type;
            _grid = l._grid;
            _qos = l._qos;
            _weight = l._weight;
            _weightType = l._weightType;
            _authType = l._authType;
        }

        return *this;
    }

    bool Vin_Endpoint::operator==(const Vin_Endpoint &l) {
        return (_host == l._host && _port == l._port && _timeout == l._timeout && _type == l._type &&
                _grid == l._grid && _qos == l._qos && _weight == l._weight && _weightType == l._weightType &&
                _authType == l._authType);
    }

    void Vin_Endpoint::setHost(const std::string &host) { _host = host; }

    std::string Vin_Endpoint::getHost() const { return _host; }

    void Vin_Endpoint::setPort(int port) { _port = port; }

    int Vin_Endpoint::getPort() const { return _port; }

    void Vin_Endpoint::setTimeout(int timeout) { _timeout = timeout; }

    int Vin_Endpoint::getTimeout() const { return _timeout; }

    int Vin_Endpoint::isTcp() const { return _type == TCP || _type == SSL; }

    int Vin_Endpoint::isSSL() const { return _type == SSL; }

    void Vin_Endpoint::setTcp(bool bTcp) { _type = bTcp; }

    void Vin_Endpoint::setType(int type) { _type = type; }

    int Vin_Endpoint::getType() const { return _type; }

    int Vin_Endpoint::getGrid() const { return _grid; }

    void Vin_Endpoint::setGrid(int grid) { _grid = grid; }

    int Vin_Endpoint::getQos() const { return _qos; }

    void Vin_Endpoint::setQos(int qos) { _qos = qos; }

    int Vin_Endpoint::getWeight() const { return _weight; }

    void Vin_Endpoint::setWeight(int weight) { _weight = weight; }

    unsigned int Vin_Endpoint::getWeightType() const { return _weightType; }

    void Vin_Endpoint::setWeightType(unsigned int weighttype) { _weightType = weighttype; }

    bool Vin_Endpoint::isUnixLocal() const { return _port == 0; }

    int Vin_Endpoint::getAuthType() const { return _authType; }

    void Vin_Endpoint::setAuthType(int type) { _authType = type; }

    std::string Vin_Endpoint::toString() {
        std::ostringstream os;
        if (_type == TCP)
            os << "tcp";
        else if (_type == UDP)
            os << "udp";
        else
            os << "ssl";

        os << " -h " << _host << " -p " << _port << " -t " << _timeout;
        if (_grid != 0) os << " -g " << _grid;
        if (_qos != 0) os << " -q " << _qos;
        if (_weight != -1) os << " -w " << _weight;
        if (_weightType != 0) os << " -v " << _weightType;
        if (_authType != 0) os << " -e " << _authType;
        return os.str();
    }

    void Vin_Endpoint::parse(const std::string &str) {
        _grid = 0;
        _qos = 0;
        _weight = -1;
        _weightType = 0;
        _authType = 0;

        const std::string delim = " \t\n\r";

        std::string::size_type beg;
        std::string::size_type end = 0;

        beg = str.find_first_not_of(delim, end);
        if (beg == std::string::npos) {
            throw Vin_Endpoint_Exception("Vin_Endpoint::parse error : " + str);
        }

        end = str.find_first_of(delim, beg);
        if (end == std::string::npos) {
            end = str.length();
        }

        std::string desc = str.substr(beg, end - beg);
        if (desc == "tcp") {
            _type = TCP;
        } else if (desc == "ssl") {
            _type = SSL;
        } else if (desc == "udp") {
            _type = UDP;
        } else {
            throw Vin_Endpoint_Exception("Vin_Endpoint::parse tcp or udp or ssl error : " + str);
        }

        desc = str.substr(end);
        end = 0;
        while (true) {
            beg = desc.find_first_not_of(delim, end);
            if (beg == std::string::npos) {
                break;
            }

            end = desc.find_first_of(delim, beg);
            if (end == std::string::npos) {
                end = desc.length();
            }

            std::string option = desc.substr(beg, end - beg);
            if (option.length() != 2 || option[0] != '-') {
                throw Vin_Endpoint_Exception("Vin_Endpoint::parse error : " + str);
            }

            std::string argument;
            std::string::size_type argumentBeg = desc.find_first_not_of(delim, end);
            if (argumentBeg != std::string::npos && desc[argumentBeg] != '-') {
                beg = argumentBeg;
                end = desc.find_first_of(delim, beg);
                if (end == std::string::npos) {
                    end = desc.length();
                }
                argument = desc.substr(beg, end - beg);
            }

            switch (option[1]) {
                case 'h': {
                    if (argument.empty()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -h error : " + str);
                    }
                    const_cast<std::string &>(_host) = argument;
                    break;
                }
                case 'p': {
                    std::istringstream p(argument);
                    if (!(p >> const_cast<int &>(_port)) || !p.eof() || _port < 0 || _port > 65535) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -p error : " + str);
                    }
                    break;
                }
                case 't': {
                    std::istringstream t(argument);
                    if (!(t >> const_cast<int &>(_timeout)) || !t.eof()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -t error : " + str);
                    }
                    break;
                }
                case 'g': {
                    std::istringstream t(argument);
                    if (!(t >> const_cast<int &>(_grid)) || !t.eof()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -g error : " + str);
                    }
                    break;
                }
                case 'q': {
                    std::istringstream t(argument);
                    if (!(t >> const_cast<int &>(_qos)) || !t.eof()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -q error : " + str);
                    }
                    break;
                }
                case 'w': {
                    std::istringstream t(argument);
                    if (!(t >> const_cast<int &>(_weight)) || !t.eof()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -w error : " + str);
                    }
                    break;
                }
                case 'v': {
                    std::istringstream t(argument);
                    if (!(t >> const_cast<unsigned int &>(_weightType)) || !t.eof()) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -v error : " + str);
                    }
                    break;
                }
                    // auth type
                case 'e': {
                    std::istringstream p(argument);
                    if (!(p >> const_cast<int &>(_authType)) || !p.eof() || _authType < 0 || _authType > 1) {
                        throw Vin_Endpoint_Exception("Vin_Endpoint::parse -e error : " + str);
                    }
                    break;
                }
                default: {
                    throw Vin_Endpoint_Exception("Vin_Endpoint::parse error : " + str);
                }
            }
        }

        if (_weightType != 0) {
            if (_weight == -1) {
                _weight = 100;
            }

            _weight = (_weight > 100 ? 100 : _weight);
        }

        if (_host.empty()) {
            throw Vin_Endpoint_Exception("TC_Endpoint::parse error : host must not be empty: " + str);
        } else if (_host == "*") {
            const_cast<std::string &>(_host) = "0.0.0.0";
        }

        if (_authType < 0)
            _authType = 0;
        else if (_authType > 0)
            _authType = 1;

    }

    void Vin_Endpoint::init(const std::string &host, int port, int timeout, int type, int grid, int qos, int weight,
                            unsigned int weighttype, int authType) {
        _host = host;
        _port = port;
        _timeout = timeout;
        _type = type;
        _grid = grid;
        _qos = qos;
        if (weighttype == 0) {
            _weight = -1;
            _weightType = 0;
        } else {
            if (weight == -1) {
                weight = 100;
            }
            _weight = (weight > 100 ? 100 : weight);
            _weightType = weighttype;
        }

        _authType = authType;
    }
}