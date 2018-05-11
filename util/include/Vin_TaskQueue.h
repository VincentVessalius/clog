//
// Created by cyz on 5/9/18.
//

#ifndef CLOG_VIN_TASKQUEUE_H
#define CLOG_VIN_TASKQUEUE_H

#include <deque>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <assert.h>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace vince {
    template<typename T, typename D = std::deque<T> >
    class Vin_TaskQueue {
    public:
        Vin_TaskQueue() : _size(0) {};
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
        virtual bool pop_front(T &t, size_t millsecond);

        virtual bool pop_front(T &t);

        /**
         * @brief 通知等待在队列上面的线程都醒过来
         */
        void notifyT();

        /**
         * @brief 放数据到队列后端.
         *
         * @param t
         */
        virtual void push_back(const T &t);

        /**
         * @brief  放数据到队列后端.
         *
         * @param vt
         */
        virtual void push_back(const queueType &qt);

        /**
         * @brief  放数据到队列前端.
         *
         * @param t
         */
        virtual void push_front(const T &t);

        /**
         * @brief  放数据到队列前端.
         *
         * @param vt
         */
        virtual void push_front(const queueType &qt);

        /**
         * @brief  等到有数据才交换.
         *
         * @param q
         * @param millsecond  阻塞等待时间(ms)
         *                   0 表示不阻塞
         *                      -1 如果为则永久等待
         * @return 有数据返回true, 无数据返回false
         */
        bool swap(queueType &q, size_t millsecond = 0);

        /**
         * @brief  队列大小.
         *
         * @return size_t 队列大小
         */
        size_t size();

        /**
         * @brief  清空队列
         */
        void clear();

        /**
         * @brief  是否数据为空.
         *
         * @return bool 为空返回true，否则返回false
         */
        bool empty();

    protected:
        size_t _size;

        queueType _queue;

        std::mutex _lock;

        std::condition_variable _cond;
    };

    template<typename T, typename D>
    bool Vin_TaskQueue<T, D>::pop_front(T &t, size_t millsecond) {
        std::unique_lock<std::mutex> lck(_lock);

        if (_queue.empty()) {
            if (millsecond == 0) {
                return false;
            }
            if (millsecond == (size_t) -1) {
                _cond.wait(lck);
            } else {
                //超时了
                if (_cond.wait_for(lck, std::chrono::milliseconds(millsecond)) == std::cv_status::timeout) {
                    return false;
                }
            }
        }

        if (_queue.empty()) {
            return false;
        }

        t = _queue.front();
        _queue.pop_front();
        assert(_size > 0);
        --_size;

        return true;
    }

    template<typename T, typename D>
    bool Vin_TaskQueue<T, D>::pop_front(T &t) {
        pop_front(t, 0);
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::notifyT() {
        std::unique_lock<std::mutex> lck(_lock);

        _cond.notify_all();
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::push_back(const T &t) {
        std::unique_lock<std::mutex> lck(_lock);

        _cond.notify_one();
        _queue.push_back(t);
        ++_size;
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::push_back(const queueType &qt) {
        std::unique_lock<std::mutex> lck(_lock);

        auto it = qt.begin();
        auto itEnd = qt.end();
        while (it != itEnd) {
            _queue.push_back(*it);
            ++it;
            ++_size;
            _cond.notify_one();
        }
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::push_front(const T &t) {
        std::unique_lock<std::mutex> lck(_lock);

        _cond.notify_one();
        _queue.push_front(t);
        ++_size;
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::push_front(const queueType &qt) {
        std::unique_lock<std::mutex> lck(_lock);

        auto it = qt.begin();
        auto itEnd = qt.end();
        while (it != itEnd) {
            _queue.push_front(*it);
            ++it;
            ++_size;
            _cond.notify_one();
        }
    }

    template<typename T, typename D>
    bool Vin_TaskQueue<T, D>::swap(queueType &q, size_t millsecond) {
        std::unique_lock<std::mutex> lck(_lock);

        if (_queue.empty()) {
            if (millsecond == 0) {
                return false;
            }
            if (millsecond == (size_t) -1) {
                _cond.wait(lck);
            } else {
                //超时了
                if (_cond.wait_for(lck, std::chrono::milliseconds(millsecond)) == std::cv_status::timeout) {
                    return false;
                }
            }
        }

        if (_queue.empty()) {
            return false;
        }

        q.swap(_queue);
        //_size = q.size();
        _size = _queue.size();

        return true;
    }

    template<typename T, typename D>
    size_t Vin_TaskQueue<T, D>::size() {
        std::unique_lock<std::mutex> lck(_lock);

        //return _queue.size();
        return _size;
    }

    template<typename T, typename D>
    void Vin_TaskQueue<T, D>::clear() {
        std::unique_lock<std::mutex> lck(_lock);

        _queue.clear();
        _size = 0;
    }

    template<typename T, typename D>
    bool Vin_TaskQueue<T, D>::empty() {
        std::unique_lock<std::mutex> lck(_lock);

        return _queue.empty();
    }

}

#endif //CLOG_VIN_TASKQUEUE_H
