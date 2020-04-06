//
// Created by bacox on 31/03/2020.
//

#ifndef PIPELINE_NETWORKPROFILER_H
#define PIPELINE_NETWORKPROFILER_H


#include <string>
#include <chrono>
#include <vector>


struct ProfileLine {
    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;
    long duration = 0;

    void start();
    void stop();
};
struct LayerProfile {
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

class NetworkProfiler {
public:
    enum LAYER_PROFILE_TYPE{
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


#endif //PIPELINE_NETWORKPROFILER_H
