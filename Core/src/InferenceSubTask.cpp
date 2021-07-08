//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceSubTask.h"

namespace EdgeCaffe
{

    InferenceSubTask::~InferenceSubTask()
    {
//      std::cout << "[DEALLOC] >>> Removing inference InferenceSubTask" << std::endl;
        if (net_ptr != nullptr)
            delete net_ptr;
    }
}
