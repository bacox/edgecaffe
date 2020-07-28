//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_MASAORCHESTRATOR_H
#define EDGECAFFE_MASAORCHESTRATOR_H

#include "Orchestrator.h"

namespace EdgeCaffe
{
    class MasaOrchestrator : public Orchestrator
    {
    protected:
        void setup() override;

        void checkBagOfTasks() override;
    public:
        static std::shared_ptr<MasaOrchestrator> Create() {
            auto ptr = std::make_shared<MasaOrchestrator>(MasaOrchestrator());
            ptr->setup();
            return ptr;
        }
    };
}


#endif //EDGECAFFE_MASAORCHESTRATOR_H
