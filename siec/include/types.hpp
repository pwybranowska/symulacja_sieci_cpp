//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_TYPES_HPP
#define SIEC_TYPES_HPP

#include <functional>


using ElementID = unsigned long long;
using Time = unsigned long long;

using TimeOffset = unsigned long long;

using ProbabilityGenerator = std::function<double()>;

#endif //SIEC_TYPES_HPP
