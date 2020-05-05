//
// Created by bacox on 03/05/2020.
//
#include <iostream>
#include <vector>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>
#include <caffe/net.hpp>
#include <caffe/caffe.hpp>
#include <filesystem>

struct LayerStat {
    int id;
    std::string name;
    long loading;
    long execution;
};

struct LayerDescription {
    int layerId = 0;
    std::string type = "";
    std::string name = "";
    std::string partialFileName = "";
    long estimated_loading = 0;
    long estimated_execution = 0;
    bool isConv = true;
    bool hasModelFile = true;

    friend std::ostream &operator<<(std::ostream &os, const LayerDescription &description)
    {
        os << "layerId: " << description.layerId << " type: " << description.type << " name: " << description.name
           << " partialFileName: " << description.partialFileName << " estimated_loading: "
           << description.estimated_loading << " estimated_execution: " << description.estimated_execution
           << " isConv: " << description.isConv << " hasModelFile: " << description.hasModelFile;
        return os;
    }

    YAML::Node toYaml()
    {
        YAML::Node node;
        node["id"] = layerId;
        node["name"] = name;
        node["partialFile"] = partialFileName;
        node["estimated_time_loading"] = estimated_loading;
        node["estimated_time_execution"] = estimated_execution;
        node["isConvLayer"] = isConv;
        node["hasModelFile"] = hasModelFile;
        return node;
    }
};

std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void extendDescriptionFile(std::string pathToNetworkDir)
{
//    Read yaml
    std::string pathToYaml = pathToNetworkDir + "/description.template.yaml";
    YAML::Node description;
    try{
        description = YAML::LoadFile(pathToYaml);
    } catch(...){
        std::cerr << "Error while attempting to read yaml file!" << std::endl;
        std::cerr << "Yaml file: " << pathToYaml << std::endl;
    }

    std::string networkFile = pathToNetworkDir + "/" + description["network-file"].as<std::string>();
    std::string modelFile = pathToNetworkDir + "/" + description["model-file"].as<std::string>();
    std::string statsFile = pathToNetworkDir + "/" + description["layer-statistics"].as<std::string>();
    bool hasInputLayer = description["has-input-layer"].as<bool>(true);
    std::vector<int> excludedPartials = description["excluded-partials"].as<std::vector<int>>(std::vector<int>());



    std::vector<LayerStat> stats;
    // Read layer statistics
    std::ifstream infile(statsFile);
    std::string line;
    bool firstLine = true;
    while (std::getline(infile, line, '\n'))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        auto items = split(line, ',');
        int layerId = std::stoi(items[0]);
        std::string layerName = items[1];
        long loading = std::stol(items[2]);
        long execution = stol(items[3]);
        stats.push_back({layerId, layerName, loading, execution});
    }
    if(stats.empty())
    {
        std::cerr << "Unable to read the statistics file '" << statsFile << "'!" << std::endl;
        std::cerr << "Using the default 0 values for estimated times now for this network" << std::endl;
    }



//    Read prototext
    auto net = caffe::Net<float>(networkFile, caffe::Phase::TEST);

//    Read binary file
    caffe::NetParameter param;
    caffe::ReadProtoFromBinaryFile(modelFile, &param);

    std::vector<LayerDescription> layers;

    // Get path parameters
    std::filesystem::path modelFileFS{modelFile};
    std::string baseFilename = modelFileFS.filename();
    std::string baseStem = modelFileFS.stem();

    std::filesystem::path exportPath{modelFileFS.parent_path() / std::filesystem::path{baseStem + "_partials"}};
    std::filesystem::create_directories(exportPath);

    int fcLayers = description["num-fc-layers"].as<int>();
    int convLayers = net.layer_names().size() - fcLayers;

    int partialOffset = 0;
    for(int layerId = 0; (size_t) layerId < net.layers().size(); ++layerId)
    {
        std::string layerName = net.layer_names()[layerId];
        auto layer = net.layers()[layerId];
        std::string type = layer->type();
        long loading = stats.empty() ? 0 : stats[layerId].loading;
        long execution = stats.empty() ? 0 :  stats[layerId].execution;
        bool hasModelFile = !(layerId==0 && !hasInputLayer);

        std::string partialModelFile = "";


        bool needsOffset = false;
        for(int excluded : excludedPartials)
            if(layerId + partialOffset == excluded)
                needsOffset = true;
        if(needsOffset)
            partialOffset++;

        std::string partialName = baseStem + (layerId + partialOffset < 10 ? "0" : "")  + std::to_string(layerId + partialOffset);
        std::filesystem::path partialPath{exportPath};
        std::string layerNameFS =
                partialPath /= partialName;

        if(hasModelFile)
        {
            partialModelFile = partialName + ".partial.caffemodel";
        } else
        {
            partialOffset--;
        }

        layers.push_back({layerId, type, layerName, partialModelFile, loading, execution, (layerId < convLayers), hasModelFile});

    }
    YAML::Node layersNode;
    for( auto item : layers)
    {
        layersNode.push_back(item.toYaml());
        auto layerInYaml = item.toYaml();
        if(item.isConv)
            description["conv-layers"].push_back(layerInYaml);
        else
            description["fc-layers"].push_back(layerInYaml);
    }

    std::ofstream fout(pathToNetworkDir + "/" + "description.yaml");
    fout << description;
    std::cout << "Extended model description file written to " << (pathToNetworkDir + "/" + "description.yaml") << std::endl;
}

/**
 * ExtendNetworkDescription extends the network description in description.template.yaml
 * By combining 3 files we will get an detailed description of all the layers in the network.
 * The network description (*.prototxt) and the modelfile (*.caffemodel) is combined to get the basic information of each layer.
 * The statistics file (optional) of the network (*.stats.csv) is used to get the estimated times of loading and executing the layers.
 *      When the statistics file is not present, the default value '0' will be used for loading/execution times.
 *      Statistic files can created using the python notebook at 'analysis/layer-satistics.exp.ipynb'
 */
int main(int argc, char *argv[])
{
    // Setup logging
    FLAGS_alsologtostderr = false;
    ::google::InitGoogleLogging(argv[0]);

    // Check for model file in command line
    if (argc < 2)
    {
        std::cerr << "Missing pathToNetworkDirectory" << std::endl;
        std::cout << "Usage: ./ExtendNetworkDescription pathToModel1 [pathToModel2] ..." << std::endl;
        return 1;
    }

    // Copy all the paths from the commandline
    std::vector<std::string> pathToNetworks(argc - 1);
    for (int i = 1; i < argc; ++i)
        pathToNetworks[i - 1] = argv[i];

    for (const std::string &pathToNetworkDirectory : pathToNetworks)
    {
        std::cout << "Extending the model " << pathToNetworkDirectory << std::endl;
        extendDescriptionFile(pathToNetworkDirectory);
    }
}