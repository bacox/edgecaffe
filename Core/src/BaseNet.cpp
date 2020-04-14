//
// Created by bacox on 31/03/2020.
//

#include "../include/BaseNet.h"
#include <yaml-cpp/yaml.h>
#include <cv.hpp>
#include <memory>

namespace EdgeCaffe {

    std::vector<double> scale_list(const cv::Mat &img) {
        int min = 0;
        int max = 0;
        double delim = 5;
        double factor = 0.7937;
        double factor_count = 0;

        std::vector<double> scales;

        max = MAX(img.cols, img.rows);
        min = MIN(img.cols, img.rows);

        //        delim = 2500 / max;
        while (delim > 1 + 1e-4) {
            scales.push_back(delim);
            delim *= factor;
        }

        while (min >= 227) {
            scales.push_back(pow(factor, factor_count++));
            min *= factor;
        }

//        std::cout << "Image size: " << img.cols << "(Width)" << ' ' << img.rows << "(Height)" << '\n';
//        std::cout << "Scaling: ";
//        std::for_each(scales.begin(), scales.end(), [](double scale) { std::cout << scale << ' '; });
//        std::cout << '\n';
        return scales;
    }

    std::string updatePrototxt(int rows, int cols, std::string pathToProtoText, std::string fileName) {
        std::string orig_proto = "../" + pathToProtoText + fileName;
        std::string changed_proto = "../" + pathToProtoText + "altered_" + fileName;
//        std::cout << "Changing " << orig_proto << " to " << changed_proto << std::endl;
        std::ifstream fin(orig_proto, std::ios::in);
        std::ofstream fout(changed_proto, std::ios::out);
        int index = 0;
        for (std::string line; std::getline(fin, line); index++) {
            if (index == 5) {
                fout << "input_dim: " << rows << '\n';
            } else if (index == 6) {
                fout << "input_dim: " << cols << '\n';
            } else {
                fout << line << '\n';
            }
        }
        fin.close();
        fout.close();
        return changed_proto;
    }

    BaseNet::BaseNet(std::string pathToDescription) : pathToDescription(pathToDescription) {
        init();
        profiler = NetworkProfiler(networkName);
    }

    void BaseNet::init() {
        std::string pathToYaml = pathToDescription + "/description.yaml";
        YAML::Node description = YAML::LoadFile(pathToYaml);

        networkName = description["name"].as<std::string>();

        std::string networkFile = description["network-file"].as<std::string>();
        std::string partialsLocation = description["partials-location"].as<std::string>();
        basePath = description["base-path"].as<std::string>();
        modelFileName = networkFile;
        pathToModelFile = pathToDescription + "/" + networkFile;
        pathToPartials = pathToDescription + "/" + partialsLocation;

        pathToParamFile = pathToDescription + "/" + description["model-file"].as<std::string>();

        hasInputLayer = description["has-input-layer"].as<bool>();
        num_conv = description["num-conv-layers"].as<int>();
        num_fc = description["num-fc-layers"].as<int>();
        num_layers = num_conv + num_fc;
//    std::vector<std::string> resultVector = description["result-vector"].as<std::vector<std::string>>();
        auto iSize = description["input-scale"];
        inputSize = cv::Size(iSize[0].as<int>(), iSize[1].as<int>());
        auto meanValues = description["mean-scalar"];
        modelMeanValues = cv::Scalar(meanValues[0].as<float>(), meanValues[1].as<float>(), meanValues[2].as<float>());


        auto convLayers = description["conv-layers"].as<std::vector<std::string>>();
        auto fcLayers = description["fc-layers"].as<std::vector<std::string>>();

        for (std::string partialName : convLayers) {
            partialNames.push_back(pathToPartials + "/" + partialName);
        }

        for (std::string partialName : fcLayers) {
            partialNames.push_back(pathToPartials + "/" + partialName);
        }

        bool usesResultVector = description["uses-result-vector"].as<bool>();
        if (usesResultVector) {
            resultVector = description["result-vector"].as<std::vector<std::string>>();
        }

    }

