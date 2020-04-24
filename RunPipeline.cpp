//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <EdgeCaffe.h>
#include <chrono>

void printUsage()
{
    std::cout << "RunPipeline [options] <mode> [outputfile]" << std::endl;
    std::cout << "Allowed arguments" << std::endl;
    std::cout << "\t--help\t\t : Print usage" << std::endl;
    std::cout << "\tmode\t\t : bulk|deepeye|partial" << std::endl;
    std::cout << "\toutputfile\t\t : output.csv" << std::endl;
}

int main(int argc, char *argv[])
{

    // Parse input
    // CMD <Mode>
    if (argc < 2)
    {
        printUsage();
        return 0;
    }

    std::string arg1 = argv[1];
    if (arg1 == "--help")
    {
        printUsage();
        return 0;
    }

    EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::PARTIAL;
    std::string modeAsString = "partial";
    if (arg1 == "bulk")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::BULK;
        modeAsString = "bulk";
    } else if (arg1 == "deepeye")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::DEEPEYE;
        modeAsString = "deepeye";
    } else if (arg1 == "linear")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::LINEAR;
        modeAsString = "linear";
    }

    std::string outputFile = "output.csv";
    std::string outputLocation = "../analysis/";
    if (argc >= 3)
    {
        outputFile = argv[2];
    }

    int repetitions = 1;

    std::cout << "=========================" << std::endl;
    std::cout << "Running\t'RunPipeline'" << std::endl;
    std::cout << "Mode: \t " << modeAsString << std::endl;


    ::google::InitGoogleLogging(argv[0]);

    EdgeCaffe::Orchestrator orchestrator;
    auto startTime = std::chrono::high_resolution_clock::now();

    if (mode == EdgeCaffe::Orchestrator::BULK)
    {
        orchestrator.setupBulkMode();
    } else if (mode == EdgeCaffe::Orchestrator::DEEPEYE)
    {
        orchestrator.setupDeepEyeMode();
    } else if (mode == EdgeCaffe::Orchestrator::LINEAR)
    {
        orchestrator.setupLinearMode();
    } else
    { // Partial
        orchestrator.setupPartialMode(2);
    }
    orchestrator.splitMode = mode;
    orchestrator.splitModeAsString = modeAsString;

    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToGenderNet = "networks/GenderNet";
    std::string pathToFaceNet = "networks/FaceNet";
    std::string pathToSoS_Alex = "networks/SoS";
    std::string pathToSoS_Google = "networks/SoS_GoogleNet";

    std::string pathToGenNet = "networks/GeneratedNetwork";
    std::cout << "Starting submitting tasks" << std::endl;
    auto layerList = caffe::LayerRegistry<float>::LayerTypeList();
    for (int ii = 0; ii < repetitions; ++ii)
    {
        std::string pathToImg = "../resources/test_1.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
        EdgeCaffe::Arrival a1{pathToGenNet, ""};
        orchestrator.submitInferenceTask(a1);

        pathToImg = "../resources/test_2.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
        EdgeCaffe::Arrival a2{pathToGenNet, ""};
        orchestrator.submitInferenceTask(a2);

        pathToImg = "../resources/test_3.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
        EdgeCaffe::Arrival a3{pathToGenNet, ""};
        orchestrator.submitInferenceTask(a3);
    }

    if (mode == EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::LINEAR)
    {
        EdgeCaffe::Task *last = nullptr;
        for (auto iTask : orchestrator.inferenceTasks)
        {
            if (last != nullptr)
            {
                iTask->net->subTasks.front()->firstTask->addTaskDependency(last);
            }
            last = iTask->net->subTasks.front()->lastTask;
        }

    }

    orchestrator.start();
    orchestrator.processTasks();
    orchestrator.waitForStop();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << duration << " milliseconds" << std::endl;

    std::ofstream fout(outputLocation + outputFile, std::ios::out);
    std::string csvHeaders = "networkName,layerId,layerName,Loading_ns,execution_ns,policy";
    fout << csvHeaders << std::endl;
    for (auto inferenceTasks : orchestrator.inferenceTasks)
    {
        std::vector<std::string> lines = inferenceTasks->output.toCsvLines();
        for (std::string line : lines)
        {
            fout << line << std::endl;
        }
    }
    fout.close();

    return 0;
}
