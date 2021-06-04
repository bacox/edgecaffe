//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_FCFSTASKPOOL_H
#define EDGECAFFE_FCFSTASKPOOL_H

#include "AbstractTaskPool.h"

namespace EdgeCaffe {
    class FCFSTaskPool : public AbstractTaskPool<int>
    {

    public:
        void addTask(Task *t_ptr) override;

    };
}


#endif //EDGECAFFE_FCFSTASKPOOL_H
