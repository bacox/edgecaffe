//
// Created by bacox on 30/03/2020.
//

#include <thread>
#include <chrono>
#include "../../include/Tasks/ExecTask.h"

namespace EdgeCaffe
{

    void ExecTask::run()
    {
//    std::cout << "Running the Exec task" << std::endl;

        // Exec the partial layer implementation
//    std::cout << "Running the " << layerId << "th layer! |-> " << network_ptr->layer_names()[layerId] << std::endl;
        network_ptr->layers()[layerId]->Forward(network_ptr->bottom_vecs()[layerId], network_ptr->top_vecs()[layerId]);

        // Unload the partial layer
//    std::cout << "Unloading layer " << layerId << std::endl;
        network_ptr->layers_unsafe()[layerId].reset();

        std::this_thread::sleep_for(std::chrono::milliseconds(executionTime));
    }

    std::string ExecTask::getTaskDescription()
    {
        return "Exec Task layer " + std::to_string(layerId);
    }
}