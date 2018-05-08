//
// Created by cyz on 5/6/18.
//

#ifndef CLOG_SERVANT_H
#define CLOG_SERVANT_H


class Servant {
private:
    int cntFd;
public:
    Servant(int&);
    int start();
};


#endif //CLOG_SERVANT_H
