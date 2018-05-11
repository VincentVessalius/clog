//
// Created by cyz on 5/10/18.
//

#include <iostream>
#include <memory>
#include "util/include/Vin_Task.h"

namespace vince{
    Vin_Task::Vin_Task() :_task(NULL),_priority(0){

    }

    Vin_Task::Vin_Task(std::function<void(void)> ts) :_task(ts),_priority(0){

    }

    Vin_Task::Vin_Task(std::function<void(void)> ts,int prt) :_task(ts),_priority(prt){

    }

    void Vin_Task::operator() (void){
        if(_task!=NULL)
            _task();
    }

    int Vin_Task::get_priority() const {
        return _priority;
    }

    void Vin_Task::set_priority(int prt) {
        _priority = prt;
    }

    Vin_Task &Vin_Task::operator=(const Vin_Task & t) {
        _priority=t._priority;
        _task=t._task;
        return *this;
    }

    bool Vin_Task::operator<(const Vin_Task & b) const {
        return this->get_priority()<b.get_priority();
    }

}

