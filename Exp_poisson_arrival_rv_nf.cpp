//
// Created by bacox on 25/05/2020.
//

//
// Created by bacox on 25/05/2020.
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
     * Vary Rho, fix number of networks
     */

    /**
     * Start parsing input parameters
     */
    std::string helpMessage = "\n\nExp_const_arrivals executes DNN's using the EdgeCaffe framework based on different arrivals."
                              "\nArrivals arrive at the system based on the generated inter-arrival time."
                              "\nNote: some cli options are not yet implemented";

    cxxopts::Options options("Exp_const_arrivals", "One line description of MyProgram" + helpMessage);
    options.add_options()
            (
                    "m,mode", "Mode to split and run the networks. Values [partial|linear|deepeye|bulk]"
                    , cxxopts::value<std::string>())
            (
                    "mem_limit"
                    , "The memory limit given by the OS to EdgeCaffe. NOTE: this does not limit the memory for this process but is used in output generation and in scheduling."
                    , cxxopts::value<std::string>())
            ("seed", "Seed for random number generator", cxxopts::value<long>())
            ("V,verbose", "Verbose")
            ("N,num-arrivals", "Number of arrivals to be generated", cxxopts::value<int>())
            (
                    "a,arrival-list"
                    , "NOT_YET_IMPLEMENTED. Use this arrival list to inject arrivals instead of the generated one"
                    , cxxopts::value<std::string>())
            (
                    "p,output-prefix", "Prefix for all output files to make it them unique for a certain run"
                    , cxxopts::value<std::string>())
            ("output-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("network-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("resources-path", "Define the path to store all output files", cxxopts::value<std::string>())
            ("s, sched-alg", "The scheduling algorithm to be used: [FCFS|SJF]", cxxopts::value<std::string>())
            (
                    "c,read-config"
                    , "Use a yaml config file to configure this run instead of the cli. This will overrule all other arguments. Example 'config/pipeline-template.yaml'"
                    , cxxopts::value<std::string>())
            (
                    "network"
                    , "The network(s) to run"
                    , cxxopts::value<std::vector<std::string>>())
            (
                    "rho"
                    , "Set the Rho value to use"
                    , cxxopts::value<double>())
            ("mst", "Set the mean service time to use", cxxopts::value<double>())
            ("iat", "Set the inter arrival time to use", cxxopts::value<double>())
            ("poisson-distribution","enable or disable poisson distribution as the arrival process. With this flag disabled, a constant distribution will be used.", cxxopts::value<bool>())
            ("h,help", "Print help message");

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
        // Update parameters if found

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
    std::string memLimit = getArgs<std::string>(result, "mem_limit", "1G", config, configAsText);
    bool verbose = getArgs<bool>(result, "verbose", false, config, configAsText);
    std::string schedAlg = getArgs<std::string>(result, "sched-alg", "FCFS", config, configAsText);
    int numArrivals = getArgs<int>(result, "num-arrivals", 10, config, configAsText);
    std::string outputPrefix = getArgs<std::string>(result, "output-prefix", "", config, configAsText);
    std::string pathToOutput = getArgs<std::string>(result, "output-path", "../analysis", config, configAsText);
    std::string pathToNetworks = getArgs<std::string>(result, "network-path", "networks", config, configAsText);
    std::string pathToResources = getArgs<std::string>(result, "resources-path", "../resources", config, configAsText);
    std::string generalOutputFile = getArgs<std::string>(result, "general-outputfile", "generalOutput.csv", config, configAsText);
    bool poissonDistribution = getArgs<bool>(result, "poisson-distribution", true, config, configAsText);

    double rho = getArgs<double>(result, "rho", 0, config, configAsText);
    double mst = getArgs<double>(result, "mst", 1, config, configAsText);
    double iat = getArgs<double>(result, "iat", 1000, config, configAsText);

    // std::vector<std::string> selectedNetwork = {"AgeNet", "FaceNet", "SoS_GoogleNet"};
    // std::vector<std::string> selectedNetwork = {"AgeNet", "GenderNet"};
    std::vector<std::string> selectedNetwork = {"AgeNet", "GenderNet", "FaceNet", "SoS"};
    if(result.count("network"))
    {
        selectedNetwork = result["network"].as<std::vector<std::string>>();
    }
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
    } else if (defaultMode == "execprio")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::PRIO_EXEC;
        modeAsString = "execprio";
    }else if (defaultMode == "execprio-inter")
    {
        mode = EdgeCaffe::Orchestrator::MODEL_SPLIT_MODE::PRIO_EXEC_INTER;
        modeAsString = "execprio-inter";
    }
    configAsText["defaultMode"] = modeAsString;
    std::string outputFile = "output.csv";
    configAsText["outputFile"] = outputFile;
