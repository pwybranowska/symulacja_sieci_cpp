#include "reports.hpp"
#include "nodes.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

void generate_structure_report(const Factory& f, std::ostream& os){
    os <<"\n== LOADING RAMPS ==" << std::endl;

    auto ramp = [&os] (const Ramp& rmp) {
        os << std::endl << "LOADING RAMP #" << rmp.get_id() << std::endl;
        os << "  Delivery interval: " << rmp.get_delivery_interval() << std::endl;
        os << "  Receivers:" << std::endl;
        int worker_counter = 1;
        for (auto it = rmp.receiver_preferences_.begin(), end_it = rmp.receiver_preferences_.cend(); it != end_it; ++it){
            os << "    worker #" << worker_counter << std::endl;
            ++worker_counter;
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
        int worker_counter = 1;
        int storehouse_counter = 1;
        for (auto &receiver: wrkr.receiver_preferences_){
            if (receiver.first->get_receiver_type() == ReceiverType::WORKER){
                os << "    worker #" << worker_counter << std::endl;
                ++worker_counter;
            }
            else {
                os << "    storehouse #" << storehouse_counter << std::endl;
                ++storehouse_counter;
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

    std::vector<ElementID> workerz_id;
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it){
        workerz_id.push_back(it->get_id());
    }
    std::sort(workerz_id.begin(), workerz_id.end());


    for (ElementID id: workerz_id){
        auto current_worker = f.find_worker_by_id(id);
        os << std::endl << "WORKER #" << (*current_worker).get_id() << std::endl;

        auto &processed_buffer = (*current_worker).get_processing_buffer();
        Time processing_time = (t - (*current_worker).get_package_processing_start_time() + 1) % (*current_worker).get_processing_duration() ;
        if (processed_buffer == std::nullopt or processed_buffer->get_id() == 0 or processing_time <= 0) {
            os << "  PBuffer: (empty)" << std::endl;
        } else {
            os << "  PBuffer: #" << processed_buffer->get_id() << " (pt = " << processing_time << ")"
               << std::endl;
        }

        const auto &queue = (*current_worker).get_queue();
        os << "  Queue: ";
        if (queue->cbegin() != queue->cend()) {
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

        auto& sent_buffer = (*current_worker).get_sending_buffer();
        if (sent_buffer == std::nullopt or sent_buffer->get_id() <= 0) {
            os << "  SBuffer: (empty)" << std::endl;
        } else {
            os << "  SBuffer: #" << sent_buffer->get_id() << std::endl;
        }
    }
    


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