//
// Created by jeroen on 5/8/20.
//

#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include <cxxopts.h>
#include <iostream>
#include <iomanip>
#include <Orchestrator/Orchestrator.h>

#ifndef EDGECAFFE_CONFIG_H
#define EDGECAFFE_CONFIG_H

namespace EdgeCaffe {
    template<class T>
    class ConfigOption{
    public:
        std::optional<T> configItem;
        T defaultValue;
    public:
        ConfigOption(T defaultValue) : defaultValue(defaultValue)
        {}

        bool isSet(){
            return configItem.has_value();
        }

        T value()
        {
            return configItem.value();
        }

        T valueOrDefault() const
        {
            return configItem.has_value() ? configItem.value() : defaultValue;
        }

        T operator()() const {
            return valueOrDefault();
        }

    };


    /**
     * Config is a Config singleton that contains run configurations, such as memory, cores, relevant directories etc.
     * use getInstance to gain access to the data stored in the Config object. Using the Config object runtime related
     * parameters can be easily accessed without passing around through function calls.
     */
    class Config {
    private:
        std::string programName = "EdgeCaffe";
        std::string helpMessage = "\n\nExp_const_arrivals executes DNN's using the EdgeCaffe framework based on different arrivals."
                                  "\nArrivals arrive at the system based on the generated inter-arrival time."
                                  "\nNote: some cli options are not yet implemented";
        cxxopts::Options options = cxxopts::Options(programName,"Efficient execution of DNN on edge devices." + helpMessage);
    public:

        // Values of the config file that can be set
        ConfigOption<std::string> outputDirectory = ConfigOption<std::string>("../anaysis");
        ConfigOption<std::size_t> numberOfWorkers = ConfigOption<std::size_t>(2);
        ConfigOption<std::string> pathToNetworks = ConfigOption<std::string>("networks");
        ConfigOption<std::string> pathToResources = ConfigOption<std::string>("../resources");
        ConfigOption<std::string> outputPrefix= ConfigOption<std::string>("");
        ConfigOption<std::string> pathToArrivalList =ConfigOption<std::string>("");
        ConfigOption<std::string> generalOutputFile =ConfigOption<std::string>("generalOutput.csv");
        ConfigOption<std::size_t> seed = ConfigOption<std::size_t>(0);
        ConfigOption<bool> verbose = ConfigOption<bool>(false);
        ConfigOption<std::string> memoryLimit = ConfigOption<std::string>("4G");
        ConfigOption<int> memoryLimitAsNum = ConfigOption<int>(Type::memory_values.at("4G"));
        ConfigOption<std::string> mode= ConfigOption<std::string>("partial");
        ConfigOption<Type::MODE_TYPE> modeAsType= ConfigOption<Type::MODE_TYPE>(Type::MODE_TYPE::PARTIAL);
        ConfigOption<std::size_t> numArrivals = ConfigOption<std::size_t>(1);
        ConfigOption<std::string> pathToConfig = ConfigOption<std::string>("");
        ConfigOption<std::string> schedAlg = ConfigOption<std::string>("FCFS");
        ConfigOption<bool> poissonDistribution = ConfigOption<bool>(true);
        ConfigOption<bool> help = ConfigOption<bool>(false);
        ConfigOption<double> iat = ConfigOption<double>(100);
        ConfigOption<std::vector<std::string>> networks = ConfigOption<std::vector<std::string>>({});
        ConfigOption<std::string> arrivalMode= ConfigOption<std::string>("batch");
        ConfigOption<std::string> memoryKey= ConfigOption<std::string>("valgrind");
        ConfigOption<Type::ARRIVAL_MODE> arrivalModeAsType= ConfigOption<Type::ARRIVAL_MODE>(Type::ARRIVAL_MODE::BATCH);

        ConfigOption<YAML::Node> configFile = YAML::Node();
        std::map<std::string, std::string> configAsText;


        void printConfig()
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

        template<typename T>
        void parseArg(const cxxopts::ParseResult &result, std::string key,ConfigOption<T> & c_option)
        {
            // First try the cli as this has precedence
            if(result.count(key))
            {
                c_option.configItem = result[key].as<T>();
                return;
            }
            // Else check the yaml file
            if(configFile.isSet())
            {
                try{
                    c_option.configItem = configFile.value()[key].as<T>();
                } catch(...){}
            }
            // Finally if all fails --> use default value and thus do nothing
        }

