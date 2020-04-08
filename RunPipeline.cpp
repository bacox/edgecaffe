//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <Orchestrator.h>
#include <chrono>

int main(int argc, char *argv[]) {
    ::google::InitGoogleLogging(argv[0]);
    std::cout << "Scheduled Pipeline" << std::endl;

    Orchestrator orchestrator;
    auto startTime = std::chrono::high_resolution_clock::now();
    orchestrator.setupBulkMode();

    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToImg = "../resources/test_1.jpg";
    orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);

//    pathToImg = "../resources/test_2.jpg";
//    orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
//
//    pathToImg = "../resources/test_3.jpg";
//    orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);


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