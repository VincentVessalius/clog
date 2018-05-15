//
// Created by cyz on 5/11/18.
//

#ifndef CLOG_VIN_SINGLETON_H
#define CLOG_VIN_SINGLETON_H

#include <mutex>

namespace vince {

    template<typename T, template<typename> class CreatePolicy>
    class Vin_Singleton;

    template<typename T>
    class Vin_Starver {
        friend class Vin_Singleton<T, Vin_Starver>;

    private:
        static T *_get();

    private:
        Vin_Starver() = delete;

        ~Vin_Starver() = delete;

        Vin_Starver(const Vin_Starver &) = delete;

        Vin_Starver &operator=(const Vin_Starver &)= delete;

    private:
        static volatile T *_item;

        static std::mutex _lock;
    };

    template<typename T> volatile T *Vin_Starver<T>::_item;

    template<typename T> std::mutex Vin_Starver<T>::_lock;

    template<typename T>
    T *Vin_Starver<T>::_get() {
        if (_item != nullptr)
            return _item;
        else {
            std::unique_lock<std::mutex> lck(_lock);
            if (_item == nullptr)
                _item = new T;
            return _item;
        }
    }

    template<typename T>
    class Vin_Lazer {
        friend class Vin_Singleton<T, Vin_Lazer>;

    private:
        static T *_get();

    private:
        Vin_Lazer() = delete;

        ~Vin_Lazer() = delete;

        Vin_Lazer(const Vin_Lazer &) = delete;

        Vin_Lazer &operator=(const Vin_Lazer &)= delete;

    private:
        static T _item;
    };

    template<typename T> T Vin_Lazer<T>::_item;

    template<typename T>
    T *Vin_Lazer<T>::_get() {
        return &_item;
    }

    template<typename T, template<typename> class CreatePolicy=Vin_Lazer>
    class Vin_Singleton {
    public:
        static T *getInstance();

    private:
        Vin_Singleton() = delete;

        ~Vin_Singleton() = delete;

        Vin_Singleton(const Vin_Singleton <T> &) = delete;

        Vin_Singleton <T> &operator=(const Vin_Singleton <T> &)= delete;
    };

    template<typename T, template<typename> class CreatePolicy>
    T *Vin_Singleton<T, CreatePolicy>::getInstance() {
        return CreatePolicy<T>::get();
    }

}


#endif //CLOG_VIN_SINGLETON_H
