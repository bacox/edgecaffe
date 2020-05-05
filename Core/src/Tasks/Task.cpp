//
// Created by bacox on 30/03/2020.
//

#include "../../include/Tasks/Task.h"

namespace EdgeCaffe
{

    bool Task::waitsForOtherTasks()
    {
        bool isWaiting = false;
        for (Task *t: dependsOn)
        {
            if (t == nullptr)
                continue;
            if (!t->executed)
                isWaiting = true;
        }
        return isWaiting;
    }

    std::vector<Task *> Task::getDependencies()
    {
        return dependsOn;
    }

    void Task::addTaskDependency(Task *t)
    {
        dependsOn.push_back(t);
    }

    Task::Task(int id, int executionTime) : id(id), estimatedExecutionTime(executionTime)
    {}

    Task::Task() : estimatedExecutionTime(0)
    {}

    bool Task::hasPoolAssigned()
    {
        return assignedPoolId > -1;
    }

    int Task::getAssignedPoolId()
    {
        return assignedPoolId;
    }

    std::string Task::getTaskDescription()
    {
        return "General Task";
    }

    Task::~Task(){}

    void Task::execute()
    {
        // Start measuring time
        profileLine.start();
        run();
        // End measuring time
        profileLine.stop();
    }
}
