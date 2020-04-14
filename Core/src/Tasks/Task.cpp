//
// Created by bacox on 30/03/2020.
//

#include "../../include/Tasks/Task.h"
namespace EdgeCaffe {

    bool Task::waitsForOtherTasks() {
        bool isWaiting = false;
        for (Task *t: dependsOn) {
            if (t == nullptr)
                continue;
            if (!t->executed)
                isWaiting = true;
        }
        return isWaiting;
    }

    std::vector<Task *> Task::getDependencies() {
        return dependsOn;
    }

    void Task::addTaskDependency(Task *t) {
        dependsOn.push_back(t);
    }

    Task::Task(int id, int executionTime) : id(id), executionTime(executionTime) {}

    Task::Task() : executionTime(0) {}

    bool Task::hasPoolAssigned() {
        return assignedPoolId > -1;
    }

    int Task::getAssignedPoolId() {
        return assignedPoolId;
    }

    std::string Task::getTaskDescription() {
        return "General Task";
    }

    Task::~Task() {
//    std::cout << "Dealloc Task" << std::endl;
//    if(network_ptr != nullptr)
//        delete network_ptr;
    }

    void Task::execute() {
        profileLine.start();
        run();
        profileLine.stop();
    }
}
