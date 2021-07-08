//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H
#define EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H

#include "AbstractTaskPool.h"
#include <Tasks/Task.h>
namespace EdgeCaffe {
    struct SNFPriorityKey {
        double networkExecutionTime;
        Task::TYPE type;
        int id;

        bool operator<(const SNFPriorityKey& b) const {
            if(networkExecutionTime != b.networkExecutionTime)
                return networkExecutionTime < b.networkExecutionTime;
            if(type != b.type)
                return type < b.type;
            return id < b.id;
        }
    };

    class SNFTypePriorityTaskPool : public AbstractTaskPool {
    public:
        std::map<SNFPriorityKey, std::shared_ptr<Task>> pool;
        void addTask(std::shared_ptr<Task> t_ptr) override;

        bool hasTask(int taskId) override;
//        AbstractTaskPool<PriorityKey> & get();
//        TypePriorityTaskPool(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);

        bool isEmpty() override;

        bool getNext(std::shared_ptr<Task> *task) override;
    };
}
#endif //EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H
