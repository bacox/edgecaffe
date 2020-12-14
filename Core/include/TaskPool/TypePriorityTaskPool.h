//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_TYPEPRIORITYTASKPOOL_H
#define EDGECAFFE_TYPEPRIORITYTASKPOOL_H

#include "AbstractTaskPool.h"

namespace EdgeCaffe {

    struct PriorityKey {
        Task::TYPE type;
        int id;

        bool operator<(const PriorityKey& b) const {
            if(type != b.type)
                return type < b.type;
            return id < b.id;
        }
    };

    class TypePriorityTaskPool : public AbstractTaskPool {
    public:
        std::map<PriorityKey, Task*> pool;
        void addTask(Task *t_ptr) override;

        bool hasTask(int taskId) override;
//        AbstractTaskPool<PriorityKey> & get();
//        TypePriorityTaskPool(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);

        bool isEmpty() override;

        bool getNext(Task **task) override;
    };
}
#endif //EDGECAFFE_TYPEPRIORITYTASKPOOL_H
