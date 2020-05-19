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


    /**
     * Read the policy from the command line argument
     */
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

    // Read the name of the output file if provided as a command line argument
    std::string outputFile = "output.csv";
    if (argc >= 3)
    {
        outputFile = argv[2];
    }


    // Read config
    std::string pathToConfig = "config/pipeline-template.yaml";
    YAML::Node config;
    try
    {
        config = YAML::LoadFile(pathToConfig);
    } catch (...)
    {
        std::cerr << "Error while attempting to read yaml file!" << std::endl;
        std::cerr << "Yaml file: " << pathToConfig << std::endl;
    }

    // Set the paths provided in the config file.
    // Note that the default value is provided if the config file doesn't provide a value
    std::string networkPath = config["pathToNetworks"].as<std::string>("networks");
    std::string resourcePath = config["pathToResources"].as<std::string>("../resources");
    std::string outputPath = config["pathToAnalysis"].as<std::string>("../analysis");
    std::string generalOutputFile = config["general-outputfile"].as<std::string>("./generalOutput.csv");
    std::string mem_limit = config["mem_limit"].as<std::string>("1GB");


    if (argc >= 4)
    {
        mem_limit = argv[3];
    }
    // Print the settings for this run to the screen
    std::cout << "=========================" << std::endl;
    std::cout << "Running\t\t\t'RunPipeline'" << std::endl;
    std::cout << "Mode: \t\t\t " << modeAsString << std::endl;
    std::cout << "networkPath: \t " << networkPath << std::endl;
    std::cout << "resourcePath: \t " << resourcePath << std::endl;
    std::cout << "outputPath: \t " << outputPath << std::endl;
    std::cout << "outputFile: \t " << outputFile << std::endl;
    std::cout << "Memory Limit: \t " << mem_limit << std::endl << std::endl;

    // Init glog to prevent everything outputting to stdout
    ::google::InitGoogleLogging(argv[0]);

    /**
     * Real EdgeCaffe code starts here
     */
    EdgeCaffe::Orchestrator orchestrator;
    // Get timestamp for end-to-end measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    orchestrator.setup(mode, modeAsString);

    // Set path to all networks
    std::string pathToAgeNet = networkPath + "/AgeNet";
    std::string pathToGenderNet = networkPath + "/GenderNet";
    std::string pathToFaceNet = networkPath + "/FaceNet";
    std::string pathToSoS_Alex = networkPath + "/SoS";
    std::string pathToSoS_Google = networkPath + "/SoS_GoogleNet";

    /**
     * Submitting inference tasks
     */
    std::cout << "Starting submitting tasks" << std::endl;

    std::string pathToImg = resourcePath + "/test_1.jpg";
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Alex, "SoS"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Google, "SoS_GoogleNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToAgeNet, "AgeNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToGenderNet, "GenderNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToFaceNet, "FaceNet"});

    pathToImg = resourcePath + "/test_2.jpg";
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Alex, "SoS"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Google, "SoS_GoogleNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToAgeNet, "AgeNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToGenderNet, "GenderNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToFaceNet, "FaceNet"});

    pathToImg = resourcePath + "/test_3.jpg";
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Alex, "SoS"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToSoS_Google, "SoS_GoogleNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToAgeNet, "AgeNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToGenderNet, "GenderNet"});
    orchestrator.submitInferenceTask(EdgeCaffe::Arrival{pathToImg, pathToFaceNet, "FaceNet"});

    // Start the worker
    orchestrator.start();
    // Process the tasks in relation to their dependencies
    orchestrator.processTasks();
    // Make sure to wait for the workers to stop before continuing
    orchestrator.waitForStop();

    // Calculate the duration of the end-to-end timing measurement
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    // Report measurement to screen
    std::cout << duration << " milliseconds" << std::endl;

    /**
     * Processing the gathered data
     * This part writes the timing information of the layers of the networks to a csv file.
     * It overwrites the specified file. It is important to use a unique filename
     *  for each run of move the data in between runs to prevent losing data.
     */

    std::string layerOutputFile = outputPath + "/" + outputFile;
    orchestrator.processLayerData(layerOutputFile);
    std::string queueEventsFile = outputPath + "/stepEvents7.csv";
    orchestrator.processEventData(queueEventsFile, startTime);
    std::string networkOutputFile = outputPath + "/networkStats6.csv";
    orchestrator.processNetworkData(networkOutputFile, startTime);

    /**
     * Save the output of the end-to-end measurement
     * Here we append the measurement to the file
     * If the file does not exist it is created.
     */
    EdgeCaffe::Output output;
    std::string generalLine = outputPath + "," + modeAsString + "," + std::to_string(duration);
    output.toCSVAppend(generalOutputFile, {generalLine}, EdgeCaffe::Output::PIPELINE);
    return 0;
}
