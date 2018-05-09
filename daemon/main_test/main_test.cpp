#include <iostream>
#include "util/include/log.h"
#include "daemon/Server.h"
#include "daemon/testThread.h"
#include "util/include/Vin_Exception.h"
#include "util/include/Vin_ThreadPool.h"

using namespace std;
using namespace vince;

int main() {
    Vin_ThreadPool testPool;
    testPool.init(1);

    auto init_fn = []() -> void { cout << pthread_self() << endl; };

    testPool.setThdInitFunc(make_shared<function<void(void)>>(init_fn));

    testPool.start();

    auto exec_fn = [] {
        cout << "a:" << pthread_self() << endl;
        sleep(1);
        return;
    };

    int i = 10;
    while (i) {
        testPool.exec(make_shared<function<void(void)>>(exec_fn));
        --i;
    }

    //等待线程结束
    cout << "waitForAllDone..." << endl;
    bool b = testPool.waitForAllDone(-1);
    cout << "waitForAllDone..." << b << ":" << testPool.getJobNum() << endl;

    testPool.stop();

    /**
     * cyz-> Test for Vin_Thread
     *
    testThread x;
    x.start();
    std::cout << "Hello, World!" << std::endl;
    vince::Vin_ThreadControl t=x.getThreadControl();

    vince::Vin_Exception xx("hi");
    std::cout<<xx.what()<<std::endl;
    t.join();*/

    /**
     * cyz-> Test for Log and OldServer
     *
    std::string tmpDir="/tmp/clog";
    InitLog(tmpDir);

    Server s;//=new Server;
    s.initServer("/tmp/clog/unix.sc");

    s.getPtrServerT()->join();
    std::cout << "Hello, World!" << std::endl;*/
    return 0;
}