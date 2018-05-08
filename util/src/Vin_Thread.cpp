//
// Created by cyz on 5/7/18.
//

#include <thread>
#include "util/include/Vin_Thread.h"


namespace vince {

//cyz-> implementation of Vin_ThreadControl

    Vin_ThreadControl::Vin_ThreadControl(const std::shared_ptr<std::thread> &thread) : _ptrThread(thread) {}

    void Vin_ThreadControl::join() {
        if(std::this_thread::get_id()==_ptrThread->get_id()){
            throw Vin_Thread_Exception("[Vin_ThreadControl::join] can't be called in the same thread");
        }
        _ptrThread->join();
    }

    void Vin_ThreadControl::detach() {
        if(std::this_thread::get_id()==_ptrThread->get_id()){
            throw Vin_Thread_Exception("[Vin_ThreadControl::detach] can't be called in the same thread");
        }
        _ptrThread->detach();
    }

    const std::shared_ptr<std::thread> &Vin_ThreadControl::getptrThread() const {
        return _ptrThread;
    }

    void Vin_ThreadControl::sleep(const long & millsecond) {
        struct timespec ts;
        ts.tv_sec = millsecond / 1000;
        ts.tv_nsec = (millsecond % 1000)*1000000;
        nanosleep(&ts, nullptr);
    }

    Vin_ThreadControl Vin_Thread::getThreadControl() const {
        return _thdControl;
    }

//cyz-> implementation of Vin_Thread

    void Vin_Thread::threadEntry() {
        {
            std::unique_lock<std::mutex> lck(_thdLock);
            _thdCond.notify_one();
        }

        run();

        _isrunning=false;
    }

    Vin_ThreadControl Vin_Thread::start() {
        std::unique_lock<std::mutex> lck(_thdLock);
        if (_isrunning) { ;//exception;
            throw Vin_Thread_Exception("[Vin_Thread::start] Thread is running");
        }
        std::shared_ptr<std::thread> ptrTmp;
        try {
            _isrunning = true;
            ptrTmp=std::make_shared<std::thread>(&Vin_Thread::threadEntry, this);
        }catch (...){
            _isrunning=false;
            throw Vin_Thread_Exception("[Vin_Thread::start] Thread start error");
        }

        _ptrThread = std::move(ptrTmp);
        _thdControl = Vin_ThreadControl(_ptrThread);
        _thdCond.wait(lck);

        return _thdControl;
    }


    bool Vin_Thread::isRun() const {
        return _isrunning;
    }

}