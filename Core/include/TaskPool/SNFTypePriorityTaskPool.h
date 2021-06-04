//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H
#define EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H

#include "AbstractTaskPool.h"

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

    class SNFTypePriorityTaskPool : public AbstractTaskPool<SNFPriorityKey> {
    public:
        void addTask(Task *t_ptr) override;
    };
}
#endif //EDGECAFFE_SNFTYPEPRIORITYTASKPOOL_H
