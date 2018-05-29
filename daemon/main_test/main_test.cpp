#include <iostream>
#include <queue>
#include <memory>
#include <bits/shared_ptr.h>
#include "util/include/Vin_Http.h"


using namespace std;
//using namespace vince;
static int i=0;
string _simplePath(const string &sPath) {
    string sNewPath;
    deque<string> stPath;
    string::size_type lastPos=0,nowPos=0;

    while(lastPos<sPath.length()){
        string slot;
        if((nowPos=sPath.find('/',lastPos))==string::npos){
            slot=sPath.substr(lastPos);

            if(slot==".."){
                if(stPath.empty())
                    return "Wrong Path";
                stPath.pop_back();
            }else if(slot=="."){
                ;
            }else{
                stPath.push_back(slot);
            }
            break;
        }

        if(nowPos==0&&lastPos==0){
            lastPos++;
            continue;
        }

        slot=sPath.substr(lastPos,nowPos-lastPos);

        if(slot==".."){
            if(stPath.empty())
                return "Wrong Path";
            stPath.pop_back();
        }else if(slot=="."){
            ;
        }else{
            stPath.push_back(slot);
        }

        lastPos=nowPos+1;
    }

    while(!stPath.empty()){
        sNewPath+="/"+stPath.front();
        stPath.pop_front();
    }

    return sNewPath;
}

int main() {

    cout<<i++<<": "<<_simplePath("/sd/../ss")<<endl;
    cout<<i++<<": "<<_simplePath("sd/./ss")<<endl;
    cout<<i++<<": "<<_simplePath("/sd/../../ss")<<endl;
    cout<<i++<<": "<<_simplePath("/sd/.././ss/..")<<endl;



    /***********************************************************************
     * cyz-> Test for Vin_MemoryPool(ThreadSafely)
     *
    Vin_MemoryPool mpool(32,1024);
    Vin_ThreadPool<> tpool;
    tpool.init(2);
    function<void(void)> exec_fn[3] = {
        [&mpool]() -> void { mpool.Allocate(32);},
        [&mpool]() -> void { mpool.Allocate(31);},
        [&mpool]() -> void { mpool.Allocate(64);}
    };

    tpool.exec(make_shared<Vin_Task>(exec_fn[0]));
    tpool.exec(make_shared<Vin_Task>(exec_fn[2]));

    tpool.start();

    tpool.waitForAllDone(-1);

    cout<<mpool.DebugPrint();*/


    /***********************************************************************
     * cyz-> Test for Vin_Singleton and Vin_Base64
     *
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
    */

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
    Vin_ThreadPool<> testPool;
    testPool.init(1);

    auto init_fn = []() -> void { cout << pthread_self() << endl; };

    testPool.setThdInitFunc(make_shared<Vin_Task>(init_fn));

    testPool.start();

    function<void(void)> exec_fn[2] = {
        [] {
            cout << "a:" << pthread_self() << endl;
            sleep(4);
            return;
        },
        [] {
            cout << "b:" << pthread_self() << endl;
            sleep(4);
            return;
        }
    };


    int i = 10;
    while (i) {
        testPool.exec(make_shared<Vin_Task>(exec_fn[i%2]));
        --i;
    }
    testPool.addWorkers(1);

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