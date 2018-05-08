//
// Created by cyz on 5/6/18.
//

#include "Servant.h"
#include "util/include/log.h"

Servant::Servant(int& nFd) : cntFd(nFd){}

int Servant::start() {
    int iCnt=0;
    char acBuf[4096];
    while(true){
        while ((iCnt = read(cntFd, acBuf, sizeof(acBuf))))
        {
            printf("Read %d Bytes: %s", iCnt, acBuf);
        }
    }
    return 0;
}
