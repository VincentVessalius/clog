//
// Created by cyz on 5/5/18.
//

#ifndef CLOG_SERVER_H
#define CLOG_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <thread>
#include <vector>

//void* StartServer(void*);

class Server {

private:
    sockaddr_un addr;

    short state;

    int listenFd;

    std::string listenPath;

    int maxCncts;

    int maxQuLen;

    std::unique_ptr<std::thread> ptrServerT;
public:
    const std::unique_ptr<std::thread> &getPtrServerT() const;

private:

    std::vector<pthread_t> servantTids;

public:
    //Server(const sockaddr &addr);

    int initServer(std::string);

    int stopServer();

    int restartServer();

    int getState();

    void startServer();
};


#endif //CLOG_SERVER_H