    void BaseNet::setInput(cv::Mat &input, bool use_scales) {
        inputData = input;
        preprocess(use_scales);
    }

    void BaseNet::preprocess(bool use_scales) {

        if (use_scales) {
            std::vector<double> scales(scale_list(inputData));
            cv::resize(inputData, inputData, cv::Size(inputData.cols * scales[0], inputData.rows * scales[0]));
            pathToModelFile = updatePrototxt(inputData.rows, inputData.cols, basePath, modelFileName);
        } else {
            cv::resize(inputData, inputData, inputSize);
        }
        inputData.convertTo(inputData, CV_32FC3);
    }

    void BaseNet::loadInputToNetwork() {
        std::vector<cv::Mat> channels;
        cv::split(inputData, channels);
        channels[0] -= modelMeanValues[0];
        channels[1] -= modelMeanValues[1];
        channels[2] -= modelMeanValues[2];

        OpenCV2Blob(channels, *net);
    }

    void BaseNet::loadNetworkStructure() {
        net = new caffe::Net<float>(pathToModelFile, caffe::Phase::TEST);
    }

    void BaseNet::loadPartialLayer(int i) {
        std::cout << "Loading the " << i << "th layer! |-> " << net->layer_names()[i] << std::endl;
        std::cout << "Loading file  " << partialNames[i] << std::endl;

        net->CopyTrainedLayersFrom(partialNames[i]); // layer 12
    }

    void BaseNet::runPartialLayer(int i) {
        std::cout << "Running the " << i << "th layer! |-> " << net->layer_names()[i] << std::endl;
        net->layers()[i]->Forward(net->bottom_vecs()[i], net->top_vecs()[i]);
    }

    void BaseNet::unloadLayer(int i) {

        std::cout << "Unloading layer " << i << std::endl;
        net->layers_unsafe()[i].reset();

    }

    void BaseNet::bulkLoadLayers() {
        net->CopyTrainedLayersFrom(pathToParamFile);
    }

    void BaseNet::bulkRunLayers() {
        net->Forward();

    }

    void BaseNet::runNetwork(bool bulk) {
        profiler.start();
        if (bulk) {
            bulkLoadLayers();
            bulkRunLayers();
        } else {
            int numLayers = net->layers().size();
            for (int i = 0; i < numLayers; ++i) {
                std::string layerName = net->layer_names()[i];
                profiler.profileNewLayer(layerName, i);

                profiler.currentLayer.estimateParamFileSize(partialNames[i]);

                profiler.start(NetworkProfiler::LAYER_PROFILE_TYPE::LOAD);
                loadPartialLayer(i);
                profiler.end(NetworkProfiler::LAYER_PROFILE_TYPE::LOAD);

                profiler.start(NetworkProfiler::LAYER_PROFILE_TYPE::EXEC);
                runPartialLayer(i);
                profiler.end(NetworkProfiler::LAYER_PROFILE_TYPE::EXEC);

                profiler.start(NetworkProfiler::LAYER_PROFILE_TYPE::UNLOAD);
//            unloadLayer(i);
                profiler.end(NetworkProfiler::LAYER_PROFILE_TYPE::UNLOAD);

                profiler.saveLayerProfile();
            }
        }
        profiler.end();
    }

    void BaseNet::createNewProfiler() {
        profiler = NetworkProfiler(networkName);

    }

    NetworkProfiler BaseNet::getProfiler() {
        return profiler;
    }

    void BaseNet::showResult() {

        if (resultVector.size() == 0) {
            std::cout << "No resultvector to show results from " << std::endl;
            return;
        }

        caffe::Blob<float> *output_layer = net->output_blobs()[0];

        const float *begin = output_layer->cpu_data();
        const float *end = begin + output_layer->channels();
        std::vector<float> result = std::vector<float>(begin, end);

        auto it = max_element(std::begin(result), std::end(result));
        long pos = it - std::begin(result);
//
        std::string output = resultVector[pos];
//
        std::cout << "The output of " << networkName << " is: '" << output << "'" << std::endl;

    }

    BaseNet::~BaseNet() {
        std::cout << "Dealloc baseNet" << std::endl;
        delete net;
    }
}