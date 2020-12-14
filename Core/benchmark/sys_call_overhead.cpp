//
// Created by bacox on 05/10/2020.
//
#include <iostream>
#include <caffe/caffe.hpp>
#include <chrono>
#include <numeric>


void loadNetParameter(const std::string& path)
{
    caffe::NetParameter param;
    caffe::ReadNetParamsFromBinaryFileOrDie(path, &param);
}

long loadParamFile(const std::string& path)
{
    auto start_tp = std::chrono::high_resolution_clock::now();
    loadNetParameter(path);
    auto end_tp = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();
    return elapsed_time;
}

long loadParamFiles(const std::vector<std::string>& paths)
{
    auto start_tp = std::chrono::high_resolution_clock::now();
    for(auto path : paths)
    {
        loadNetParameter(path);
    }
    auto end_tp = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();
    return elapsed_time;
}


std::vector<long> benchmarkSingle(int repetitions)
{
//    const std::string path = "../../networks/ObjectNet/VGG_ILSVRC_19_layers.caffemodel";
//    const std::string path = "../../networks/SoS_GoogleNet/GoogleNet_SOS.caffemodel";
    const std::string path = "../../networks/TinyYolo/tiny_yolo.caffemodel";
    std::vector<long> durations;
    durations.reserve(repetitions);
    for (int i = 0; i < repetitions; ++i)
    {
        auto duration = loadParamFile(path);
        durations.push_back(duration);
    }
    return durations;
}

std::vector<long> benchmarkMultiple(int repetitions)
{
//    const std::vector<std::string> paths = {
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS00.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS01.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS02.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS03.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS04.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS05.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS06.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS07.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS08.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS09.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS10.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS11.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS12.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS13.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS14.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS15.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS16.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS17.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS18.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS19.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS20.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS21.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS22.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS23.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS24.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS25.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS26.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS27.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS28.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS29.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS30.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS31.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS32.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS33.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS34.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS35.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS36.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS37.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS38.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS39.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS40.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS41.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS42.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS43.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS44.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS45.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS46.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS47.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS48.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS49.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS50.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS51.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS52.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS53.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS54.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS55.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS56.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS57.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS58.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS59.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS60.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS61.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS62.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS63.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS64.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS65.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS66.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS67.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS68.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS69.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS70.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS71.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS72.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS73.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS74.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS75.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS76.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS77.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS78.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS79.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS80.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS81.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS82.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS83.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS84.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS85.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS86.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS87.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS88.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS89.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS90.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS91.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS92.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS93.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS94.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS95.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS96.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS97.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS98.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS99.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS100.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS101.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS102.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS103.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS104.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS105.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS106.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS107.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS108.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS109.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS110.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS111.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS112.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS113.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS114.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS115.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS116.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS117.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS118.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS119.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS120.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS121.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS122.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS123.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS124.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS125.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS126.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS127.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS128.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS129.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS130.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS131.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS132.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS133.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS134.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS135.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS136.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS137.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS138.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS139.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS140.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS141.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS142.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS143.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS144.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS145.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS146.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS147.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS148.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS149.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS150.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS151.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS152.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS153.partial.caffemodel",
//            "../../networks/SoS_GoogleNet/GoogleNet_SOS_partials/GoogleNet_SOS154.partial.caffemodel"
//    };
//    const std::vector<std::string> paths = {
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers00.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers01.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers02.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers03.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers04.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers05.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers06.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers07.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers08.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers09.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers10.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers11.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers12.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers13.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers14.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers15.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers16.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers17.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers18.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers19.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers20.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers21.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers22.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers23.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers24.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers25.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers26.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers27.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers28.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers29.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers30.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers31.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers32.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers33.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers34.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers35.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers36.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers37.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers38.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers39.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers40.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers41.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers42.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers43.partial.caffemodel",
//            "../../networks/ObjectNet/VGG_ILSVRC_19_layers_partials/VGG_ILSVRC_19_layers44.partial.caffemodel"
//    };
    const std::vector<std::string> paths = {
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo00.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo01.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo02.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo03.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo04.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo05.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo06.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo07.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo08.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo09.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo10.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo11.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo12.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo13.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo14.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo15.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo16.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo17.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo18.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo19.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo20.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo21.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo22.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo23.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo24.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo25.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo26.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo27.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo28.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo29.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo30.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo31.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo32.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo33.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo34.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo35.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo36.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo37.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo38.partial.caffemodel",
            "../../networks/TinyYolo/tiny_yolo_partials/tiny_yolo39.partial.caffemodel",
    };
    std::vector<long> durations;
    durations.reserve(repetitions);
    for (int i = 0; i < repetitions; ++i)
    {
        auto duration = loadParamFiles(paths);
        durations.push_back(duration);
    }
    return durations;
}


std::tuple<double, double> vectorStats(const std::vector<long>& v)
{

    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();

    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(), [mean](double x) { return x - mean; });
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / v.size());

    return std::make_tuple(mean, stdev);
}


void printConverted(double ns, const std::string typeName)
{
    double us = ns / 1000;
    double ms = us / 1000;
    double sec = ms / 1000;
    std::cout << typeName << ":" << std::endl;
    std::cout << "\t" << us << " us" << std::endl;
    std::cout << "\t" << ms << " ms" << std::endl;
    std::cout << "\t" << sec << " secs" << std::endl;
}

int main(int argc, char *argv[]) {

    if(argc <= 1)
    {
        std::cerr << "Missing parameter!" << std::endl;
        exit(1);
    }
    std::cout << "Run this with the tool 'strace -c <progam>' to measure the system calls" << std::endl;
    std::string mode = argv[1];
    bool single = false;
    if(mode == "single"){
        single = true;
    }
//    std::cout << "The mode that is being used is: '"<< (single? "single": "multiple") << "'" << std::endl;

    ::google::InitGoogleLogging(argv[0]);
    int repetitions = 500;
    std::cout << "Sys-call overhead test started with " << repetitions << " repetitions, mode: " << (single? "single": "multiple") << std::endl;

    if(single)
    {
        auto durations_single = benchmarkSingle(repetitions);
        auto [single_mean, single_stdev] = vectorStats(durations_single);
        std::cout << "The single vector has a mean of " << single_mean << " ns and a stdev of " << single_stdev << std::endl;
        printConverted(single_mean, "Mean");
        printConverted(single_stdev, "Stdev");
    }else {

        auto durations_multiple = benchmarkMultiple(repetitions);
        auto [multiple_mean, multiple_stdev] = vectorStats(durations_multiple);
        std::cout << "The multiple vector has a mean of " << multiple_mean << " ns and a stdev of " << multiple_stdev << std::endl;
        printConverted(multiple_mean, "Mean");
        printConverted(multiple_stdev, "Stdev");
    }
    std::cout << "Starting sys call benchmark" << std::endl;
    return 0;
}