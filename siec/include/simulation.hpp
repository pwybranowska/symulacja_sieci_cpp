#ifndef SIEC_SIMULATION_HPP
#define SIEC_SIMULATION_HPP
#include "factory.hpp"
#include "types.hpp"
#include <functional>

class IntervalReportNotifier{
    IntervalReportNotifier(TimeOffset to) : to_(to) {}
    bool should_generate_report(Time t) { return (t - 1)% to_ == 0; }
private:
    TimeOffset to_;
};

class SpecificTurnsReportNotifier{
    SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(turns) {}
    bool should_generate_report(Time t) {return turns_.count(t) > 0;}
private:
    std::set<Time> turns_;
};

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf);

#endif //SIEC_SIMULATION_HPP
