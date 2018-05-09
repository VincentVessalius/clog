//
// Created by cyz on 5/9/18.
//

#ifndef CLOG_TEST_VIN_TASKPRIORITYQUEUE_H
#define CLOG_TEST_VIN_TASKPRIORITYQUEUE_H

#include <queue>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <assert.h>
#include "Vin_TaskQueue.h"

namespace vince {

    template<typename T, typename D = std::priority_queue<T> >
    class Vin_TaskPriorityQueue :public Vin_TaskQueue<T, D> {
    public:
        Vin_TaskPriorityQueue() = default;
    public:
        typedef D queueType;

        /**
         * @brief 从头部获取数据, 没有数据则等待.
         *
         * @param t
         * @param millsecond   阻塞等待时间(ms)
         *                    0 表示不阻塞
         *                      -1 永久等待
         * @return bool: true, 获取了数据, false, 无数据
         */
        virtual bool pop_front(T &t, size_t millsecond) override ;

        /**
         * @brief 放数据到队列后端.
         *
         * @param t
         */
        virtual void push_back(const T &t) override ;

        /**
         * @brief  放数据到队列后端.
         *
         * @param vt
         */
        virtual void push_back(const queueType &qt)override;



    private:
        /**
         * @brief  放数据到队列前端.
         *
         * @param t
         */
        void push_front(const T &t) final = default;

        /**
         * @brief  放数据到队列前端.
         *
         * @param vt
         */
        void push_front(const queueType &qt) final= default;

    };

    template<typename T, typename D>
    bool Vin_TaskPriorityQueue<T, D>::pop_front(T &t, size_t millsecond) {
        std::unique_lock<std::mutex> lck(_lock);

        if (_queue.empty()) {
            if (millsecond == 0) {
                return false;
            }
            if (millsecond == (size_t) -1) {
                _cond.wait(lck);
            } else {
                //超时了
                if (!_cond.wait_for(lck, millsecond * 1ms)) {
                    return false;
                }
            }
        }

        if (_queue.empty()) {
            return false;
        }

        t = _queue.top();
        _queue.pop();
        assert(_size > 0);
        --_size;

        return true;
    }

    template<typename T, typename D>
    void Vin_TaskPriorityQueue<T, D>::push_back(const T &t) {
        std::unique_lock<std::mutex> lck(_lock);

        _cond.notify_one();

        _queue.push(t);
        ++_size;
    }

    //TODO
    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::push_back(const queueType &qt) {
        std::unique_lock<std::mutex> lck(_lock);

        while (!qt.empty()) {
            _queue.push(qt.top());
            qt.pop();
            ++_size;
            _cond.notify_one();
        }
    }

}


#endif //CLOG_TEST_VIN_TASKPRIORITYQUEUE_H
