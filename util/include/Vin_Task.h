//
// Created by cyz on 5/10/18.
//

#ifndef CLOG_VIN_TASK_H
#define CLOG_VIN_TASK_H

#include <functional>

namespace vince {
    typedef void (*func)();

    class Vin_Task {
    public:

        Vin_Task();

        Vin_Task(std::function<void(void)>);

        Vin_Task(std::function<void(void)>, int);

        Vin_Task &operator=(const Vin_Task &);

        bool operator<(const Vin_Task &) const;

    public:
        void set_priority(int);

        int get_priority() const;

        void operator()(void);

    private:
        std::function<void(void)> _task;

        int _priority;
    };
}


#endif //CLOG_VIN_TASK_H
