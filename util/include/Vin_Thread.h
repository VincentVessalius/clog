//
// Created by cyz on 5/7/18.
//

#ifndef CLOG_VIN_THREAD_H
#define CLOG_VIN_THREAD_H

#include <pthread.h>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Vin_Exception.h"

namespace vince {

    struct Vin_Thread_Exception : public Vin_Exception {
        explicit Vin_Thread_Exception(const std::string &buffer) : Vin_Exception(buffer) {};

        Vin_Thread_Exception(const std::string &buffer, int err) : Vin_Exception(buffer, err) {};

        ~Vin_Thread_Exception() noexcept override = default;
    };

    class Vin_ThreadControl {
    public:
        Vin_ThreadControl()= default;

        explicit Vin_ThreadControl(const std::shared_ptr<std::thread> &);

        Vin_ThreadControl(const Vin_ThreadControl &) = default;

        Vin_ThreadControl &operator=(const Vin_ThreadControl &)= default;

        void join();

        void detach();

        const std::shared_ptr<std::thread> &getptrThread() const;

        void sleep(const long&);

        //yield();

        //void cancelWithTime(const long&));
    private:
        std::shared_ptr<std::thread> _ptrThread;
    };

/////////////////////////////////////TODO
    class Vin_ThreadLock {

    };

    class Vin_Thread {
    public:
        Vin_Thread()= default;

        virtual ~Vin_Thread()= default;

        Vin_ThreadControl start();

        Vin_ThreadControl getThreadControl() const;

        bool isRun() const;


    protected:
        void threadEntry();

        virtual void run()=0;

    protected:
        bool _isrunning = false;
        std::shared_ptr<std::thread> _ptrThread;
        Vin_ThreadControl _thdControl;
        Vin_ThreadLock _lock;
        std::mutex _thdLock;
        std::condition_variable _thdCond;
    };

}
#endif //CLOG_VIN_THREAD_H
