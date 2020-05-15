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
            distribution = new UniformDist(distributionParameters);
            break;
        case DISTRIBUTION_TYPE::EXPONENTIAL :
            distribution = new ExpDist(distributionParameters);
            break;
        case DISTRIBUTION_TYPE::NORMAL:
            distribution = new NormalDist(distributionParameters);
            break;
        case DISTRIBUTION_TYPE::POISSON:
            distribution = new PoissonDist(distributionParameters);
            break;
    }
    //Use smart pointer to make sure that the pointer is clean up at the end of the function
    std::unique_ptr<Distribution> dist_ptr(distribution);

    if(seed < 0)
    {
        // No seed is provided by the user so generate a new one
        std::random_device r_device;
        seed = r_device();
    }

    static std::default_random_engine generator(seed);

    // If no networks are set use all the network
    if(allowedNetworks.size() == 0)
    {
        for (auto & kvp : networks)
        {
            allowedNetworks.push_back(kvp.first);
        }
    }

    // Use uniform distribution to sample evenly from available networks
    UniformDist networkDist = UniformDist({0, (double)allowedNetworks.size() - 1});

    // For now use a static input
    std::string pathToImg = "../resources/test_1.jpg";

    // Generate the arrivals
    for( int i = 0; i < numberOfArrivals; ++i)
    {
        // Get the inter-arrival time for the next arrival
        int delay = (int) distribution->getRandom(generator);

        // Sample from available networks
        std::string networkKey = allowedNetworks[networkDist.getRandom(generator)];
        std::string networkPath = networks[networkKey];
        arrivals.push_back(Arrival{delay, pathToImg, networkPath, networkKey});
    }

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

std::vector<std::string> EdgeCaffe::ArrivalList::getKeysAvailableNetworks()
{
    std::vector<std::string> keys;
    for (auto & kvp : networks)
    {
        keys.emplace_back(kvp.first);
    }
    return keys;
}

void EdgeCaffe::ArrivalList::setAllowedNetworks(std::vector<std::string> keys)
{
    bool invalidKeys = false;
    for(auto &key : keys)
    {
        if(networks.count(key) == 0)
        {
            std::cerr << "Provided key '" << key << "' not found!" << std::endl;
            invalidKeys = true;
            continue;
        }
        allowedNetworks.push_back(key);
    }

    if(invalidKeys)
        throw std::invalid_argument( "Network(s) not found" );
}

void EdgeCaffe::ArrivalList::printArrivals()
{
        for(auto a : arrivals)
            std::cout << a.toString() << "\t\t" << a.pathToNetwork << std::endl;
}

std::vector<std::string> EdgeCaffe::ArrivalList::toCSVLines()
{
    std::vector<std::string> lines;
    for(auto a : arrivals)
        lines.push_back(std::to_string(a.time) + "," + a.networkName);

    return lines;
}

void EdgeCaffe::ArrivalList::setSeed(long seed)
{
    this->seed = (long) seed;
}

long EdgeCaffe::ArrivalList::getSeed() const
{
    if(seed < 0)
        return 0;
    return seed;
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

double EdgeCaffe::NormalDist::getRandom(std::default_random_engine &generator)
{
    return distribution(generator);
}

EdgeCaffe::UniformDist::UniformDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::uniform_int_distribution<int>((int)params.first,(int)params.second);
}

double EdgeCaffe::UniformDist::getRandom(std::default_random_engine &generator)
{
    return distribution(generator);
}

EdgeCaffe::ExpDist::ExpDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::exponential_distribution<double>(params.first);
}

double EdgeCaffe::ExpDist::getRandom(std::default_random_engine &generator)
{
    return distribution(generator);
}

EdgeCaffe::UniformRealDist::UniformRealDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::uniform_real_distribution<double>(params.first,params.second);
}

double EdgeCaffe::UniformRealDist::getRandom(std::default_random_engine &generator)
{
    return distribution(generator);
}

EdgeCaffe::PoissonDist::PoissonDist(const EdgeCaffe::DistParam &params) : Distribution(params)
{
    distribution = std::poisson_distribution<int>(params.first);
}

double EdgeCaffe::PoissonDist::getRandom(std::default_random_engine &generator)
{
    return distribution(generator);
}
