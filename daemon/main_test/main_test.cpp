#include <iostream>
#include <queue>
#include <memory>
#include <util/include/Vin_TaskPriorityQueue.h>
#include <bits/shared_ptr.h>
#include "util/include/Vin_Task.h"
#include "util/include/log.h"
#include "daemon/Server.h"
#include "daemon/testThread.h"
#include "util/include/Vin_Exception.h"
#include "util/include/Vin_Comparers.h"
#include "util/include/Vin_ThreadPool.h"
#include "util/include/Vin_Singleton.h"
#include "util/include/Vin_Codec.h"
#include "util/include/Vin_Base64.h"

using namespace std;
using namespace vince;

int main() {

    /***********************************************************************
     * cyz-> Test for Vin_Singleton and Vin_Base64
     **/
    Vin_Codec* ccc=Vin_Singleton<Vin_Base64>::getInstance();
    string x;
    try {
        x=ccc->encode("");
    }catch (Vin_Codec_Exception ex){
        cout<<ex.what();
    }

    try {
        ccc->decode(x);
    }catch (Vin_Codec_Exception ex){
        cout<<ex.what();
    }
    /**/

    /***********************************************************************
     * cyz-> Test for Vin_ThreadPool using Vin_TaskPriorityQueue
     *
    Vin_ThreadPool<std::shared_ptr<Vin_Task>,Vin_TaskPriorityQueue<std::shared_ptr<Vin_Task> > > testPool;
    testPool.init(4);

    auto init_fn = []() -> void { cout << pthread_self()<<": 0" << endl; };
    testPool.setThdInitFunc(make_shared<Vin_Task>(init_fn));

    function<void(void)> exec_fn[3] = {[]() -> void { cout <<pthread_self()<<": 4"<< endl; sleep(1);},
                                       []() -> void { cout << pthread_self()<<": 5" << endl;sleep(1);},
                                       []() -> void { cout << pthread_self()<<": 2"<< endl; sleep(1);}};

    testPool.exec(make_shared<Vin_Task>(exec_fn[0],4));
    testPool.exec(make_shared<Vin_Task>(exec_fn[1],5));
    testPool.exec(make_shared<Vin_Task>(exec_fn[2],2));
    testPool.exec(make_shared<Vin_Task>(exec_fn[0],4));
    testPool.exec(make_shared<Vin_Task>(exec_fn[0],4));
    testPool.exec(make_shared<Vin_Task>(exec_fn[1],5));
    testPool.exec(make_shared<Vin_Task>(exec_fn[2],2));
    testPool.exec(make_shared<Vin_Task>(exec_fn[1],5));
    testPool.exec(make_shared<Vin_Task>(exec_fn[2],2));

    testPool.start();

    //等待线程结束
    cout << "waitForAllDone..." << endl;
    bool b = testPool.waitForAllDone(-1);
    cout << "waitForAllDone..." << b << ":" << testPool.getJobNum() << endl;

    testPool.stop();
    */

    /***********************************************************************
     * cyz-> Test for Vin_ThreadPool and Vin_Task
     *
    Vin_ThreadPool testPool;
    testPool.init(4);

    auto init_fn = []() -> void { cout << pthread_self() << endl; };

    testPool.setThdInitFunc(make_shared<Vin_Task>(init_fn));

    testPool.start();

    function<void(void)> exec_fn[2] = {
        [] {
            cout << "a:" << pthread_self() << endl;
            sleep(1);
            return;
        },
        [] {
            cout << "b:" << pthread_self() << endl;
            sleep(1);
            return;
        }
    };


    int i = 10;
    while (i) {
        testPool.exec(make_shared<Vin_Task>(exec_fn[i%2]));
        --i;
    }

    //等待线程结束
    cout << "waitForAllDone..." << endl;
    bool b = testPool.waitForAllDone(-1);
    cout << "waitForAllDone..." << b << ":" << testPool.getJobNum() << endl;

    testPool.stop();*/

    /***********************************************************************
     * cyz-> Test for Vin_Thread
     *
    testThread x;
    x.start();
    std::cout << "Hello, World!" << std::endl;
    vince::Vin_ThreadControl t=x.getThreadControl();

    vince::Vin_Exception xx("hi");
    std::cout<<xx.what()<<std::endl;
    t.join();*/

    /***********************************************************************
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