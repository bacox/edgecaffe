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
            ARRIVALS
        };

        std::map <TYPE, std::string> headers {
                 { LAYER,  "networkName,layerId,layerName,Loading_ns,execution_ns,policy"}
                 , { QUEUE, "time,count,type"}
                 , { PIPELINE, "mem_limit,policy,time"}
                 , { ARRIVALS, "inter-arrival-time,network"}
        };

        void toCSV(std::string pathToFile, std::vector<std::string> lines, TYPE type, bool writeHeaders = true);
        void toCSVAppend(std::string pathToFile, std::vector<std::string> lines, TYPE type);
    };
}


#endif //EDGECAFFE_OUTPUT_H
