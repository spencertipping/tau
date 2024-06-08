#include "../../tau.hh"
#include "../../sigma.hh"

#include <iostream>
#include <random>
#include <cmath>

int main() {
    // Seed for the random number generator
    std::random_device rd;
    std::mt19937 generator(rd());

    // Parameters for the exponential distribution
    // You can adjust the lambda parameters to control the 'rate' of the distribution.
    double lambda1 = 0.0000001;  // Mean of 10000000
    double lambda2 = 0.00001;    // Mean of 100000

    // Exponential distribution instances with the chosen lambdas
    std::exponential_distribution<double> exp_dist1(lambda1);
    std::exponential_distribution<double> exp_dist2(lambda2);

    std::uniform_int_distribution<uint64_t> unif_dist1;
    std::uniform_int_distribution<uint64_t> unif_dist2;

    // Infinite loop to generate and print pairs of random integers
    while (true) {
        {
            // Generate exponential distributed random doubles
            double random_double1 = exp_dist1(generator);
            double random_double2 = exp_dist2(generator);

            // Convert the generated doubles to integers
            auto o = static_cast<τ::u64>(random_double1);
            auto s = static_cast<τ::u64>(random_double2);

            std::cout << o << " " << s << " "
                      << σ::Ωhm(0,
                                o & 0x0000'0fff'ffff'ffffull,
                                s & 0x0000'0000'ffff'ffffull)
                      << std::endl;
        }

        {
            // Generate uniform distributed random integers
            auto o = unif_dist1(generator) & 0x0000'0fff'ffff'ffffull;
            auto s = unif_dist2(generator) & 0x0000'0000'ffff'ffffull;

            std::cout << std::hex << "0x" << o << " 0x" << s
                      << std::dec << " "
                      << σ::Ωhm(0, o, s)
                      << std::endl;
        }
    }

    return 0;
}
