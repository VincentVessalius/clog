#include <iostream>
#include "util/include/log.h"
#include "Server.h"
#include "testThread.h"
#include "util/include/Vin_Exception.h"

int main() {
    testThread x;
    x.start();
    std::cout << "Hello, World!" << std::endl;
    vince::Vin_ThreadControl t=x.getThreadControl();

    vince::Vin_Exception xx("hi");
    std::cout<<xx.what()<<std::endl;
    t.join();

    /*std::string tmpDir="/tmp/clog";
    InitLog(tmpDir);

    Server s;//=new Server;
    s.initServer("/tmp/clog/unix.sc");

    s.getPtrServerT()->join();
    std::cout << "Hello, World!" << std::endl;*/
    return 0;
}