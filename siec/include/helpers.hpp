//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_HELPERS_HPP
#define SIEC_HELPERS_HPP

#include <functional>
#include <random>

#include "types.hpp"

extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

extern ProbabilityGenerator probability_generator;

#endif //SIEC_HELPERS_HPP
