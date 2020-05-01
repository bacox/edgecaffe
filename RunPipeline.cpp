//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <EdgeCaffe.h>
#include <chrono>
#include <filesystem>

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
    if (argc >= 3)
    {
        outputFile = argv[2];
    }

    int repetitions = 1;

    // Read config
    std::string pathToConfig = "config/pipeline-template.yaml";
    YAML::Node config;
    try{
        config = YAML::LoadFile(pathToConfig);
    } catch(...){
        std::cerr << "Error while attempting to read yaml file!" << std::endl;
        std::cerr << "Yaml file: " << pathToConfig << std::endl;
    }

    std::string networkPath = config["pathToNetworks"].as<std::string>("networks");
    std::string resourcePath = config["pathToResources"].as<std::string>("../resources");
    std::string outputPath = config["pathToAnalysis"].as<std::string>("../analysis");
    std::string generalOutputFile = config["general-outputfile"].as<std::string>("./generalOutput.csv");
    std::string mem_limit = config["mem_limit"].as<std::string>("1GB");

    std::cout << "=========================" << std::endl;
    std::cout << "Running\t\t\t'RunPipeline'" << std::endl;
    std::cout << "Mode: \t\t\t " << modeAsString << std::endl;
    std::cout << "networkPath: \t " << networkPath << std::endl;
    std::cout << "resourcePath: \t " << resourcePath << std::endl;
    std::cout << "outputPath: \t " << outputPath << std::endl;
    std::cout << "outputFile: \t " << outputFile << std::endl << std::endl;


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

    std::string pathToAgeNet = networkPath + "/AgeNet";
    std::string pathToGenderNet = networkPath + "/GenderNet";
    std::string pathToFaceNet = networkPath + "/FaceNet";
    std::string pathToSoS_Alex = networkPath + "/SoS";
    std::string pathToSoS_Google = networkPath + "/SoS_GoogleNet";

    std::cout << "Starting submitting tasks" << std::endl;
    for (int ii = 0; ii < repetitions; ++ii)
    {
        std::string pathToImg = resourcePath + "/test_1.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);

        pathToImg = resourcePath + "/test_2.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);

        pathToImg = resourcePath + "/test_3.jpg";
        orchestrator.submitInferenceTask(pathToSoS_Alex, pathToImg);
        orchestrator.submitInferenceTask(pathToSoS_Google, pathToImg);
        orchestrator.submitInferenceTask(pathToAgeNet, pathToImg);
        orchestrator.submitInferenceTask(pathToGenderNet, pathToImg);
        orchestrator.submitInferenceTask(pathToFaceNet, pathToImg, true);
    }

    orchestrator.start();
    orchestrator.processTasks();
    orchestrator.waitForStop();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << duration << " milliseconds" << std::endl;

    std::ofstream fout(outputPath + "/" + outputFile, std::ios::out);
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

    // Check if general outputfile exists
    if(!std::filesystem::exists(generalOutputFile))
    {
        // Create file with headers
        std::ofstream foutGeneral(generalOutputFile, std::ios::out);
        std::string csvHeaders = "mem_limit,policy,time";
        foutGeneral << csvHeaders << std::endl;
        foutGeneral.close();
    }

    std::string generalLine = mem_limit + "," + modeAsString + "," + std::to_string(duration);
    std::ofstream foutGeneral(generalOutputFile, std::ios_base::app);
    foutGeneral << generalLine << std::endl;
    foutGeneral.close();

    return 0;
}
