//
// Created by bacox on 31/03/2020.
//

#include <filesystem>
#include "../../include/Profiler/NetworkProfiler.h"

namespace EdgeCaffe {

    void NetworkProfiler::start() {
        networkProfile.start();
    }

    void NetworkProfiler::end() {
        networkProfile.stop();

    }

    void NetworkProfiler::profileNewLayer(std::string layerName, int layerid) {
        currentLayer = LayerProfile();
        currentLayer.layerName = layerName;
        currentLayer.layerId = layerid;
    }

    void NetworkProfiler::start(NetworkProfiler::LAYER_PROFILE_TYPE type) {
        switch (type) {
            case LAYER_PROFILE_TYPE::LOAD :
                currentLayer.loading.start();
            case LAYER_PROFILE_TYPE::EXEC :
                currentLayer.executing.start();
            case LAYER_PROFILE_TYPE::UNLOAD :
                currentLayer.unloading.start();
        }
    }

    void NetworkProfiler::end(NetworkProfiler::LAYER_PROFILE_TYPE type) {
        switch (type) {
            case LAYER_PROFILE_TYPE::LOAD :
                currentLayer.loading.stop();
            case LAYER_PROFILE_TYPE::EXEC :
                currentLayer.executing.stop();
            case LAYER_PROFILE_TYPE::UNLOAD :
                currentLayer.unloading.stop();
        }
    }

    NetworkProfiler::NetworkProfiler(const std::string &networkName) : networkName(networkName) {

    }

    void NetworkProfiler::saveLayerProfile() {
        layerProfiles.push_back(currentLayer);

    }

    std::vector<std::string> NetworkProfiler::layerCsvLines(bool header) {
        std::vector<std::string> lines;
        if (header)
            lines.push_back(LayerProfile::csvHeaders());
        for (auto i : layerProfiles) {
            lines.push_back(i.toCsv());
        }
        return lines;
    }


    void ProfileLine::start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    void ProfileLine::stop() {
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
    }

    std::string LayerProfile::toCsv() {
        std::string sep = ",";

        return layerName + sep + std::to_string(layerId) + sep + std::to_string(loading.duration) + sep +
               std::to_string(executing.duration) + sep + std::to_string(unloading.duration) + sep +
               std::to_string(paramFileSize);
    }

    std::string LayerProfile::csvHeaders() {
        std::string sep = ",";
        return "layerName" + sep + "layerId" + sep + "loading_ns" + sep + "exec_ns" + sep + "unload_ns" + sep +
               "param_file_bytes";
    }

    void LayerProfile::estimateParamFileSize(const std::string &file) {
        paramFileSize = std::filesystem::file_size(file);
    }
}
