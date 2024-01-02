#include "simulation.hpp"
#include "reports.hpp"
#include <stdexcept>
#include <iostream>

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf){

    if (f.is_consistent()){

        Time t = 1;

        while(t <= d){
            f.do_deliveries(t);
            f.do_package_passing();
            f.do_work(t);
            rf(f,  t);


            t++;
        }

    }
    else {
        throw std::logic_error("Factory not consistent");
    }
}