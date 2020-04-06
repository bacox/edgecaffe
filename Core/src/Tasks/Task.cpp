//
// Created by bacox on 30/03/2020.
//

#include "../../include/Tasks/Task.h"

bool Task::waitsForOtherTasks() {
    bool isWaiting = false;
    for(Task* t: dependsOn) {
        if(t == nullptr)
            continue;
        if(!t->executed)
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