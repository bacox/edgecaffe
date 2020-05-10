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
        InitNetworkTask(int id, int executionTime);

        InitNetworkTask();

        std::string pathToInput;
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
