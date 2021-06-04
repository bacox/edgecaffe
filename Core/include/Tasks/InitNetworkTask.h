//
// Created by bacox on 08/05/2020.
//

#ifndef EDGECAFFE_INITNETWORKTASK_H
#define EDGECAFFE_INITNETWORKTASK_H


#include <InferenceNetwork.h>
#include "Task.h"

namespace EdgeCaffe
{
    class InitNetworkTask : public Task
    {
    public:

        InitNetworkTask(
                int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0
                , int estimatedNeededMemory = 0
        );

        std::vector<std::string> inputPaths;
        std::string pathToDescription;

        // Quick and dirty way to test
        // Hold reference to InferenceNetwork
        InferenceNetwork *inet = nullptr;
        bool use_scales = false;
    protected:
        void run() override;

    public:
        std::string getTaskDescription() override;
    };
}


#endif //EDGECAFFE_INITNETWORKTASK_H
