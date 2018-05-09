//
// Created by cyz on 5/8/18.
//

#ifndef CLOG_VIN_THREADPOOL_H
#define CLOG_VIN_THREADPOOL_H

#include <vector>
#include <set>
#include "Vin_Thread.h"
#include "Vin_TaskQueue.h"

namespace vince {
    struct Vin_ThreadPool_Exception : public Vin_Exception {
        explicit Vin_ThreadPool_Exception(const std::string &buffer) : Vin_Exception(buffer) {};

        Vin_ThreadPool_Exception(const std::string &buffer, int err) : Vin_Exception(buffer, err) {};

        ~Vin_ThreadPool_Exception() noexcept override = default;
    };

    typedef std::function<void(void)> Vin_Function;

    class Vin_ThreadPool {
    public:
        Vin_ThreadPool();

        virtual ~Vin_ThreadPool();

        void init(const size_t &);

        size_t getThreadNum();

        size_t getJobNum();

        void start();

        void stop();

        void setThdInitFunc(std::shared_ptr<Vin_Function>);

        void exec(std::shared_ptr<Vin_Function>);

        bool waitForAllDone(int millsecond = -1);

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
            virtual void run();

        protected:

            Vin_ThreadPool *_tpool;

            bool _b_terminate;
        };

    protected:
        void notifyT();

        bool isFinish();

        void clear();

        void exit();

        void idle(ThreadWorker * const);

        std::shared_ptr<Vin_Function> get(ThreadWorker* const);

        std::shared_ptr<Vin_Function> get();

    protected:

        /**
         * 任务队列
         */
        Vin_TaskQueue<std::shared_ptr<Vin_Function> > _jobqueue;

        /**
         * 启动任务
         */
        Vin_TaskQueue<std::shared_ptr<Vin_Function> > _startqueue;

        /**
         * 工作线程
         */
        std::vector<ThreadWorker* > _jobthread;

        /**
         * 繁忙线程
         */
        std::set<ThreadWorker* > _busythread;

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
    };
}


#endif //CLOG_VIN_THREADPOOL_H
