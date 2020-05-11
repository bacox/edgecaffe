//
// Created by bacox on 08/05/2020.
//

#include <iostream>
#include <EdgeCaffe.h>
#include <caffe/caffe.hpp>

int main(int argc, char *argv[])
{

//    std::string protoFile = "../networks/AgeNet/age_deploy.prototxt";
    std::cout << "Running TaskFromDescription" << std::endl;
//    caffe::NetParameter param;
//    caffe::ReadNetParamsFromTextFileOrDie(protoFile, &param);
//    int num_layers = param.layer_size();
//    for (int layer_id = 0; layer_id < num_layers; ++layer_id) {
//        const caffe::LayerParameter& layer_param = param.layer(layer_id);
//        std::cout << "Got layer: " << layer_param.name() << std::endl;
//
//    }
//    std::cout << "File is read" << std::endl;

    EdgeCaffe::ArrivalList al;
    auto keys = al.getKeysAvailableNetworks();
    for(auto &keyName : keys)
        std::cout << "Key: " << keyName << std::endl;

    al.setAllowedNetworks({"AgeNet", "GenderNet", "SoS", "SoS_GoogleNet", "FaceNet"});

    al.generateList(50, EdgeCaffe::ArrivalList::UNIFORM, {1000,15;00});

    al.printArrivals();



    return 0;
}