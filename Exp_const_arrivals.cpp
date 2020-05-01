//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <EdgeCaffe.h>
#include <chrono>

void printUsage()
{
    std::cout << "Exp_const_arrivals [options] <mode> [outputfile]" << std::endl;
    std::cout << "Allowed arguments" << std::endl;
    std::cout << "\t--help\t\t : Print usage" << std::endl;
    std::cout << "\tmode\t\t : bulk|deepeye|partial" << std::endl;
    std::cout << "\toutputfile\t\t : output.csv" << std::endl;
}

int main(int argc, char *argv[])
{
    /**
     * This example is almost the same as the RunPipeline example
     * The only difference is that the networks and input in RunPipeline are submitted right at the beginning while
     *  in this case the Arrivals are generated that submit a network and an input based in a arrival time.
     *
     *  This means we gradually submit new inference-tasks over time or according to a certain distribution.
     */

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

    /**
     * Instead of defining the networks and the input, we let it be generated by a (constant) distribution
     */
    EdgeCaffe::ArrivalList arrivals;
    arrivals.generateList(20);

    orchestrator.arrivals = arrivals;

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

    return 0;
}