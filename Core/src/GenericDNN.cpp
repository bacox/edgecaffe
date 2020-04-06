//
// Created by bacox on 23/03/2020.
//

#include <yaml-cpp/yaml.h>
#include <cv.hpp>
#include "../include/GenericDNN.h"
#include <memory>


std::vector<double> scale_list(const cv::Mat &img)
{
    int min             = 0;
    int max             = 0;
    double delim        = 5;
    double factor       = 0.7937;
    double factor_count = 0;

    std::vector<double> scales;

    max = MAX(img.cols, img.rows);
    min = MIN(img.cols, img.rows);

    //        delim = 2500 / max;
    while (delim > 1 + 1e-4)
    {
        scales.push_back(delim);
        delim *= factor;
    }

    while (min >= 227)
    {
        scales.push_back(pow(factor, factor_count++));
        min *= factor;
    }

    std::cout << "Image size: " << img.cols << "(Width)" << ' ' << img.rows <<  "(Height)" <<'\n';
    std::cout << "Scaling: ";
    std::for_each(scales.begin(), scales.end(), [](double scale){ std::cout << scale << ' '; });
    std::cout << '\n';
    return scales;
}

std::string updatePrototxt(int rows, int cols, std::string pathToProtoText, std::string fileName)
{
    std::string orig_proto = "../" + pathToProtoText + fileName;
    std::string changed_proto = "../" + pathToProtoText + "altered_" + fileName;
    std::cout << "Changing " << orig_proto << " to " << changed_proto << std::endl;
    std::ifstream fin(orig_proto, std::ios::in);
    std::ofstream fout(changed_proto, std::ios::out);
    int index = 0;
    for (std::string line; std::getline(fin, line); index++)
    {
        if (index == 5)
        {
            fout << "input_dim: " << rows << '\n';
        }
        else if (index == 6)
        {
            fout << "input_dim: " << cols << '\n';
        }
        else
        {
            fout << line << '\n';
        }
    }
    fin.close();
    fout.close();
    return changed_proto;
}

GenericDNN::GenericDNN(std::string pathToDescription) : pathToDescription(pathToDescription){
    init();
}

void GenericDNN::init() {
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

    for(std::string partialName : convLayers) {
        partialNames.push_back(pathToPartials + "/" + partialName);
    }

    for(std::string partialName : fcLayers) {
        partialNames.push_back(pathToPartials + "/" + partialName);
    }

}

void GenericDNN::setInput(cv::Mat &input, bool use_scales) {
    inputData = input;
    preprocess(use_scales);
}

void GenericDNN::preprocess(bool use_scales) {

    if(use_scales)
    {
        std::vector<double> scales(scale_list(inputData));
        cv::resize(inputData, inputData, cv::Size(inputData.cols * scales[0], inputData.rows * scales[0]));
        pathToModelFile = updatePrototxt(inputData.rows, inputData.cols, basePath, modelFileName);
    } else {
        cv::resize(inputData, inputData, inputSize);
    }
    inputData.convertTo(inputData, CV_32FC3);
}

void GenericDNN::loadInputToNetwork() {
    std::vector<cv::Mat> channels;
    cv::split(inputData, channels);
    channels[0] -= modelMeanValues[0];
    channels[1] -= modelMeanValues[1];
    channels[2] -= modelMeanValues[2];

    OpenCV2Blob(channels, *net);
}

void GenericDNN::loadNetworkStructure() {
//    net(new caffe::Net<float>(pathToModelFile, caffe::Phase::TEST));
//    net = std::make_shared<caffe::Net<float>>(pathToModelFile, caffe::Phase::TEST);
    net = new caffe::Net<float>(pathToModelFile, caffe::Phase::TEST);
}

void GenericDNN::loadPartialLayer(int i) {
    std::cout << "Loading the " << i << "th layer! |-> " << net->layer_names()[i] << std::endl;
    std::cout << "Loading file  " << partialNames[i] << std::endl;

    net->CopyTrainedLayersFrom(partialNames[i]); // layer 12
}

void GenericDNN::runPartialLayer(int i) {
    std::cout << "Running the " << i << "th layer! |-> " << net->layer_names()[i] << std::endl;
    auto layer = net->layers()[i];
    auto bottom = net->bottom_vecs()[i];
    auto top = net->top_vecs()[i];
    // Use slow version?
//    net->ForwardFromTo(i,i+1);
    // Fast version...
    net->layers()[i]->Forward(net->bottom_vecs()[i],net->top_vecs()[i]);
}

void GenericDNN::runPartialLayer(caffe::Layer<float> *layer, int i) {
    std::cout << "Running the " << i << "th layer version 2! |-> " << net->layer_names()[i] << std::endl;
    layer->Forward(net->bottom_vecs()[i],net->top_vecs()[i]);
}

void GenericDNN::unloadLayer(int i) {
//    std::make_unique<caffe::Layer<float>>(net->layers_unsafe()[i]);
//    net->layers_unsafe()[i].;
//    net->layers_unsafe().erase(net->layers_unsafe().begin() + i);
    std::cout << "Unloading layer " << i << std::endl;
//    delete net->layers()[i].get();
    net->layers_unsafe()[i].reset();
//    net->layers()[i].reset();

}

