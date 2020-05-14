//
// Created by bacox on 31/03/2020.
//

#ifndef PIPELINE_NETWORKPROFILER_H
#define PIPELINE_NETWORKPROFILER_H


#include <string>
#include <chrono>
#include <vector>

namespace EdgeCaffe
{

    struct ProfileLine
    {
        std::chrono::time_point<std::chrono::system_clock> startTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;
        long duration = 0;

        void start();

        void stop();
    };

    struct TaskProfile
    {
        int taskId;
        int layerId;
        bool isLoading = false;
        std::chrono::time_point<std::chrono::system_clock> toWaiting;
        std::chrono::time_point<std::chrono::system_clock> toReady;
        std::chrono::time_point<std::chrono::system_clock> startTask;
        std::chrono::time_point<std::chrono::system_clock> stopTask;
        std::chrono::time_point<std::chrono::system_clock> networkFinished;
    };

    struct NetworkProfile
    {
        enum TYPE
        {
            ARRIVAL,
            START,
            STOP
        };
        std::chrono::time_point<std::chrono::system_clock> arrivalTime;
        std::chrono::time_point<std::chrono::system_clock> startExecution;
        std::chrono::time_point<std::chrono::system_clock> stopExecution;

        void measure(TYPE type)
        {
            std::chrono::time_point<std::chrono::high_resolution_clock> tp = std::chrono::high_resolution_clock::now();
            switch(type)
            {
                case ARRIVAL:
                    arrivalTime = tp;
                    break;
                case START:
                    startExecution = tp;
                    break;
                case STOP:
                    stopExecution = tp;
                    break;
            }
        };

        std::string durationAsCSVLine(int networkId, const std::string &networkName, const std::chrono::time_point<std::chrono::system_clock> &reference)
        {
          std::string line;
          long waitingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(startExecution - arrivalTime).count();
          long executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(stopExecution - startExecution).count();
          long arrivalTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(arrivalTime - reference).count();
          line = std::to_string(networkId) + "," + networkName + "," + std::to_string(arrivalTimestamp) + "," + std::to_string(waitingTime) + "," + std::to_string(executionTime);
          return line;
        };

    };

    struct LayerProfile
    {
        int layerId;
        std::string layerName;
        ProfileLine loading;
        ProfileLine executing;
        ProfileLine unloading;

        size_t paramFileSize = 0;


        void estimateParamFileSize(const std::string &file);

        std::string toCsv();

        static std::string csvHeaders();
    };

    class NetworkProfiler
    {
    public:
        enum LAYER_PROFILE_TYPE
        {
            LOAD,
            EXEC,
            UNLOAD
        };
        std::string networkName;
        ProfileLine networkProfile;

        LayerProfile currentLayer;

        std::vector<LayerProfile> layerProfiles;

        NetworkProfiler(const std::string &networkName);


        void start();

        void end();

        void profileNewLayer(std::string layerName, int layerid);

        void saveLayerProfile();

        void start(LAYER_PROFILE_TYPE type);

        void end(LAYER_PROFILE_TYPE type);

        std::vector<std::string> layerCsvLines(bool header = false);


    };
}

#endif //PIPELINE_NETWORKPROFILER_H
