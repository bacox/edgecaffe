//
// Created by bacox on 08/05/2020.
//

#include <opencv2/opencv.hpp>
#include "Tasks/InitNetworkTask.h"

//EdgeCaffe::InitNetworkTask::InitNetworkTask(int id, int executionTime) : Task(id, executionTime)
//{}
//
//EdgeCaffe::InitNetworkTask::InitNetworkTask()
//{}

void EdgeCaffe::InitNetworkTask::run()
{
    inet->networkProfile.measure(NetworkProfile::START);

    cv::Mat input_img = cv::imread(pathToInput);
    inet->setInput(input_img, use_scales);
    inet->loadNetworkStructure();
    inet->loadInputToNetwork();
}

std::string EdgeCaffe::InitNetworkTask::getTaskDescription()
{
    return "InitNetwork Task ";
}

EdgeCaffe::InitNetworkTask::InitNetworkTask(
        int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
) : Task(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory)
{}
