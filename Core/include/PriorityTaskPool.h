//
// Created by bacox on 13/06/2020.
//

#ifndef EDGECAFFE_PRIORITYTASKPOOL_H
#define EDGECAFFE_PRIORITYTASKPOOL_H

#include "TaskPool.h"

namespace EdgeCaffe {
    class PriorityTaskPool : public TaskPool
    {
    public:
        PriorityTaskPool();

        bool hasTask(int taskId) override;

        void addTask(Task *t_ptr) override;

        bool isEmpty() override;

        bool getNext(Task **task) override;

    private:
        std::map<int, Task*> loadTasks;
        std::map<int, Task*> execTasks;


    };
}


#endif //EDGECAFFE_PRIORITYTASKPOOL_H
