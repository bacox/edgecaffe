//
// Created by bacox on 08/05/2020.
//

#include <iostream>
#include <ArrivalList.h>
#include <caffe/caffe.hpp>
#include <algorithm>    // std::sort
#include <Util/Output.h>


auto k(double val)
{
    static const double p=1.0 / std::sqrt( 2.0 * M_PI);

    return p * std::exp(- 0.5 * (val*val));
}

auto kde(double x, std::vector<double> val, double h)
{
    int len=val.size();
    const double p = 1.0 / (h * len );

    double sum = 0;
    for(int i=0;i<len;i++)
        sum += k( (x - val[i]) / h );
    return p*sum;
}

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

//    EdgeCaffe::ArrivalList al;
//    auto keys = al.getKeysAvailableNetworks();
//    for(auto &keyName : keys)
//        std::cout << "Key: " << keyName << std::endl;
//
//    al.setAllowedNetworks({"AgeNet", "GenderNet", "SoS", "SoS_GoogleNet", "FaceNet"});
//
//    al.generateList(100, EdgeCaffe::ArrivalList::NORMAL, {5,1});
//
////    al.printArrivals();
//
//    double min;
//    double max;
//    bool first = true;
//    int numberOfBars = 200;
//
//    std::vector<double> barValues;
//    for(auto a : al.arrivals)
//    {
//        if(first)
//        {
//            first = false;
//            min = max = a.time;
//        }
//        if(a.time < min)
//        {
//            min = a.time;
//        }
//        if(a.time > max)
//        {
//            max = a.time;
//        }
//        barValues.push_back(a.time);
//    }
//
//    std::sort(barValues.begin(), barValues.end());
//    double delta = (max - min) / (numberOfBars * 1.0);
//
//    std::vector<int> barPoints(numberOfBars, 0);
//
//    double upperCheck = delta;
//    int vectorLoc = 0;
//    for(auto bv : barValues)
//    {
//        if(bv > upperCheck)
//        {
//            upperCheck += delta;
//            vectorLoc++;
//        }
//        ++barPoints[vectorLoc];
//    }
//
////    int offset = min < 0 ? abs(min) : 0;
////    min = min = offset;
////    max = max + offset;
////    std::vector<int> barPoints(numberOfBars, 0);
////
////    for (auto a : al.arrivals) {
////        int value = int(a.time) + offset;
////        if ((value>=min)&&(value<max)) ++barPoints[value];
////    }
//
//
//
//    /* Plot distribution */
////    std::vector<int> test_data;
////    for (int i = 0; i < 20; i++) {
////        test_data.push_back(i);
////    }
//
//    plt::bar(barPoints);
//    plt::show();

    EdgeCaffe::Output out;

    std::string outputFile = "../analysis/hello/world/data.txt";

    out.toCSV(outputFile,{}, EdgeCaffe::Output::LAYER);

    const int n=100;
    std::vector<double> val(n);

    for(int i=0;i<n;i++)
        val[i] = (double)rand() / (1.0 + RAND_MAX);


    double x0=-10.0;
    double xN=10.0;
    int Nx=10000;
    double hx=(xN-x0)/(Nx-1);

    double integral=0.0;
    for(int i=0;i<Nx;++i)
    {
        double x=x0+i*hx;

        integral += hx * kde (x, val, 0.1);
    }

    std::cout<<"integral: "<<integral<<std::endl;

    return 0;
}