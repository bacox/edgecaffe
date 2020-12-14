//
// Created by bacox on 05/08/2020.
//

#ifndef EDGECAFFE_EXECBULKTASK_H
#define EDGECAFFE_EXECBULKTASK_H


#include "ExecTask.h"

namespace EdgeCaffe
{
    class ExecBulkTask : public ExecTask
    {

    public:
        void run() override;

        std::string getTaskDescription() override;

        ExecBulkTask(
                int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0, int estimatedNeededMemory =0
        );
    };
}


#endif //EDGECAFFE_EXECBULKTASK_H
