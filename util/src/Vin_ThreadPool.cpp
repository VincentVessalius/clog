//
// Created by cyz on 5/8/18.
//

#include "util/include/Vin_ThreadPool.h"

namespace vince {

/////////////////////////////////////////////////////////////////
//cyz-> Implementation of ThreadData

    pthread_key_t Vin_ThreadPool::g_key;

    Vin_ThreadPool::KeyInitializer Vin_ThreadPool::g_key_initializer;

    void Vin_ThreadPool::destructor(void *p) {
        ThreadData *ttd = (ThreadData *) p;
        delete ttd;
    }

    void Vin_ThreadPool::setThreadData(Vin_ThreadPool::ThreadData *p) {
        Vin_ThreadPool::ThreadData *pOld = getThreadData();
        if (pOld == p)
            return;
        if (pOld != NULL)
            delete pOld;

        int ret = pthread_setspecific(g_key, (void *) p);
        if (ret != 0) {
            throw Vin_ThreadPool_Exception("[Vin_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    Vin_ThreadPool::ThreadData *Vin_ThreadPool::getThreadData() {
        return (ThreadData *) pthread_getspecific(g_key);
    }

    void Vin_ThreadPool::setThreadData(pthread_key_t pkey, ThreadData *p) {
        Vin_ThreadPool::ThreadData *pOld = getThreadData(pkey);
        if (pOld == p)
            return;
        if (pOld != NULL)
            delete pOld;

        int ret = pthread_setspecific(pkey, (void *) p);
        if (ret != 0) {
            throw Vin_ThreadPool_Exception("[Vin_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    Vin_ThreadPool::ThreadData *Vin_ThreadPool::getThreadData(pthread_key_t pkey) {
        return (ThreadData *) pthread_getspecific(pkey);
    }

////////////////////////////////////////////////////////////////
//cyz-> Implementation of ThreadWorker
    void Vin_ThreadPool::ThreadWorker::terminate() {
        _b_terminate = true;
        //cyz-> wake up all threads who are using ThreadPool::get(ThreadWorker)
        _tpool->notifyT();
    }

    Vin_ThreadPool::ThreadWorker::ThreadWorker(Vin_ThreadPool *tpool) : _tpool(tpool), _b_terminate(false) {

    }

    void Vin_ThreadPool::ThreadWorker::run() {
        //调用初始化部分
        std::shared_ptr<Vin_Function> pst = _tpool->get();
        if (pst) {
            try {
                (*pst)();
            }
            catch (...) {
            }
            pst = NULL;
        }

        //调用处理部分
        while (!_b_terminate) {
            std::shared_ptr<Vin_Function> pfw = _tpool->get(this);
            if (pfw != NULL) {

                try {
                    (*pfw)();
                }
                catch (...) {
                }

                _tpool->idle(this);
            }
        }

        //结束
        _tpool->exit();
    }

////////////////////////////////////////////////////////////////
//cyz-> Implementation of ThreadPool
    void Vin_ThreadPool::init(const size_t &num) {
        stop();

        std::unique_lock<std::mutex> lck(_lock);

        clear();

        for (size_t i = 0; i < num; i++) {
            _jobthread.push_back(new ThreadWorker(this));
        }
    }

    void Vin_ThreadPool::stop() {
        std::unique_lock<std::mutex> lck(_lock);

        auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            if ((*it)->isRun()) {
                (*it)->terminate();
                (*it)->getThreadControl().join();
            }
            ++it;
        }
        _bAllDone = true;
    }

    void Vin_ThreadPool::clear() {
        /*auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            delete (*it);
            ++it;
        }*/
        _jobthread.clear();
        _busythread.clear();
    }

    void Vin_ThreadPool::start() {
        std::unique_lock<std::mutex> lck(_lock);

        auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            (*it)->start();
            ++it;
        }
        _bAllDone = false;
    }

    bool Vin_ThreadPool::isFinish() {
        return _startqueue.empty() && _jobqueue.empty() && _busythread.empty() && _bAllDone;
    }

    void Vin_ThreadPool::idle(ThreadWorker * const ptw) {
        std::unique_lock<std::mutex> lck(_tqlock);
        _busythread.erase(ptw);

        //无繁忙线程, 通知等待在线程池结束的线程醒过来
        if (_busythread.empty()) {
            _bAllDone = true;
            _tqcond.notify_all();
        }
    }

    //TODO
    void Vin_ThreadPool::exit() {
        Vin_ThreadPool::ThreadData *p = getThreadData();
        if (p) {
            delete p;
            int ret = pthread_setspecific(g_key, NULL);
            if (ret != 0) {
                throw Vin_ThreadPool_Exception("[TC_ThreadPool::setThreadData] pthread_setspecific error", ret);
            }
        }

        _jobqueue.clear();
    }

    bool Vin_ThreadPool::waitForAllDone(int millsecond) {
        std::unique_lock<std::mutex> lck(_tqlock);

        //永远等待
        while (millsecond < 0) {
            if (isFinish()) {
                return true;
            }
            _tqcond.wait_for(lck, std::chrono::seconds(1));
        }

        std::cv_status b = _tqcond.wait_for(lck, std::chrono::milliseconds(millsecond));
        //完成处理了
        if (isFinish()) {
            return true;
        }

        if (b == std::cv_status::timeout) {
            return false;
        }

        return false;
    }

    std::shared_ptr<Vin_Function> Vin_ThreadPool::get(ThreadWorker* const ptw)
    {
        std::shared_ptr<Vin_Function> pFunctor = NULL;
        if(!_jobqueue.pop_front(pFunctor, 1000))
        {
            return NULL;
        }

        {
            std::unique_lock<std::mutex> lck(_tqlock);
            _busythread.insert(ptw);
        }

        return pFunctor;
    }

    std::shared_ptr<Vin_Function> Vin_ThreadPool::get()
    {
        std::shared_ptr<Vin_Function> pFunctor = NULL;
        if(!_startqueue.pop_front(pFunctor))
        {
            return NULL;
        }

        return pFunctor;
    }

    void Vin_ThreadPool::notifyT() {
        _jobqueue.notifyT();
    }

    Vin_ThreadPool::Vin_ThreadPool() :_bAllDone(true){

    }

    Vin_ThreadPool::~Vin_ThreadPool() {
        stop();
        clear();
    }

    size_t Vin_ThreadPool::getThreadNum() {
        std::unique_lock<std::mutex> lck(_lock);
        return _jobthread.size();
    }

    size_t Vin_ThreadPool::getJobNum() {
        std::unique_lock<std::mutex> lck(_lock);
        return _jobqueue.size();
    }


    void Vin_ThreadPool::setThdInitFunc(std::shared_ptr<Vin_Function> init_fn) {
        for(size_t i = 0; i < _jobthread.size(); i++)
        {
            _startqueue.push_back(init_fn);
        }
    }

    void Vin_ThreadPool::exec(std::shared_ptr<Vin_Function> init_fn) {
        _jobqueue.push_back(init_fn);
    }


}