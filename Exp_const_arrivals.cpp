//
// Created by bacox on 08/04/2020.
//
#include <iostream>
#include <EdgeCaffe.h>
#include <chrono>
#include <Util/Output.h>
#include <cxxopts.h>
#include <iomanip>      // std::setw

template<typename T>
T getArgs(const cxxopts::ParseResult &result, std::string key, T defaultValue,const YAML::Node &yamlConfig,std::map<std::string, std::string> &configAsText)
{
    T parsed = defaultValue;
    try{
        parsed = yamlConfig[key].as<T>();
    } catch(...){}
    if(result.count(key))
    {
        parsed = result[key].as<T>();
    }
    std::stringstream stream;
    stream << parsed;
    configAsText[key] = stream.str();
    return parsed;
}

void printConfig(const std::string &programName, std::map<std::string, std::string> &configAsText)
{
    std::cout << "==========[Config]==========" << std::endl;
    int align = 20;
    std::cout << "Running" << std::setw(align - 7) << "'" << programName << "'" << std::endl;
    for(auto pair : configAsText)
    {
        if (pair.second.empty())
            continue;
        std::cout << pair.first << std::setw(align - pair.first.length()) << ": " << pair.second << std::endl;
    }
    std::cout << std::endl;
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
            ("c,read-config", "Use a yaml config file to configure this run instead of the cli. This will overrule all other arguments. Example 'config/pipeline-template.yaml'", cxxopts::value<std::string>())
            ("h,help", "Print help message")
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);

    // Check if the help message needs to be printed;
    if (result.count("help") or result.arguments().size() == 0)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }

    YAML::Node config;
    std::map<std::string,std::string> configAsText;
    std::string pathToConfig = getArgs<std::string>(result, "read-config", "", config, configAsText);
    // Check if yaml file is provided
    try{
        config = YAML::LoadFile(pathToConfig);
    } catch(...)
    {
        if(result.count("read-config") == 0)
        {
            if(result.count("verbose") > 0)
            {
                std::cerr << "Error while attempting to read yaml file!" << std::endl;
                std::cerr << "Yaml file: " << pathToConfig << std::endl;
            }
        }
    }
    std::string defaultMode = getArgs<std::string>(result, "mode", "partial", config, configAsText);
    long seed = getArgs<long>(result, "seed", -1, config, configAsText);
    std::string memLimit = getArgs<std::string>(result, "mem_limit", "1GB", config, configAsText);
    bool verbose = getArgs<bool>(result, "verbose", false, config, configAsText);
    std::string schedAlg = getArgs<std::string>(result, "sched-alg", "FCFS", config, configAsText);
    int numArrivals = getArgs<int>(result, "num-arrivals", 10, config, configAsText);
    std::string outputPrefix = getArgs<std::string>(result, "output-prefix", "", config, configAsText);
    std::string pathToOutput = getArgs<std::string>(result, "output-path", "../analysis", config, configAsText);
    std::string pathToNetworks = getArgs<std::string>(result, "network-path", "networks", config, configAsText);
    std::string pathToResources = getArgs<std::string>(result, "resources-path", "../resources", config, configAsText);
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
    configAsText["defaultMode"] = modeAsString;
    std::string outputFile = "output.csv";
    configAsText["outputFile"] = outputFile;
    std::string generalOutputFile = "./generalOutput.csv";
    configAsText["generalOutputFile"] = generalOutputFile;

    printConfig("Exp_const_arrivals", configAsText);

    ::google::InitGoogleLogging(argv[0]);
    if(verbose)
        FLAGS_alsologtostderr = true;

    /**
     * End of configuring all.
     * The real running begins here
     */

    EdgeCaffe::Orchestrator orchestrator;

    /**
     * Instead of defining the networks and the input, we let it be generated by a distribution
     */
    EdgeCaffe::ArrivalList arrivals;
    // Right now sets all the networks to allowed
    // Omitting this line will have the same effect
//    arrivals.setAllowedNetworks({"AgeNet", "GenderNet", "SoS", "SoS_GoogleNet", "FaceNet"});
    arrivals.setEnabledNetworks({{"SoS_GoogleNet"}});
    arrivals.setSeed(seed);
//    double interArrivalTime = 1713.4308;
    double interArrivalTime = 20'000;
    arrivals.generateList(numArrivals, EdgeCaffe::ArrivalList::DISTRIBUTION_TYPE::CONSTANT, {interArrivalTime, interArrivalTime/6});

    {
        // Make sure to save the arrival distribution
        std::string arrivalListOutputFile = pathToOutput + "/arrivals7.csv";
        EdgeCaffe::Output output;
        output.toCSV(arrivalListOutputFile, arrivals.toCSVLines(), EdgeCaffe::Output::ARRIVALS);
    }


    orchestrator.setup(mode, modeAsString);
    orchestrator.setArrivals(arrivals);

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

    std::string layerOutputFile = pathToOutput + "/" + outputFile;
    orchestrator.processLayerData(layerOutputFile);
    std::string queueEventsFile = pathToOutput + "/stepEvents7.csv";
    orchestrator.processEventData(queueEventsFile, startTime);
    std::string networkOutputFile = pathToOutput + "/networkStats6.csv";
    orchestrator.processNetworkData(networkOutputFile, startTime);

    /**
     * Save the output of the end-to-end measurement
     * Here we append the measurement to the file
     * If the file does not exist it is created.
     */
    EdgeCaffe::Output output;
    std::string generalLine = memLimit + "," + modeAsString + "," + std::to_string(duration) + "," + std::to_string(numArrivals);
    output.toCSVAppend(generalOutputFile, {generalLine}, EdgeCaffe::Output::PIPELINE);


    return 0;
}
