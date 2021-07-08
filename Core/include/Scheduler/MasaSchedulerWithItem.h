//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_MASASCHEDULERWITHITEM_H
#define EDGECAFFE_MASASCHEDULERWITHITEM_H

#include <Tasks/Task.h>
#include <TaskPool/TypePriorityTaskPool.h>
#include <TaskPool/SNFTypePriorityTaskPool.h>
#include "Scheduler.h"

namespace EdgeCaffe {
    class MasaSchedulerWithItem : public Scheduler
    {
        SNFTypePriorityTaskPool execTasks;
        SNFTypePriorityTaskPool loadTasks;
    public:
        void addTask(std::shared_ptr<Task> t_ptr) override;


        bool isEmpty() override;


        bool getNext(std::shared_ptr<Task> *t_ptr) override;

        MasaSchedulerWithItem(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);
    };
}


#endif //EDGECAFFE_MASASCHEDULERWITHITEM_H
