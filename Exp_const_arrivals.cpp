//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <EdgeCaffe.h>
#include <chrono>
#include <Util/Output.h>
#include <cxxopts.h>

template<typename T>
T getArgs(const cxxopts::ParseResult &result, std::string key, T defaultValue)
{
    if(result.count(key))
    {
        return result[key].as<T>();
    }
    return defaultValue;
}

void printConfig();


int main(int argc, char *argv[])
{
    /**
     * This example is almost the same as the RunPipeline example
     * The only difference is that the networks and input in RunPipeline are submitted right at the beginning while
     *  in this case the Arrivals are generated that submit a network and an input based in a arrival time.
     *
     *  This means we gradually submit new inference-tasks over time or according to a certain distribution.
     */

    /**
     * Start parsing input parameters
     */
    std::string helpMessage = "\n\nExp_const_arrivals executes DNN's using the EdgeCaffe framework based on different arrivals."
                              "\nArrivals arrive at the system based on the generated inter-arrival time."
                              "\nNote: some cli options are not yet implemented";

    cxxopts::Options options("Exp_const_arrivals", "One line description of MyProgram" + helpMessage);
    options.add_options()
            ("m,mode", "Mode to split and run the networks. Values [partial|linear|deepeye|bulk]", cxxopts::value<std::string>())
            ("mem_limit", "The memory limit given by the OS to EdgeCaffe. NOTE: this does not limit the memory for this process but is used in output generation and in scheduling.", cxxopts::value<std::string>())
            ("seed", "Seed for random number generator", cxxopts::value<long>())
            ("V,verbose", "Verbose")
            ("N,num-arrivals", "Number of arrivals to be generated", cxxopts::value<int>())
            ("a,arrival-list", "NOT_YET_IMPLEMENTED. Use this arrival list to inject arrivals instead of the generated one", cxxopts::value<std::string>())
            ("p,output-prefix", "Prefix for all output files to make it them unique for a certain run", cxxopts::value<std::string>())
            ("output-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("network-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("resources-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("s, sched-alg", "The scheduling algorithm to be used: [FCFS|SJF]", cxxopts::value<std::string>())
            ("c,read-config", "NOT_YET_IMPLEMENTED. Use a yaml config file to configure this run instead of the cli. This will overrule all other arguments", cxxopts::value<std::string>())
            ("h,help", "Print help message")
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);

    // Check if the help message needs to be printed;
    if (result.count("help") or result.arguments().size() == 0)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }
    std::string defaultMode = getArgs<std::string>(result, "mode", "partial");
    long seed = getArgs<long>(result, "seed", -1);
    std::string memLimit = getArgs<std::string>(result, "mem_limit", "1GB");
    bool verbose = getArgs<bool>(result, "verbose", false);
    std::string schedAlg = getArgs<std::string>(result, "sched-alg", "FCFS");
    int numArrivals = getArgs<int>(result, "num-arrivals", 10);
    std::string outputPrefix = getArgs<std::string>(result, "output-prefix", "");
    std::string pathToOutput = getArgs<std::string>(result, "output-path", "../analysis");
    std::string pathToNetworks = getArgs<std::string>(result, "network-path", "networks");
    std::string pathToResources = getArgs<std::string>(result, "resources-path", "../resources");
    /**
     * End parsing input parameters
     */

    EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::PARTIAL;
    std::string modeAsString = "partial";
    if (defaultMode == "bulk")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::BULK;
        modeAsString = "bulk";
    } else if (defaultMode == "deepeye")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::DEEPEYE;
        modeAsString = "deepeye";
    } else if (defaultMode == "linear")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::LINEAR;
        modeAsString = "linear";
    }

    std::string outputFile = "output.csv";
    if (argc >= 3)
    {
        outputFile = argv[2];
    }
    std::string generalOutputFile = "./generalOutput2.csv";

    // Read config
