//
// Created by bacox on 13/08/2020.
//

#ifndef EDGECAFFE_MULTIBULKORCHESTRATOR_H
#define EDGECAFFE_MULTIBULKORCHESTRATOR_H

#include "Orchestrator.h"

namespace EdgeCaffe {
    class MultiBulkOrchestrator : public Orchestrator
    {
    protected:
        void setup() override;

        void checkBagOfTasks() override;
    public:
        static std::shared_ptr<MultiBulkOrchestrator> Create() {
            auto ptr = std::make_shared<MultiBulkOrchestrator>(MultiBulkOrchestrator());
            ptr->setup();
            return ptr;
        }
    };
};


#endif //EDGECAFFE_MULTIBULKORCHESTRATOR_H
