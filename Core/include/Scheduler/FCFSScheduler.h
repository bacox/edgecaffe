//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_FCFSSCHEDULER_H
#define EDGECAFFE_FCFSSCHEDULER_H

#include "Scheduler.h"
#include "../TaskPool/FCFSTaskPool.h"
namespace EdgeCaffe {
    class FCFSScheduler : public Scheduler
    {
        FCFSTaskPool taskPool;
    public:


        void addTask(Task *t_ptr) override;

        bool getNext(Task **t_ptr) override;

        bool isEmpty() override;


        FCFSScheduler(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);
    };
}


#endif //EDGECAFFE_FCFSSCHEDULER_H
