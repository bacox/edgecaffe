//
// Created by bacox on 26/04/2020.
//

#include <iostream>
#include <random>
#include <memory>
#include "ArrivalList.h"
void EdgeCaffe::ArrivalList::generateList(int numberOfArrivals, DISTRIBUTION_TYPE type, DistParam distributionParameters)
{
    // Units in miliseconds
    Distribution *distribution = nullptr;
    switch(type) {
        case DISTRIBUTION_TYPE::UNIFORM :
//            distribution(new UniformDist(distributionParameters);
            distribution = new UniformDist(distributionParameters);
            break;
        case DISTRIBUTION_TYPE::EXPONENTIAL :
            distribution = new ExpDist(distributionParameters);
            break;
        case DISTRIBUTION_TYPE::NORMAL:
            distribution = new NormalDist(distributionParameters);
            break;
    }
    //Use smart pointer to make sure that the pointer is clean up at the end of the function
    std::unique_ptr<Distribution> dist_ptr(distribution);

    // Get random number generator
    std::default_random_engine generator;

    // Define input network and data
    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToImg = "../resources/test_1.jpg";

    for( int i = 0; i < numberOfArrivals; ++i)
    {
        int delay = (int) distribution->getRandom(generator);
        arrivals.push_back(Arrival{delay, pathToImg, pathToAgeNet});
    }

    for(auto a : arrivals)
        std::cout << a.toString() << std::endl;
}

bool EdgeCaffe::ArrivalList::isEmpty()
{
    return arrivals.empty();
}

EdgeCaffe::Arrival &EdgeCaffe::ArrivalList::first()
{
    return arrivals.front();
}

void EdgeCaffe::ArrivalList::pop()
{
    arrivals.pop_front();
}

/**
 * Distibution implementations
 */


EdgeCaffe::Distribution::Distribution(const EdgeCaffe::DistParam &params) : params(params)
{}

EdgeCaffe::NormalDist::NormalDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::normal_distribution<double>(params.first,params.second);
}

double EdgeCaffe::NormalDist::getRandom(std::default_random_engine generator)
{
    return distribution(generator);
}

EdgeCaffe::UniformDist::UniformDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::uniform_real_distribution<double>(params.first,params.second);
}

double EdgeCaffe::UniformDist::getRandom(std::default_random_engine generator)
{
    return distribution(generator);
}

EdgeCaffe::ExpDist::ExpDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::exponential_distribution<double>(params.first);
}

double EdgeCaffe::ExpDist::getRandom(std::default_random_engine generator)
{
    return distribution(generator);
}
