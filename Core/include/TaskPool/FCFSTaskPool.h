//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_FCFSTASKPOOL_H
#define EDGECAFFE_FCFSTASKPOOL_H

#include "AbstractTaskPool.h"

namespace EdgeCaffe {
    class FCFSTaskPool : public AbstractTaskPool
    {
//    public:
//        FCFSTaskPool(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);

    protected:
        std::map<int, Task*> pool;
    public:
        void addTask(Task *t_ptr) override;

        bool isEmpty() override;

        bool getNext(Task **task) override;

        bool hasTask(int taskId) override;
    };
}


#endif //EDGECAFFE_FCFSTASKPOOL_H
