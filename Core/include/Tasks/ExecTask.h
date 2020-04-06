//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_EXECTASK_H
#define PIPELINE_EXECTASK_H

#include "Task.h"

class ExecTask : public Task{
public:
    void run() override;

    ExecTask(int id, int executionTime) : Task(id, executionTime) {}

    ExecTask() {}
};


#endif //PIPELINE_EXECTASK_H
