//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_ABSTRACTPRIORITYTASKPOOL_H
#define EDGECAFFE_ABSTRACTPRIORITYTASKPOOL_H

#include "AbstractTaskPool.h"

namespace EdgeCaffe {

    struct PriorityKey {
        Task::TYPE type;
        int id;

//        bool operator()(const PriorityKey& a, const PriorityKey& b) const {
//            if(a.type != b.type)
//                return a.type < b.type;
//            return a.id < b.id;
//        }

        bool operator<(const PriorityKey& b) const {
            if(type != b.type)
                return type < b.type;
            return id < b.id;
        }
    };

    class AbstractPriorityTaskPool : public AbstractTaskPool<PriorityKey> {
    public:
        void addTask(Task *t_ptr) override;

    };
}
#endif //EDGECAFFE_ABSTRACTPRIORITYTASKPOOL_H
