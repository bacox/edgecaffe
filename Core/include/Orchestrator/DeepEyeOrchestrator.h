//
// Created by bacox on 28/07/2020.
//

#ifndef EDGECAFFE_DEEPEYEORCHESTRATOR_H
#define EDGECAFFE_DEEPEYEORCHESTRATOR_H

#include "Orchestrator.h"

namespace EdgeCaffe
{
    class DeepEyeOrchestrator : public Orchestrator
    {
    protected:
        void setup() override;

        void checkBagOfTasks() override;
    public:
        static std::shared_ptr<DeepEyeOrchestrator> Create() {
            auto ptr = std::make_shared<DeepEyeOrchestrator>(DeepEyeOrchestrator());
            ptr->setup();
            return ptr;
        }
    };
}


#endif //EDGECAFFE_DEEPEYEORCHESTRATOR_H
