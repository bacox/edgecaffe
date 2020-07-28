//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_MASASCHEDULER_H
#define EDGECAFFE_MASASCHEDULER_H

#include <Tasks/Task.h>
#include <TaskPool/TypePriorityTaskPool.h>
#include "Scheduler.h"

namespace EdgeCaffe {
    class MasaScheduler : public Scheduler
    {
        TypePriorityTaskPool execTasks;
        TypePriorityTaskPool loadTasks;
    public:
        void addTask(Task *t_ptr) override;

        bool isEmpty() override;


        bool getNext(Task **t_ptr) override;

        MasaScheduler(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);
    };
}


#endif //EDGECAFFE_MASASCHEDULER_H
