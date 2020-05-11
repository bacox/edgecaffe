//
// Created by bacox on 26/04/2020.
//

#ifndef EDGECAFFE_ARRIVALLIST_H
#define EDGECAFFE_ARRIVALLIST_H
#include <string>
#include <deque>
#include <random>
#include <map>

namespace EdgeCaffe
{
    /**
     * Unified data structure to hold one or more parameters for stochastic distributions
     */
    struct DistParam {
        double first = 0;
        double second = 0;
    };

    /**
     * Wrapper class to create a uniform class type to the C++ distribution classes
     * The Distribution class is a abstract class
     */
    class Distribution {
        DistParam params;
    public:
        Distribution(const DistParam &params);

        /**
         * Generates a random value depending on the underlying distribution
         *
         * Side note: The stupidly poorly documented copy-constructor of the linear_congruential_engine did cost me
         * the better part of 4 hours! If the standard library resets the damn seed by using the copy constructor, it
         * really is something that should be in the docs!
         * Without using a reference the generator will be created with a new seed! For random generators always
         * use pass by reference or pointers; never pass by value!
         * @param generator
         * @return double - generated random value
         */
        virtual double getRandom(std::default_random_engine &generator) =0;
    };

    /**
     * Normal Distribution
     */
    class NormalDist : public Distribution{
        std::normal_distribution<double> distribution;
    public:
        /**
         * Constructor
         * @param params    DistParam - hold the mu (first) and sigma (second) parameters of the normal distribution. For example NormalDist({mu, sigma})
         */
        NormalDist(const DistParam &params);
        /**
         * Generates a random value depending from the normal distribution
         * @param generator
         * @return double - generated random value
         */
        double getRandom(std::default_random_engine &generator) override;
    };
    /**
     * Uniform distribution
     */
    class UniformDist : public Distribution {
        std::uniform_int_distribution<int> distribution;
    public:
        /**
         * Constructor
         * @param params    DistParam - hold the alpha (first) and beta (second) parameters of the uniform distribution. For example NormalDist({alpha, beta})
         */
        UniformDist(const DistParam &params);
        /**
         * Generates a random value depending on the uniform distribution
         * @param generator
         * @return double - generated random value
         */
        double getRandom(std::default_random_engine &generator) override;
    };

    /**
     * Uniform distribution
     */
    class UniformRealDist : public Distribution {
        std::uniform_real_distribution<double> distribution;
    public:
        /**
         * Constructor
         * @param params    DistParam - hold the alpha (first) and beta (second) parameters of the uniform distribution. For example NormalDist({alpha, beta})
         */
        UniformRealDist(const DistParam &params);
        /**
         * Generates a random value depending on the uniform distribution
         * @param generator
         * @return double - generated random value
         */
        double getRandom(std::default_random_engine &generator) override;
    };

    /**
     * Exponential distribution
     */
    class ExpDist : public Distribution {
        std::exponential_distribution<double> distribution;
    public:
        /**
         * Constructor
         * @param params    DistParam - hold the lambda (first) parameter of the exponential distribution. For example NormalDist({lambda})
         */
        ExpDist(const DistParam &params);

        /**
         * Generates a random value depending on the exponential distribution
         * @param generator
         * @return double - generated random value
         */
        double getRandom(std::default_random_engine &generator) override;
    };


    /**
     * Data structure used in the ArrivalList to describe an Arrival
     * It hold a location to input data and the location of the network to be used.
     */
    struct Arrival {
        long time = 0;
        std::string pathToData;
        std::string pathToNetwork;
        std::string networkName;

        std::string toString(){
            return "Arrival<time: " + std::to_string(time)+ ">";
        }
    };

    /**
     * Arrival list can be used to generate a list of arrivals based on a stochastic distribution.
     * Currently the uniform distribution is provided.
     *
     */
    class ArrivalList
    {
    public:
        // Queue structure to hold the variables.
        std::deque<Arrival> arrivals;

    public:

        enum DISTRIBUTION_TYPE {
            UNIFORM,
            NORMAL,
            EXPONENTIAL
        };
        /**
         * Generate a `numberOfArrivals` based on a stochastic distribution
         * The distribution will generate the time between arrivals.
         * So the time is the relative time in relation to the previous arrival, not the absolute time.
         * @param numberOfArrivals  Int - The number of arrivals to be generated
         */
        void generateList(int numberOfArrivals, DISTRIBUTION_TYPE type = UNIFORM, DistParam distributionParameters = {1000,2000});

        std::map <std::string, std::string> networks {
                { "AgeNet", "networks/AgeNet" }
                ,{ "GenderNet", "networks/GenderNet" }
                ,{ "SoS", "networks/SoS" }
                ,{ "SoS_GoogleNet", "networks/SoS_GoogleNet" }
                ,{ "FaceNet", "networks/FaceNet" }
        };

        std::vector<std::string> allowedNetworks;

        std::vector<std::string> getKeysAvailableNetworks();

        bool isEmpty();

        void setAllowedNetworks(std::vector<std::string> keys);

        void printArrivals();

        /**
         * Gets the next arrival
         * @return Arrival object
         *
         * Usage:
         *  ArrivalList list;
         *  auto nextArrival = list.first();
         *  list.pop();
         */
        Arrival& first();

        /**
         * Removes the first element of the queue
         */
        void pop();


        std::vector<std::string> toCSVLines();
    };
}


#endif //EDGECAFFE_ARRIVALLIST_H
