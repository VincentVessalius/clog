//
// Created by cyz on 5/8/18.
//

#ifndef CLOG_VIN_EXCEPTION_H
#define CLOG_VIN_EXCEPTION_H

#include <stdexcept>
#include <cstring>
#include <cerrno>

namespace vince {

    class Vin_Exception : public std::exception {
    public:
        explicit Vin_Exception(const std::string &str) : _what(str), _errno(0) { if (rcd_backtrace)getBacktrace(); };

        Vin_Exception(const std::string &str, int err) {
            _what = str + " : " + strerror(err);
            _errno = err;
            if (rcd_backtrace)getBacktrace();
        }

        ~Vin_Exception() noexcept override = default;

        const char *what() const noexcept override { return _what.c_str(); };

        int getErrCode() { return _errno; }

        void needBacktrace(bool is) { rcd_backtrace = is; };

        static bool rcd_backtrace;
    private:
        void getBacktrace();

    private:

        int _errno = 0;
        std::string _what = "";
    };

}


#endif //CLOG_VIN_EXCEPTION_H
