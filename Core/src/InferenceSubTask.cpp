//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceSubTask.h"

namespace EdgeCaffe
{

    InferenceSubTask::~InferenceSubTask()
    {
        if (net_ptr != nullptr)
            delete net_ptr;
    }
}
