#ifndef SIEC_REPORTS_HPP
#define SIEC_REPORTS_HPP
#include "factory.hpp"
#include "types.hpp"

void generate_structure_report(const Factory& f, std::ostream& os);
void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

#endif //SIEC_REPORTS_HPP
