//
// Created by bacox on 11/05/2020.
//

#ifndef EDGECAFFE_OUTPUT_H
#define EDGECAFFE_OUTPUT_H

#include <string>
#include <map>
#include <vector>

namespace EdgeCaffe{
    class Output
    {
    public:
        void ensurePathExists(std::string pathToFile);

        bool fileExists(std::string pathToFile);

        enum TYPE {
            LAYER,
            QUEUE,
            PIPELINE,
            PIPELINE_EXT,
//            PIPELINE_RHO,
            ARRIVALS,
            NETWORK,
            WORKER,
            CONFIG,
            CONCURRENT_NETWORKS
        };

        std::map <TYPE, std::string> headers {
                 { LAYER,  "networkName,layerId,layerName,Loading_ns,execution_ns,policy"}
                 , { QUEUE, "time,count,type"}
                 , { PIPELINE, "mem_limit,policy,time,number-of-networks"}
                 , { PIPELINE_EXT, "mem_limit,policy,time,number-of-networks,network,rho,mst,iat,n_workers"}
//                 , { PIPELINE_RHO, "mem_limit,policy,time,number-of-networks,rho,mean_service_time,iat"}
                 , { ARRIVALS, "inter-arrival-time,network"}
                 , { NETWORK, "networkId,networkName,arrivalTimestamp,waitingTime,executionTime"}
                , { WORKER, "startTime,stopTime,duration,type"}
                , { CONFIG, "variable,value"}
                , {CONCURRENT_NETWORKS, "timestamp,number-of-networks"}
        };

        void toCSV(std::string pathToFile, std::vector<std::string> lines, TYPE type, bool writeHeaders = true);
        void toCSV(std::string pathToFile, std::map<std::string,std::string> &config, TYPE type, bool writeHeaders = true);
        void toCSVAppend(std::string pathToFile, std::vector<std::string> lines, TYPE type);
    };
}


#endif //EDGECAFFE_OUTPUT_H
