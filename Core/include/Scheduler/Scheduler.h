//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_SCHEDULER_H
#define EDGECAFFE_SCHEDULER_H
#include <map>
#include <mutex>
#include <NetworkRegistry.h>
#include <MemoryCounter.h>
#include <Tasks/Task.h>


namespace EdgeCaffe {
    class Scheduler
    {
    protected:
        std::mutex mtx;
        std::shared_ptr<MemoryCounter> mc;
        std::shared_ptr<NetworkRegistry> nr;
        bool *masaEnabled = nullptr;

    public:
        virtual void addTask(Task *t_ptr) = 0;

        virtual bool getNext(Task **t_ptr) = 0;
        virtual bool isEmpty() = 0;

        Scheduler(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr) : mc(mc), nr(nr)
        {}
    };
}

#endif //EDGECAFFE_SCHEDULER_H