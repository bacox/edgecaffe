//
// Created by bacox on 08/05/2020.
//

#include <opencv2/opencv.hpp>
#include "Tasks/InitNetworkTask.h"

void EdgeCaffe::InitNetworkTask::run()
{
    // Subtract memory usage from free memory space
//    inet->mc->lockMemory(inet->maxMemoryUsage);
    inet->networkProfile.measure(NetworkProfile::START);
    std::vector<cv::Mat> images;
//    std::cout << "Loading input from " << inputPaths << std::endl;
    for (std::string & path : inputPaths) {
        std::cout << "Loading image from: " << path << std::endl;
        images.push_back(cv::imread(path));
    }

    inet->setInput(images, use_scales);
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
