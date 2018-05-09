//
// Created by cyz on 5/7/18.
//

#include <zconf.h>
#include <iostream>
#include "testThread.h"

testThread::testThread() {}

void testThread::run() {
    while(true){
        sleep(2);

        std::cout<<pthread_self()<<std::endl;
    }
}
