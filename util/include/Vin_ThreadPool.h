//
// Created by cyz on 5/8/18.
//

#ifndef CLOG_VIN_THREADPOOL_H
#define CLOG_VIN_THREADPOOL_H

class KeyInitializer;

class KeyInitializer;

#include <vector>
#include <set>
#include "Vin_Thread.h"
#include "Vin_TaskQueue.h"
#include "Vin_Task.h"

namespace vince {
    struct Vin_ThreadPool_Exception : public Vin_Exception {
        explicit Vin_ThreadPool_Exception(const std::string &buffer) noexcept : Vin_Exception(buffer) {};

        Vin_ThreadPool_Exception(const std::string &buffer, int err) noexcept : Vin_Exception(buffer, err) {};

        ~Vin_ThreadPool_Exception() noexcept override = default;
    };

    template<typename T=std::shared_ptr<Vin_Task>, typename Q=vince::Vin_TaskQueue<T> >
    class Vin_ThreadPool {
    public:
        Vin_ThreadPool();

        virtual ~Vin_ThreadPool();

        void init(const size_t &);

        void start();

        void stop();

        void addWorkers(const int& num);

        void setThdInitFunc(const T &);

        void exec(const T &);

        bool waitForAllDone(int millsecond = -1);

        size_t getThreadNum();

        size_t getJobNum();

    public:
        /**
         * @brief 线程数据基类,所有线程的私有数据继承于该类
         */
        class ThreadData {
        public://cyz-> you have to override this class
            /**
             * @brief 构造
             */
            ThreadData() = default;

            /**
             * @brief 析够
             */
            virtual ~ThreadData() = default;

        };

        /**
         * @brief 设置线程数据.
         *
         * @param p 线程数据
         */
        static void setThreadData(ThreadData *p);

        /**
         * @brief 获取线程数据.
         *
         * @return ThreadData* 线程数据
         */
        static ThreadData *getThreadData();

        /**
         * @brief 设置线程数据, key需要自己维护.
         *
         * @param pkey 线程私有数据key
         * @param p    线程指针
         */
        static void setThreadData(pthread_key_t pkey, ThreadData *p);

        /**
         * @brief 获取线程数据, key需要自己维护.
         *
         * @param pkey 线程私有数据key
         * @return     指向线程的ThreadData*指针
         */
        static ThreadData *getThreadData(pthread_key_t pkey);

    protected:
        /**
         * @brief 释放资源.
         *
         * @param p
         */
        static void destructor(void *p);

        /**
         * @brief 初始化key
         */
        class KeyInitializer {
        public:
            /**
             * @brief 初始化key
             */
            KeyInitializer() {
                int ret = pthread_key_create(&Vin_ThreadPool::g_key, Vin_ThreadPool::destructor);
                if (ret != 0) {
                    throw Vin_ThreadPool_Exception("[Vin_ThreadPool::KeyInitialize] pthread_key_create error", ret);
                }
            }

            /**
             * @brief 释放key
             */
            ~KeyInitializer() {
                pthread_key_delete(Vin_ThreadPool::g_key);
            }
        };

        /**
         * @brief 数据key
         */
        static pthread_key_t g_key;

        /**
         * @brief 初始化key的控制
         */
        static KeyInitializer g_key_initializer;


    protected:
        class ThreadWorker : public Vin_Thread {
        public:
            explicit ThreadWorker(Vin_ThreadPool *tpool);

            void terminate();

        protected:
            virtual void run() override;

        protected:

            Vin_ThreadPool *_tpool;

            bool _b_terminate;
        };

    protected:
        void _notifyT();

        bool _isFinish();

        void _clear();

        void _exit();

        void _idle(ThreadWorker *const);

        T _get(ThreadWorker *const);

        T _get();

    protected:

        /**
         * 任务队列
         */
        Q _jobqueue;

        /**
         * init任务
         */
        T _initjob=nullptr ;

        /**
         * 工作线程
         */
        std::vector<ThreadWorker *> _jobthread;