//    std::string generalOutputFile = "./generalOutput.csv";
//    configAsText["generalOutputFile"] = generalOutputFile;

    double meanServiceTime = 0;
    try
    {
        auto meanServiceTimes = config["mean-service-time"].as<std::map<std::string, double>>();
        meanServiceTime = meanServiceTimes[memLimit];
    } catch(...){}
    configAsText["meanServiceTime"] = std::to_string(meanServiceTime);
    // double interArrivalTime = 1.0 /( (1.0/meanServiceTime) * rho);
    double interArrivalTime = iat;
    configAsText["interArrivalTime"] = std::to_string(interArrivalTime);

    printConfig("Exp_const_arrivals", configAsText);
    {
        EdgeCaffe::Output output;
        std::string configOutputFile = outputPrefix + "config.csv";
        output.toCSV(pathToOutput + "/" + configOutputFile, configAsText, EdgeCaffe::Output::CONFIG);
    }


    /**
     * End of configuring all.
     * The real running begins here
     */
//    if(!verbose)
    ::google::InitGoogleLogging(argv[0]);

    EdgeCaffe::Orchestrator orchestrator;

    /**
     * Instead of defining the networks and the input, we let it be generated by a distribution
     */
    EdgeCaffe::ArrivalList arrivals;
    // Right now sets all the networks to allowed
    // Omitting this line will have the same effect
//    arrivals.setAllowedNetworks({"AgeNet", "GenderNet", "SoS", "SoS_GoogleNet", "FaceNet"});
//    std::vector<std::vector<std::string>> singleBatchList;
//    for(auto network : selectedNetwork)
//    {
//        singleBatchList.push_back({network});
//    }
//    arrivals.setEnabledNetworks(singleBatchList);
    // arrivals.setEnabledNetworks({{"FaceNet", "FaceNet"}});
    arrivals.setEnabledNetworks({{"AgeNet", "AgeNet"}});
//    arrivals.setEnabledNetworks({{"AgeNet", "AgeNet", "AgeNet", "AgeNet", "AgeNet"}});
//    arrivals.setEnabledNetworks({{"SoS", "SoS", "SoS", "SoS", "SoS"}});
//    arrivals.setEnabledNetworks({{"AgeNet"}});
    arrivals.setSeed(seed);
//    double interArrivalTime = 1713.4308;
//    double interArrivalTime = 20'000;
poissonDistribution = true;
    if(poissonDistribution){
        arrivals.generateList(numArrivals, EdgeCaffe::ArrivalList::DISTRIBUTION_TYPE::POISSON, {interArrivalTime});
//        arrivals.generateList(2, EdgeCaffe::ArrivalList::DISTRIBUTION_TYPE::POISSON, {1});

    } else {
        arrivals.generateList(numArrivals, EdgeCaffe::ArrivalList::DISTRIBUTION_TYPE::CONSTANT, {0});

    }
    {
        // Make sure to save the arrival distribution
        std::string arrivalListOutputFile = pathToOutput + "/" + outputPrefix + "arrivals.csv";
        EdgeCaffe::Output output;
        output.toCSV(arrivalListOutputFile, arrivals.toCSVLines(), EdgeCaffe::Output::ARRIVALS);
    }


    orchestrator.setup(mode, modeAsString);
    orchestrator.verbose = verbose;
    for(const auto &worker : orchestrator.getWorkers())
        worker->verbose = verbose;
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

    std::string layerOutputFile = pathToOutput + "/" + outputPrefix + outputFile;
    orchestrator.processLayerData(layerOutputFile);
    std::string queueEventsFile = pathToOutput + "/" + outputPrefix + "stepEvents.csv";
    orchestrator.processEventData(queueEventsFile, startTime);
    std::string networkOutputFile = pathToOutput + "/" + outputPrefix + "networkStats6.csv";
    orchestrator.processNetworkData(networkOutputFile, startTime);

    /**
     * Save the output of the end-to-end measurement
     * Here we append the measurement to the file
     * If the file does not exist it is created.
     */
    EdgeCaffe::Output output;
    std::string generalLine = memLimit + "," + modeAsString + "," + std::to_string(duration) + "," + std::to_string(numArrivals) + "," + selectedNetwork.front() + "," + std::to_string(rho) + "," + std::to_string(meanServiceTime) + "," + std::to_string(interArrivalTime);
    output.toCSVAppend(pathToOutput + "/" + generalOutputFile, {generalLine}, EdgeCaffe::Output::PIPELINE_EXT);


    for(auto worker : orchestrator.getWorkers())
    {
        auto id = worker->workerId;
        std::string workerStatFile = pathToOutput + "/" + outputPrefix + "worker" + std::to_string(id) + ".csv";
        auto lines = worker->workerProfileToCSVLines();
        output.toCSV(workerStatFile, lines, EdgeCaffe::Output::WORKER);
    }
    return 0;
}