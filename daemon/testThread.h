//
// Created by cyz on 5/7/18.
//

#ifndef CLOG_TESTTHREAD_H
#define CLOG_TESTTHREAD_H

#include "util/include/Vin_Thread.h"

class testThread :public vince::Vin_Thread{
public:
    testThread();

private:

    virtual void run() override;
};


#endif //CLOG_TESTTHREAD_H
