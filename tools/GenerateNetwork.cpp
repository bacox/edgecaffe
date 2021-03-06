//
// Created by bacox on 21/04/2020.
//

#include <iostream>
#include <caffe/net.hpp>
#include <caffe/caffe.hpp>
#include <opencv2/core/types.hpp>




int main(int argc, char *argv[])
{

    cv::Size inputDim(227,227);

    int numConvLayers = 5;
    int numFcLayers = 5;



    std::cout << "Generating network" << std::endl;
    std::string pathToProtoText = "../networks/AgeNet/age_deploy.prototxt";
    std::string output = "generated-model.caffemodel";

    caffe::Net<float> network(pathToProtoText, caffe::Phase::TEST);
    caffe::NetParameter net_param;
    network.ToProto(&net_param, false);
    caffe::WriteProtoToBinaryFile(net_param, output.c_str());
    std::cout << "End of program" << std::endl;
}