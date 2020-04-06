//
// Created by bacox on 09/03/2020.
//

#ifndef CPU_ONLY
#define CPU_ONLY
#endif

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <caffe/caffe.hpp>

void splitModel(std::string modelPath) {
    caffe::NetParameter param;
    caffe::ReadProtoFromBinaryFile(modelPath, &param);

    // Get path parameters
    std::filesystem::path modelFile{modelPath};
    std::string baseFilename = modelFile.filename();
    std::string baseStem = modelFile.stem();

    std::filesystem::path exportPath{modelFile.parent_path() / std::filesystem::path{baseStem + "_partials"}};
    std::filesystem::create_directories(exportPath);

    int num_source_layers = param.layer_size();
    if(num_source_layers > 0)
        std::cout << "Using LayerParameters" << std::endl;
    else
        std::cout << "Using V1LayerParameter" << std::endl;

    for (int i = 0; i < num_source_layers; ++i) {
        const caffe::LayerParameter &source_layer = param.layer(i);
        caffe::NetParameter partial;
        std::string partialName = baseStem + (i < 10 ? "0":"") +  std::to_string(i);
        std::filesystem::path partialPath{exportPath};
        partialPath /= partialName + ".partial.caffemodel";
        partial.set_name(partialName);
        caffe::LayerParameter *newLayer = partial.add_layer();
        newLayer->CopyFrom(source_layer);
        caffe::WriteProtoToBinaryFile(partial, partialPath);
        std::cout << ">\t\tLayer["<< i << "] -> \t" << source_layer.name() << "\t\tCreated partial model: " << partialName << ".partial.caffemodel" << std::endl;
//        std::cout <<  partialName << ".partial.caffemodel" << std::endl;
    }

    int counter = 0;
    for(caffe::V1LayerParameter origLayer : param.layers()) {
        caffe::NetParameter partial;
        std::string partialName = baseStem + (counter < 10 ? "0":"") +std::to_string(counter);
        std::filesystem::path partialPath{exportPath};
        std::string layerName =
        partialPath /= partialName;
        std::string partialModelPath = partialPath.string() + ".partial.caffemodel";
        partial.set_name(partialName);
        caffe::V1LayerParameter *newLayer = partial.add_layers();
        newLayer->CopyFrom(origLayer);
        caffe::WriteProtoToBinaryFile(partial, partialModelPath);
        std::cout << ">\t\tLayer["<< counter << "] -> \t" << origLayer.name() << "\t\tCreated partial model: " << partialModelPath << std::endl;
        counter++;
    }
}




int main(int argc, char *argv[]){

    /*
     * Load param file from argv
     * Load model from file into NetParameter var
     * Split the model
     * Write the partials back to disk
     * */

    // Setup logging
    FLAGS_alsologtostderr = true;
    ::google::InitGoogleLogging(argv[0]);

    // Check for model file in command line
    if(argc < 2) {
        std::cerr << "Missing modelfile" << std::endl;
        std::cout << "Usage: ./ModelSplitter pathToModel1 [pathToModel2] ..." << std::endl;
        return 1;
    }

    // Copy all the paths from the commandline
    std::vector<std::string> models(argc - 1);
    for (int i = 1; i < argc; ++i)
        models[i-1] = argv[i];

    for(const std::string& model : models) {
        std::cout << "Splitting the model " << model << std::endl;
        splitModel(model);
    }

    return 0;
}