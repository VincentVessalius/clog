//
// Created by cyz on 5/8/18.
//

#include "util/include/Vin_Exception.h"
#include <execinfo.h>

namespace vince {
    bool Vin_Exception::rcd_backtrace= false;

    void Vin_Exception::getBacktrace() {
        void *array[64];
        int nSize = backtrace(array, 64);
        char **symbols = backtrace_symbols(array, nSize);

        _what += "\n";
        for (int i = 0; i < nSize; i++) {
            _what += symbols[i];
            _what += "\n";
        }
        free(symbols);
    }

}