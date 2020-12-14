//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_BULKORCHESTRATOR_H
#define EDGECAFFE_BULKORCHESTRATOR_H

#include "Orchestrator.h"

namespace EdgeCaffe {
    class BulkOrchestrator : public Orchestrator
    {
    protected:
        void setup() override;

        void checkBagOfTasks() override;
    public:
        static std::shared_ptr<BulkOrchestrator> Create() {
            auto ptr = std::make_shared<BulkOrchestrator>(BulkOrchestrator());
            ptr->setup();
            return ptr;
        }
    };


}


#endif //EDGECAFFE_BULKORCHESTRATOR_H