        /**
         * 繁忙线程
         */
        std::set<ThreadWorker *> _busythread;

        /**
         * lock for ThreadPool
         */
        std::mutex _lock;

        /**
         * 任务队列的锁
         */
        std::mutex _tqlock;


        std::condition_variable _tqcond;

        /**
         * 是否所有任务都执行完毕
         */
        bool _bAllDone;

        /**
         * is Start?
         */
        bool _bStarted;
    };

/////////////////////////////////////////////////////////////////
//cyz-> Implementation of ThreadData
    template<typename T, typename Q> pthread_key_t Vin_ThreadPool<T, Q>::g_key;

    template<typename T, typename Q> typename Vin_ThreadPool<T, Q>::KeyInitializer Vin_ThreadPool<T, Q>::g_key_initializer;

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::destructor(void *p) {
        auto *ttd = (Vin_ThreadPool<T, Q>::ThreadData *) p;
        delete ttd;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::setThreadData(Vin_ThreadPool<T, Q>::ThreadData *p) {
        Vin_ThreadPool<T, Q>::ThreadData *pOld = getThreadData();
        if (pOld == p)
            return;
        if (pOld != NULL)
            delete pOld;

        int ret = pthread_setspecific(g_key, (void *) p);
        if (ret != 0) {
            throw Vin_ThreadPool_Exception("[Vin_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    template<typename T, typename Q>
    typename Vin_ThreadPool<T, Q>::ThreadData *Vin_ThreadPool<T, Q>::getThreadData() {
        return (Vin_ThreadPool<T, Q>::ThreadData *) pthread_getspecific(g_key);
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::setThreadData(pthread_key_t pkey, ThreadData *p) {
        Vin_ThreadPool<T, Q>::ThreadData *pOld = getThreadData(pkey);
        if (pOld == p)
            return;
        if (pOld != NULL)
            delete pOld;

        int ret = pthread_setspecific(pkey, (void *) p);
        if (ret != 0) {
            throw Vin_ThreadPool_Exception("[Vin_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    template<typename T, typename Q>
    typename Vin_ThreadPool<T, Q>::ThreadData *Vin_ThreadPool<T, Q>::getThreadData(pthread_key_t pkey) {
        return (ThreadData *) pthread_getspecific(pkey);
    }

////////////////////////////////////////////////////////////////
//cyz-> Implementation of ThreadWorker
    template<typename T, typename Q>
    Vin_ThreadPool<T, Q>::ThreadWorker::ThreadWorker(Vin_ThreadPool<T, Q> *tpool) : _tpool(tpool), _b_terminate(false) {

    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::ThreadWorker::run() {
        //调用初始化部分
        auto pst = _tpool->_get();

        if (pst != NULL) {
            try {
                (*pst)();
            }
            catch (...) {
            }

            pst = NULL;
        }

        //调用处理部分
        while (!_b_terminate) {
            auto pfw = _tpool->_get(this);
            if (pfw != NULL) {
                try {
                    (*pfw)();
                }
                catch (...) {
                }
            }
            pfw = NULL;
            _tpool->_idle(this);
        }

        //结束
        _tpool->_exit();
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::ThreadWorker::terminate() {
        _b_terminate = true;
        //cyz-> wake up all threads who are using ThreadPool::_get(ThreadWorker)
        _tpool->_notifyT();
    }

////////////////////////////////////////////////////////////////
//cyz-> Public Implementation of ThreadPool
    template<typename T, typename Q>
    Vin_ThreadPool<T, Q>::Vin_ThreadPool() : _bAllDone(true),_bStarted(false),_initjob(nullptr) {

    }

    template<typename T, typename Q>
    Vin_ThreadPool<T, Q>::~Vin_ThreadPool() {
        stop();
        _clear();
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::init(const size_t &num) {
        stop();

        std::unique_lock<std::mutex> lck(_lock);

        _clear();

        for (size_t i = 0; i < num; i++) {
            _jobthread.push_back(new ThreadWorker(this));
        }
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::stop() {
        std::unique_lock<std::mutex> lck(_lock);

        _bStarted= false;

        auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            if ((*it)->isRun()) {
                (*it)->terminate();
                (*it)->getThreadControl().join();
            }
            ++it;
        }

        _initjob= nullptr;
        _jobqueue.clear();

        _bAllDone = true;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::start() {
        std::unique_lock<std::mutex> lck(_lock);

        _bStarted=true;

        auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            (*it)->start();
            ++it;
        }
        _bAllDone = false;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::setThdInitFunc(const T &init_fn) {
        _initjob=init_fn;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::exec(const T &init_fn) {
        _jobqueue.push_back(init_fn);
    }

    template<typename T, typename Q>
    bool Vin_ThreadPool<T, Q>::waitForAllDone(int millsecond) {
        std::unique_lock<std::mutex> lck(_tqlock);

        //永远等待
        while (millsecond < 0) {
            if (_isFinish()) {
                return true;
            }
            _tqcond.wait_for(lck, std::chrono::seconds(10));
        }

        std::cv_status b = _tqcond.wait_for(lck, std::chrono::milliseconds(millsecond));
        //完成处理了
        if (_isFinish()) {
            return true;
        }

        return false;
    }

    template<typename T, typename Q>
    size_t Vin_ThreadPool<T, Q>::getThreadNum() {
        std::unique_lock<std::mutex> lck(_lock);
        return _jobthread.size();
    }

    template<typename T, typename Q>
    size_t Vin_ThreadPool<T, Q>::getJobNum() {
        std::unique_lock<std::mutex> lck(_lock);
        return _jobqueue.size();
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::addWorkers(const int& num) {
        if(num==0)
            return;
        else if(num>0){
            std::unique_lock<std::mutex> lck(_lock);

            for (size_t i = 0; i < num; i++) {
                _jobthread.push_back(new ThreadWorker(this));
                if(_bStarted)(*_jobthread.rbegin())->start();
            }
        }else {
            int orig_num=this->getThreadNum();
            if(orig_num+num<0){
                throw Vin_ThreadPool_Exception("[Vin_ThreadPool::addWorkers] the new num of thds is invalid!");
            }
            init(orig_num+num);
        }
    }

//cyz-> protected and private Implementation of ThreadPool
    template<typename T, typename Q>
    bool Vin_ThreadPool<T, Q>::_isFinish() {
        return _jobqueue.empty() && _busythread.empty() && _bAllDone;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::_idle(ThreadWorker *const ptw) {
        std::unique_lock<std::mutex> lck(_tqlock);
        _busythread.erase(ptw);

        //无繁忙线程, 通知等待在线程池结束的线程醒过来
        if (_busythread.empty()) {
            _bAllDone = true;
            _tqcond.notify_one();
        }
    }

    //TODO
    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::_exit() {
        Vin_ThreadPool<T, Q>::ThreadData *p = getThreadData();
        if (p) {
            delete p;
            int ret = pthread_setspecific(g_key, NULL);
            if (ret != 0) {
                throw Vin_ThreadPool_Exception("[Vin_ThreadPool::setThreadData] pthread_setspecific error", ret);
            }
        }
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::_clear() {
        auto it = _jobthread.begin();
        while (it != _jobthread.end()) {
            delete (*it);
            ++it;
        }
        _jobthread.clear();
        _busythread.clear();
    }

    template<typename T, typename Q>
    T Vin_ThreadPool<T, Q>::_get(ThreadWorker *const ptw) {
        T pFunctor = NULL;
        if (!_jobqueue.pop_front(pFunctor, 1000)) {
            return NULL;
        }

        std::unique_lock<std::mutex> lck(_tqlock);
        _busythread.insert(ptw);

        return pFunctor;
    }

    template<typename T, typename Q>
    T Vin_ThreadPool<T, Q>::_get() {
        return _initjob;
    }

    template<typename T, typename Q>
    void Vin_ThreadPool<T, Q>::_notifyT() {
        _jobqueue.notifyT();
    }

}


#endif //CLOG_VIN_THREADPOOL_H
