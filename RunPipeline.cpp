//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <Orchestrator.h>
#include <chrono>

void printUsage(){
    std::cout << "RunPipeline [options] <mode>" << std::endl;
    std::cout << "Allowed arguments" << std::endl;
    std::cout << "\t--help\t\t : Print usage" << std::endl;
    std::cout << "\tmode\t\t : bulk|deepeye|partial" << std::endl;
}

int main(int argc, char *argv[]) {

    // Parse input
    // CMD <Mode>
    if(argc < 2) {
        printUsage();
        return 0;
    }

    std::string arg1 = argv[1];
    if(arg1 == "--help") {
        printUsage();
        return 0;
    }

    Orchestrator::MODEL_SPLIT_MODE mode = Orchestrator::MODEL_SPLIT_MODE::PARTIAL;
    std::string modeAsString = "partial";
    if(arg1 == "bulk") {
        mode = Orchestrator::MODEL_SPLIT_MODE::BULK;
        modeAsString = "bulk";
    } else if (arg1 == "deepeye") {
        mode = Orchestrator::MODEL_SPLIT_MODE::DEEPEYE;
        modeAsString = "deepeye";
    } else if(arg1 == "linear") {
        mode = Orchestrator::MODEL_SPLIT_MODE::LINEAR;
        modeAsString = "linear";
    }

    std::cout << "Running\t'RunPipeline'" << std::endl;
    std::cout << "Mode: \t " << modeAsString << std::endl;


    ::google::InitGoogleLogging(argv[0]);
//    std::cout << "Scheduled Pipeline" << std::endl;

    Orchestrator orchestrator;
    auto startTime = std::chrono::high_resolution_clock::now();

    if(mode == Orchestrator::BULK) {
        orchestrator.setupBulkMode();
    } else if (mode == Orchestrator::DEEPEYE) {
        orchestrator.setupDeepEyeMode();
    } else if (mode == Orchestrator::LINEAR){
        orchestrator.setupLinearMode();
    } else { // Partial
        orchestrator.setupPartialMode(2);
    }
    orchestrator.splitMode = mode;

    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToGenderNet = "networks/GenderNet";
    std::string pathToFaceNet = "networks/FaceNet";

    for(int ii = 0; ii < 1; ++ii) {
        std::string pathToImg = "../resources/test_1.jpg";
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);

        pathToImg = "../resources/test_2.jpg";
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
//
        pathToImg = "../resources/test_3.jpg";
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
    }
//    std::string pathToImg = "../resources/test_1.jpg";
//
//    orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
//    orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);

    if(mode == Orchestrator::MODEL_SPLIT_MODE::LINEAR) {
        Task* last = nullptr;
        for(auto iTask : orchestrator.inferenceTasks)
        {
            if(last != nullptr)
            {
                iTask->net->subTasks.front()->firstTask->addTaskDependency(last);
            }
            last = iTask->net->subTasks.front()->lastTask;
        }

    }



    orchestrator.start();
    orchestrator.processTasks();
    orchestrator.waitForStop();
//
    auto endTime = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( endTime - startTime).count();
//
    std::cout << duration << " milliseconds" << std::endl;
////    for( auto inferenceTask : orchestrator.inferenceTasks)
////    {
////        inferenceTask->net->showResult();
////    }


    return 0;
}