        void parseCli(int argc, char *argv[])
        {
            // First check if a config file is given
            cxxopts::ParseResult result = options.parse(argc, argv);
            if (result.count("help") or result.arguments().size() == 0)
            {
                help.configItem = true;
                std::cout << options.help({"", "Group"}) << std::endl;
                exit(0);
            }

            if( result.count("read-config"))
            {
                // Config file has been set
                pathToConfig.configItem = result["read-config"].as<std::string>();
                try{
                    configFile.configItem = YAML::LoadFile(pathToConfig.value());
                    // Update parameters if found

                } catch(...)
                {
                    std::cerr << "Error while attempting to read yaml file!" << std::endl;
                    std::cerr << "Yaml file: " << pathToConfig.value() << std::endl;
                }

            }


            parseArg(result, "mode", mode);
            modeAsType.configItem = Type::mapToVals.at(mode.valueOrDefault());
            configAsText["mode"] = mode.valueOrDefault();

            parseArg(result, "arrival-mode", arrivalMode);
            arrivalModeAsType.configItem = Type::arrivalModeToVals.at(arrivalMode.valueOrDefault());
            configAsText["arrival-mode"] = arrivalMode.valueOrDefault();

            parseArg(result, "seed", seed);
            configAsText["seed"] = std::to_string(seed.valueOrDefault());
            parseArg(result, "mem_limit", memoryLimit);
            memoryLimitAsNum.configItem = Type::memory_values.at(memoryLimit.valueOrDefault());
            configAsText["mem_limit"] = memoryLimit.valueOrDefault();
            parseArg(result, "verbose", verbose);
            configAsText["verbose"] = std::to_string(verbose.valueOrDefault());
            parseArg(result, "sched-alg", schedAlg);
            configAsText["sched"] = schedAlg.valueOrDefault();

            parseArg(result, "memory-key", memoryKey);
            if(memoryKey() != "rss" && memoryKey() != "valgrind" && memoryKey() != "valgrind-050"  && memoryKey() != "valgrind-150" && memoryKey() != "valgrind-075"  && memoryKey() != "valgrind-125")
            {
                std::cerr << "Invalid config: " << memoryKey() << " is not allowed for 'memory-key'" << std::endl;
                memoryKey.configItem = "valgrind";
            }

            memoryKey.configItem = "max-memory-" + memoryKey();
            configAsText["memory-key"] = memoryKey.valueOrDefault();



            parseArg(result, "n-arrivals", numArrivals);
            configAsText["n-arrivals"] = std::to_string(numArrivals.valueOrDefault());
            parseArg(result, "output-prefix", outputPrefix);
            configAsText["output-prefix"] = outputPrefix.valueOrDefault();
            parseArg(result, "output-path", outputDirectory);
            configAsText["output-path"] = outputDirectory.valueOrDefault();
            parseArg(result, "network-path", pathToNetworks);
            configAsText["network-path"] = pathToNetworks.valueOrDefault();
            parseArg(result, "resources-path", pathToResources);
            configAsText["resources-path"] = pathToResources.valueOrDefault();
            parseArg(result, "general-outputfile", generalOutputFile);
            configAsText["general-outputfile"] = generalOutputFile.valueOrDefault();
            parseArg(result, "arrival-list", pathToArrivalList);
            configAsText["arrival-list"] = pathToArrivalList.valueOrDefault();
            parseArg(result, "n-workers", numberOfWorkers);
            configAsText["n-workers"] = std::to_string(numberOfWorkers.valueOrDefault());
            parseArg(result, "poisson-distribution", poissonDistribution);
            configAsText["poisson-distribution"] = std::to_string(poissonDistribution.valueOrDefault());
            parseArg(result, "network", networks);
            std::string networksAsString = "";
            for(std::string network : networks.valueOrDefault()){
                networksAsString += network + " ";
            }

            configAsText["network"] = networksAsString;
            parseArg(result, "ait", iat);
            configAsText["ait"] = std::to_string(iat.valueOrDefault());
        }

        std::string defaultOutPath()
        {
            return outputDirectory() + "/" + outputPrefix();
        }



//        std::string analysis_directory;
//        std::string memory_description;
//        long available_mb;
//        std::string execution_mode;
//        std::string schedule_mode;
//        std::shared_ptr<YAML::Node> profileCollection;
//        std::size_t max_workers;

        static Config &getInstance() {
            static Config instance;
            return instance;
        }

    private:
        Config() {
            /* Set the config options */
            options.add_options()
                    (
                            "m,mode", "Mode to split and run the networks. Values [partial|linear|deepeye|bulk]"
                            , cxxopts::value<std::string>())
                    (
                            "mem_limit"
                            , "The memory limit given by the OS to EdgeCaffe. NOTE: this does not limit the memory for this process but is used in output generation and in scheduling."
                            , cxxopts::value<std::string>())
                    ("seed", "Seed for random number generator", cxxopts::value<std::size_t>())
                    ("V,verbose", "Verbose")
                    ("N,n-arrivals", "Number of arrivals to be generated", cxxopts::value<std::size_t>())
                    ("arrival-mode", "How arrivals should be handled: batch or stochastic", cxxopts::value<std::string>())
                    (
                            "a,arrival-list"
                            , "Use this arrival list to inject arrivals instead of the generated one"
                            , cxxopts::value<std::string>())
                    (
                            "p,output-prefix", "Prefix for all output files to make it them unique for a certain run"
                            , cxxopts::value<std::string>())
                    ("output-path", "Define the path to store all output files", cxxopts::value<std::string>())
                    ("network-path", "Define the path to store all output files", cxxopts::value<std::string>())
                    ("resources-path", "Define the path to store all output files", cxxopts::value<std::string>())
                    ("memory-key", "Define the key to be used in the config for reading the required memory for the network. Options: one of ['valgrind', 'rss'] Default is 'valgrind'", cxxopts::value<std::string>())
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
                    ("n-workers", "Set the number of workers to be used for execution of the tasks. This only has effect on algorithms that do not use a fixed number of workers", cxxopts::value<std::size_t>())
                    ("poisson-distribution","enable or disable poisson distribution as the arrival process. With this flag disabled, a constant distribution will be used.", cxxopts::value<bool>())
                    ("h,help", "Print help message");

        }

    public:
        Config(Config const &) = delete;

        void operator=(Config const &) = delete;

    };
}


#endif //EDGECAFFE_CONFIG_H
