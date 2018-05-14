//
// Created by cyz on 5/10/18.
//

#ifndef CLOG_VIN_COMPARERS_H
#define CLOG_VIN_COMPARERS_H

#include <bits/shared_ptr.h>
#include <memory>
#include "Vin_Task.h"

namespace vince {

    template<typename T>
    class GreaterForPtr {
    public:
        bool operator()(T &a, T &b) const {
            return *a < *b;
        };

    };
}
#endif //CLOG_VIN_COMPARERS_H
