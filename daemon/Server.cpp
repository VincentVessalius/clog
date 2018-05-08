//
// Created by cyz on 5/5/18.
//

#include <errno.h>
#include "Server.h"
#include "util/include/log.h"
#include "stdio.h"
#include "Servant.h"

using namespace std;

void *NewServant(void *args) {
    int nFd=*((int*)args);
    Servant* sv=new Servant(nFd);
    fprintf(stdout,"servant begin:%d\n",nFd);
    sv->start();
}

void Server::startServer() {
    //Server *s = (Server *) args;
    if (listen(listenFd, maxCncts + maxQuLen) < 0) {
        LOG_ERROR("Listen Failed: %s", strerror(errno));
        return;
    }

    LOG_INFO("Listening on %s", listenPath.c_str());

    while (true) {
        sockaddr_un nClientAddr;
        socklen_t nClientAddrLen = sizeof(nClientAddr);
        memset(&nClientAddr, 0, sizeof(nClientAddr));

        int nClientFd = accept(listenFd, (sockaddr *) &nClientAddr, &nClientAddrLen);
        if (nClientFd < 0) {
            LOG_ERROR("Accept Failed: %s", strerror(errno));
            return;
        }
        pthread_t nClientTid;
        pthread_create(&nClientTid, NULL, NewServant, &nClientFd);
        pthread_detach(nClientTid);
        sleep(2);
        servantTids.push_back(nClientTid);
    }
}

int Server::initServer(std::string path) {
    listenPath = path;

    remove(path.c_str());

    listenFd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path));

    if (bind(listenFd, (sockaddr *) &addr, sizeof(addr))) {
        LOG_ERROR("Bind Failed: %s", strerror(errno));
        return -1;
    };

    std::unique_ptr<std::thread> ptrT(new std::thread(&Server::startServer,this));
    ptrServerT=std::move(ptrT);
    //pthread_create(&serverTid, NULL, StartServer, (void *) this);
    //ptrServerT->join();
    return 0;
}

//TODO
int Server::stopServer() {

    return 0;
}

const unique_ptr<thread> &Server::getPtrServerT() const {
    return ptrServerT;
};
