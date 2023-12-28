#include "simulation.hpp"
#include "reports.hpp"
#include <stdexcept>
#include <iostream>

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf){

    if (f.is_consistent()){
        SpecificTurnsReportNotifier notifier(d);
        Time t = 1;
        Time tk = t + turns*d;
        std::ostream& os = std::cout;
        while(t < tk){
            f.do_deliveries(t);
            f.do_work(t);
            f.do_package_passing();
            if (){
                generate_simulation_turn_report(f, os, t);
            }

            t += d;
        }

    }
    else {
        throw std::logic_error("Factory not consistent");
    }
}