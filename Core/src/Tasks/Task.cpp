//
// Created by bacox on 30/03/2020.
//

#include "../../include/Tasks/Task.h"

namespace EdgeCaffe
{

    bool Task::waitsForOtherTasks()
    {
        bool isWaiting = false;
        for(TaskDependency t: dependsOn)
        {
            if(!t.isExecuted())
                isWaiting = true;
        }

        // If true we are enforcing the linear behaviour
        if(*dependencyCondition)
        {
            for (ConditionalDependency t: dependsOnConditional)
            {
                if (!t.isExecuted())
                    isWaiting = true;
            }
        }


//        for (Task *t: dependsOn)
//        {
//            if (t == nullptr)
//                continue;
//            if (!t->executed)
//                isWaiting = true;
//        }
        return isWaiting;
    }

    std::vector<TaskDependency> Task::getDependencies() const
    {
        return dependsOn;
    }

    std::vector<ConditionalDependency> Task::getConditionalDependencies() const
    {
        return dependsOnConditional;
    }

    void Task::addTaskDependency(TaskDependency t)
    {
        dependsOn.emplace_back(t);
    }

    void Task::addTaskDependency(ConditionalDependency t)
    {
        dependsOnConditional.emplace_back(t);
    }

    Task::Task(int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory)
    : id(id)
    , networkId(networkId)
    , taskName(taskName)
    , estimatedExecutionTime(estimatedExecutionTime)
    , estimatedNeededMemory(estimatedNeededMemory)
    {
    }

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

    Task::~Task(){
//      std::cout << "[DEALLOC] >>> Deallocating Task: " << this->id << " <|> " << this->layerId << " <|> " << this->networkId << " <|> " << this->networkName << std::endl;
    }

    void Task::execute()
    {
//        std::cout << "Running task " << this->id << " <|> " << this->layerId << " <|> " << this->networkId << " <|> " << this->networkName << std::endl;
        // Start measuring time
        profileLine.start();
        measureTime(TIME::START);
        run();
        // End measuring time
        measureTime(TIME::STOP);
        profileLine.stop();
        if(saveResults)
        {
            saveNetworkResults();
        }
    }

    void Task::measureTime(Task::TIME type)
    {
        std::chrono::time_point<std::chrono::system_clock> *tp_ptr;
        switch (type)
        {
//            case TIME::NET_SUBMIT:
//                tp_ptr = &networkSubmission;
//                break;
            case TIME::TO_READY:
                tp_ptr = &moveToReady;
                break;
            case TIME::TO_WAITING:
                tp_ptr = &moveToWaiting;
                break;
            case TIME::START:
                tp_ptr = &startTask;
                break;
            case TIME::STOP:
                tp_ptr = &stopTask;
                break;
            case TIME::FINISHED:
                tp_ptr = &networkFinished;
                break;
        }

        (*tp_ptr) = std::chrono::high_resolution_clock::now();
    }

    void Task::saveNetworkResults() {
        if(itask != nullptr)
        {
            itask->saveNetworkResult();
        }
    }

    void Task::invalidate() {
        invalid = true;
    }

    bool Task::isValid() {
        return !invalid;
    }
}