//    std::string pathToConfig = "config/pipeline-template.yaml";
//    YAML::Node config;
//    try{
//        config = YAML::LoadFile(pathToConfig);
//    } catch(...){
//        std::cerr << "Error while attempting to read yaml file!" << std::endl;
//        std::cerr << "Yaml file: " << pathToConfig << std::endl;
//    }
//
//    std::string networkPath = config["pathToNetworks"].as<std::string>("networks");
//    std::string resourcePath = config["pathToResources"].as<std::string>("../resources");
//    std::string outputPath = config["pathToAnalysis"].as<std::string>("../analysis");
//    std::string generalOutputFile = config["general-outputfile"].as<std::string>("./generalOutput2.csv");
//    std::string mem_limit = config["mem_limit"].as<std::string>("1GB");


    std::cout << "=========================" << std::endl;
    std::cout << "Running\t\t\t'Exp_const_arrivals'" << std::endl;
    std::cout << "Mode: \t\t\t " << modeAsString << std::endl;
    std::cout << "networkPath: \t " << pathToNetworks << std::endl;
    std::cout << "resourcePath: \t " << pathToResources << std::endl;
    std::cout << "outputPath: \t " << pathToOutput << std::endl;
    std::cout << "outputFile: \t " << outputFile << std::endl << std::endl;

    ::google::InitGoogleLogging(argv[0]);
    if(verbose)
        FLAGS_alsologtostderr = true;
    EdgeCaffe::Orchestrator orchestrator;

    /**
     * Instead of defining the networks and the input, we let it be generated by a distribution
     */
    EdgeCaffe::ArrivalList arrivals;
    // Right now sets all the networks to allowed
    // Omitting this line will have the same effect
    arrivals.setAllowedNetworks({"AgeNet", "GenderNet", "SoS", "SoS_GoogleNet", "FaceNet"});
    arrivals.setSeed(seed);
    double interArrivalTime = 1713.4308;
    arrivals.generateList(numArrivals, EdgeCaffe::ArrivalList::DISTRIBUTION_TYPE::POISSON, {interArrivalTime, interArrivalTime/6});

    {
        // Make sure to save the arrival distribution
        std::string arrivalListOutputFile = pathToOutput + "/arrivals7.csv";
        EdgeCaffe::Output output;
        output.toCSV(arrivalListOutputFile, arrivals.toCSVLines(), EdgeCaffe::Output::ARRIVALS);
    }



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

    orchestrator.arrivals = arrivals;

    auto startTime = std::chrono::high_resolution_clock::now();

    orchestrator.start();
    orchestrator.processTasks();
    orchestrator.waitForStop();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << duration << " milliseconds" << std::endl;


    /**
     * Processing the gathered data
     * This part writes the timing information of the layers of the networks to a csv file.
     * It overwrites the specified file. It is important to use a unique filename
     *  for each run of move the data in between runs to prevent losing data.
     */
    EdgeCaffe::Output output;

    std::string layerOutputFile = pathToOutput + "/" + outputFile;
    std::vector<std::string> layerOutputLines;
    for (auto inferenceTasks : orchestrator.inferenceTasks)
        for (std::string line : inferenceTasks->output.toCsvLines())
            layerOutputLines.push_back(line);
    output.toCSV(layerOutputFile, layerOutputLines, EdgeCaffe::Output::LAYER);

    std::string queueEventsFile = pathToOutput + "/stepEvents7.csv";
    std::vector<EdgeCaffe::InferenceOutput::event> taskEvents;
    for (auto inferenceTasks : orchestrator.inferenceTasks)
    {
        auto outputObj = inferenceTasks->output;
        outputObj.getTaskEvents(taskEvents, startTime);
    }
    auto lines_tasks = EdgeCaffe::InferenceOutput::calculateTaskProfile(taskEvents);
    output.toCSV(queueEventsFile, lines_tasks, EdgeCaffe::Output::QUEUE);

    std::vector<std::string> networkLines;
    int networkId = 0;
    for (auto inferenceTasks : orchestrator.inferenceTasks)
    {
        std::string networkName = inferenceTasks->pathToNetwork;
        std::string line = inferenceTasks->output.netProfile.durationAsCSVLine(networkId, networkName, startTime);
        networkLines.push_back(line);
        networkId++;
        //
    }
    std::string networkOutputFile = pathToOutput + "/networkStats6.csv";
    output.toCSV(networkOutputFile, networkLines, EdgeCaffe::Output::NETWORK);

    /**
     * Save the output of the end-to-end measurement
     * Here we append the measurement to the file
     * If the file does not exist it is created.
     */
    std::string generalLine = memLimit + "," + modeAsString + "," + std::to_string(duration);
    output.toCSVAppend(generalOutputFile, {generalLine}, EdgeCaffe::Output::PIPELINE);


    return 0;
}
