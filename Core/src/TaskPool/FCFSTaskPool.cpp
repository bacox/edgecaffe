//
// Created by bacox on 27/07/2020.
//

#include "../../include/TaskPool/FCFSTaskPool.h"

void EdgeCaffe::FCFSTaskPool::addTask(EdgeCaffe::Task *t_ptr) {
    this->pool[t_ptr->id] = t_ptr;
}

//EdgeCaffe::FCFSTaskPool::FCFSTaskPool(
//        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
//) : AbstractTaskPool(mc, nr)
//{}

