//
// Created by cyz on 5/17/18.
//

#include <zconf.h>
#include "util/include/Vin_Epoller.h"

namespace vince {

    Vin_Epoller::Vin_Epoller(bool bEt) : _iEpollfd(-1), _max_connections(1024), _pevs(nullptr), _et(bEt) {

    }

    Vin_Epoller::~Vin_Epoller() {
        if (_pevs != nullptr)
            delete[] _pevs;

        if (_iEpollfd > 0)
            ::close(_iEpollfd);
    }

    void Vin_Epoller::ctrl(int fd, long long data, __uint32_t events, int op) {
        epoll_event ev;
        ev.data.u64 = data;
        if (_et) {
            ev.events = events | EPOLLET;
        } else {
            ev.events = events;
        }

        epoll_ctl(_iEpollfd, op, fd, &ev);
    }

    void Vin_Epoller::create(int max_connections) {
        _max_connections = max_connections;
        _iEpollfd = epoll_create(_max_connections + 1);

        if (_pevs != nullptr) {
            delete[] _pevs;
        }

        _pevs = new epoll_event[_max_connections + 1];
    }

    void Vin_Epoller::add(int fd, long long data, __uint32_t event) {
        ctrl(fd, data, event, EPOLL_CTL_ADD);
    }

    void Vin_Epoller::mod(int fd, long long data, __uint32_t event) {
        ctrl(fd, data, event, EPOLL_CTL_MOD);
    }

    void Vin_Epoller::del(int fd, long long data, __uint32_t event) {
        ctrl(fd, data, event, EPOLL_CTL_DEL);
    }

    int Vin_Epoller::wait(int millsecond) {
        return epoll_wait(_iEpollfd, _pevs, _max_connections + 1, millsecond);;
    }

    epoll_event &Vin_Epoller::get(int i) {
        assert(_pevs != nullptr);
        return _pevs[i];
    }
}