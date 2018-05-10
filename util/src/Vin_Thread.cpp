//
// Created by cyz on 5/7/18.
//

#include <thread>
#include "util/include/Vin_Thread.h"


namespace vince {

//cyz-> implementation of Vin_ThreadControl

    Vin_ThreadControl::Vin_ThreadControl(const std::shared_ptr<std::thread> &thread) : _ptr_thread(thread) {}

    void Vin_ThreadControl::join() {
        if (std::this_thread::get_id() == _ptr_thread->get_id()) {
            throw Vin_Thread_Exception("[Vin_ThreadControl::join] can't be called in the same thread");
        }
        _ptr_thread->join();
    }

    void Vin_ThreadControl::detach() {
        if (std::this_thread::get_id() == _ptr_thread->get_id()) {
            throw Vin_Thread_Exception("[Vin_ThreadControl::detach] can't be called in the same thread");
        }
        _ptr_thread->detach();
    }

    const std::shared_ptr<std::thread> &Vin_ThreadControl::getptrThread() const {
        return _ptr_thread;
    }

    void Vin_ThreadControl::sleep(const long &millsecond) {
        struct timespec ts;
        ts.tv_sec = millsecond / 1000;
        ts.tv_nsec = (millsecond % 1000) * 1000000;
        nanosleep(&ts, nullptr);
    }

    Vin_ThreadControl Vin_Thread::getThreadControl() const {
        return _thd_control;
    }

//cyz-> implementation of Vin_Thread

    void Vin_Thread::threadEntry() {
        {
            std::unique_lock<std::mutex> lck(_thd_lock);
            _thd_cond.notify_one();
        }

        run();

        _is_running = false;
    }

    Vin_ThreadControl Vin_Thread::start() {
        std::unique_lock<std::mutex> lck(_thd_lock);
        if (_is_running) { ;//exception;
            throw Vin_Thread_Exception("[Vin_Thread::start] Thread is running");
        }
        std::shared_ptr<std::thread> ptrTmp;
        try {
            _is_running = true;
            ptrTmp = std::make_shared<std::thread>(&Vin_Thread::threadEntry, this);
        } catch (...) {
            _is_running = false;
            throw Vin_Thread_Exception("[Vin_Thread::start] Thread start error");
        }

        _ptr_thread = std::move(ptrTmp);
        _thd_control = Vin_ThreadControl(_ptr_thread);
        _thd_cond.wait(lck);

        return _thd_control;
    }


    bool Vin_Thread::isRun() const {
        return _is_running;
    }

}