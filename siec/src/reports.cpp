#include "reports.hpp"
#include <iostream>
#include <string>

void generate_structure_report(const Factory& f, std::ostream& os){
    os <<"\n== LOADING RAMPS ==" << std::endl;

    auto ramp = [&os] (const Ramp& rmp) {
        os << std::endl << "LOADING RAMP #" << rmp.get_id() << std::endl;
        os << "  Delivery interval: " << rmp.get_delivery_interval() << std::endl;
        os << "  Receivers:" << std::endl;
        int i = 1;
        for (auto it = rmp.receiver_preferences_.begin(), end_it = rmp.receiver_preferences_.cend(); it != end_it; ++it){
            os << "    worker #" << i << std::endl;
            ++i;
        }
    };
    std::for_each(f.ramp_cbegin(), f.ramp_cend(), ramp);


    os << std::endl << std::endl <<"== WORKERS ==" << std::endl;
    auto worker = [&os](const Worker& wrkr){
        os << std::endl << "WORKER #" << wrkr.get_id() << std::endl;
        os << "  Processing time: " << wrkr.get_processing_duration();
        std::string s;
        if (wrkr.get_queue()->get_queue_type() == PackageQueueType::LIFO){
            s = " LIFO";
        }
        else if (wrkr.get_queue()->get_queue_type() == PackageQueueType::FIFO){
            s = " FIFO";
        }
        os << "\n  Queue type:" << s << std::endl;

        os << "  Receivers:" << std::endl;

        for (auto &receiver: wrkr.receiver_preferences_){
            if (receiver.first->get_receiver_type() == ReceiverType::WORKER){
                os << "    worker #" << receiver.first->get_id() << std::endl;
            }
            else {
                os << "    storehouse #" << receiver.first->get_id() << std::endl;
            }
        }
    };
    std::for_each(f.worker_cbegin(), f.worker_cend(), worker);


    os << std::endl << std::endl << "== STOREHOUSES ==" << std::endl;
    auto str = [&os] (const Storehouse& store){
        os << "\nSTOREHOUSE #" << store.get_id() << "\n";
    };
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), str);
    os << "\n";
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n" << std::endl;

    os << "== WORKERS ==" << std::endl;

    auto worker = [&os] (const Worker &wrkr) {
        os << std::endl << "WORKER #" << wrkr.get_id() << std::endl;

        auto &processed_buffer = wrkr.get_processing_buffer();
        if (processed_buffer != std::nullopt) {
            os << "  PBuffer: #" << processed_buffer->get_id() << " (pt = " << wrkr.get_processing_duration() - wrkr.get_package_processing_start_time() << ")"
               << std::endl;
        } else {
            os << "  PBuffer: (empty)" << std::endl;

        }

        const auto &queue = wrkr.get_queue();
        os << "  Queue: ";
        if (!queue->empty()) {
            for (const auto &package: *queue) {
                os << "#" << package.get_id();
                if (package.get_id() != queue->cend()->get_id()) {
                    os << ", ";
                }
            }
            os << std::endl;
        } else {
            os << "(empty)" << std::endl;
        }

        auto& sent_buffer = wrkr.get_processing_buffer();
        if (sent_buffer != std::nullopt) {
            os << "  SBuffer: #" << sent_buffer->get_id() << std::endl;

        } else {
            os << "  SBuffer: (empty)" << std::endl;
        }


    };
        std::for_each(f.worker_cbegin(), f.worker_cend(), worker);


        os << std::endl << std::endl << "== STOREHOUSES ==" << std::endl;
        auto str = [&os](const Storehouse &store) {
            os << std::endl << "STOREHOUSE #" << store.get_id();
            os << "\n  Stock: ";
            if (store.cbegin() == store.cend()){
                os << "(empty)";
            }
            else {
                for (auto &package: store) {
                    os << "#" << package.get_id();
                    if (package.get_id() != store.cend()->get_id()) {
                        os << ", ";
                    }
                }
            }
            os << "\n";

        };
        std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), str);
        os << "\n";
}