//
// Created by bacox on 01/08/2020.
//

#ifndef EDGECAFFE_LINEARORCHESTRATOR_H
#define EDGECAFFE_LINEARORCHESTRATOR_H

#include "Orchestrator.h"

namespace EdgeCaffe {
    class LinearOrchestrator : public Orchestrator
    {
    protected:
        void setup() override;

        void checkBagOfTasks() override;
    public:
        static std::shared_ptr<LinearOrchestrator> Create() {
            auto ptr = std::make_shared<LinearOrchestrator>(LinearOrchestrator());
            ptr->setup();
            return ptr;
        }
    };
}


#endif //EDGECAFFE_LINEARORCHESTRATOR_H
