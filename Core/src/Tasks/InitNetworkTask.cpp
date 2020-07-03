//
// Created by bacox on 08/05/2020.
//

#include <opencv2/opencv.hpp>
#include "Tasks/InitNetworkTask.h"

void EdgeCaffe::InitNetworkTask::run()
{
    // Subtract memory usage from free memory space
    inet->mc->lockMemory(inet->maxMemoryUsage);
    inet->networkProfile.measure(NetworkProfile::START);
    std::cout << "Loading input from " << pathToInput << std::endl;
    cv::Mat input_img = cv::imread(pathToInput);
    inet->setInput(input_img, use_scales);
    inet->loadNetworkStructure();
    inet->loadInputToNetwork();
}

std::string EdgeCaffe::InitNetworkTask::getTaskDescription()
{
    return "InitNetwork Task " + std::to_string(layerId);
}

EdgeCaffe::InitNetworkTask::InitNetworkTask(
        int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
) : Task(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory)
{}
