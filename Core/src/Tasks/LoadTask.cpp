//
// Created by bacox on 30/03/2020.
//

#include <thread>
#include <chrono>
#include "../../include/Tasks/LoadTask.h"
void LoadTask::run() {
    if(needsLoading)
    {
//        std::cout << "Running the loading task" << std::endl;
//
//        std::cout << "Loading the " << layerId << "th layer! |-> " << network_ptr->layer_names()[layerId] << std::endl;
//        std::cout << "Loading file  " << pathToPartial << std::endl;
        network_ptr->CopyTrainedLayersFrom(pathToPartial); // layer 12

        std::this_thread::sleep_for(std::chrono::milliseconds(executionTime));
    } else {
        std::cout << "Running the loading task (Dummy not actual loading)" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(executionTime));
    }
}

std::string LoadTask::getTaskDescription() {
    return "Load Task layer " + std::to_string(layerId);
}
