//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceSubTask.h"

namespace EdgeCaffe
{

    InferenceSubTask::~InferenceSubTask()
    {
//        std::cout << "Dealloc InferenceSubTask" << std::endl;
        if (net_ptr != nullptr)
            delete net_ptr;

//    for(auto task : tasks)
//        if(task != nullptr)
//            delete task;
    }
}
