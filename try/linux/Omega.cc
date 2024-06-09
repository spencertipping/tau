#include "../../tau.hh"
#include "../../sigma.hh"

#include "../../tau/begin.hh"

#include <iostream>
#include <random>
#include <cmath>


using namespace τ;
using namespace σ;


void verify(u64 o, u64 s, Ωhmc &hm)
{
  A(o == hm.offset(), "Ωhm o != offset: " << o << " != " << hm.offset());
  A(hm.size() >= s,   "Ωhm size < s: " << hm.size() << " < " << s);
  A(s <= 2 || hm.size() <= s * (1.0 + 1.0/32768),
    "Ωhm size > +0.003%: " << hm.size() << " > 1.003% * " << s);
  A(s <= 2 || hm.size() - s <= hm.size_error(),
    "Ωhm size - s > size_error: " << hm.size() << " - " << s << " > " << hm.size_error());
}


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
    for (u64 i = 0;; i++) {
        {
            // Generate exponential distributed random doubles
            double random_double1 = exp_dist1(generator);
            double random_double2 = exp_dist2(generator);

            // Convert the generated doubles to integers
            auto o = static_cast<τ::u64>(random_double1) & 0x0000'0fff'ffff'ffffull;
            auto s = static_cast<τ::u64>(random_double2) & 0x0000'0000'ffff'ffffull;

            verify(o, s, Ωhm(0, o, s));
        }

        {
            // Generate uniform distributed random integers
            auto o = unif_dist1(generator) & 0x0000'0fff'ffff'ffffull;
            auto s = unif_dist2(generator) & 0x0000'0000'ffff'ffffull;

            s >>= o & 0x0f;
            verify(o, s, Ωhm(0, o, s));
        }

        if (i % 1048576 == 0)
            std::cout << "Tested " << i << " case(s)" << std::endl;
    }

    return 0;
}
