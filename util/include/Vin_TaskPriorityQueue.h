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
#include "Vin_Task.h"
#include "Vin_Comparers.h"

namespace vince {

    template<typename T, typename D = std::deque<T>, typename C=vince::GreaterForPtr<T> >
    class Vin_TaskPriorityQueue : public Vin_TaskQueue<T, D> {
    public:
        Vin_TaskPriorityQueue() = default;
    public:

        typedef std::priority_queue<T,D,C> pqueueType;

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

        virtual bool pop_front(T &t) override ;

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
        virtual void push_back(D &qt);

    protected:
        void push_front(const T &t) final{
            return;
        }

        void push_front(const D &qt) final{
            return;
        };

    protected:
        pqueueType _pqueue;

    };

    template<typename T, typename D,typename C>
    bool Vin_TaskPriorityQueue<T, D, C>::pop_front(T &t, size_t millsecond) {
        std::unique_lock<std::mutex> lck(this->_lock);

        if (this->_pqueue.empty()) {
            if (millsecond == 0) {
                return false;
            }
            if (millsecond == (size_t) -1) {
                this->_cond.wait(lck);
            } else {
                //超时了
                if (this->_cond.wait_for(lck, std::chrono::milliseconds(millsecond)) == std::cv_status::timeout) {
                    return false;
                }
            }
        }

        if (this->_pqueue.empty()) {
            return false;
        }

        t = this->_pqueue.top();
        this->_pqueue.pop();
        assert(this->_size > 0);
        --this->_size;

        return true;
    }

    template<typename T, typename D,typename C>
    bool Vin_TaskPriorityQueue<T,D,C>::pop_front(T &t) {
        pop_front(t,0);
    }

    template<typename T, typename D,typename C>
    void Vin_TaskPriorityQueue<T,D,C>::push_back(const T &t) {
        std::unique_lock<std::mutex> lck(this->_lock);

        this->_cond.notify_one();

        this->_pqueue.push(t);
        ++this->_size;
    }

    //TODO
    template<typename T, typename D,typename C>
    void Vin_TaskPriorityQueue<T, D, C>::push_back(D &qt) {
        std::unique_lock<std::mutex> lck(this->_lock);

        while (!qt.empty()) {
            this->_pqueue.push(qt.front());
            qt.pop_front();
            ++this->_size;
            this->_cond.notify_one();
        }
    }

}


#endif //CLOG_TEST_VIN_TASKPRIORITYQUEUE_H
