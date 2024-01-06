#include "reports.hpp"
#include "nodes.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

void generate_structure_report(const Factory& f, std::ostream& os){
    os <<"\n== LOADING RAMPS ==" << std::endl;

    std::vector<ElementID> rampz_id;
    for (auto it = f.ramp_cbegin(); it != f.ramp_cend(); ++it){
        rampz_id.push_back(it->get_id());
    }
    std::sort(rampz_id.begin(), rampz_id.end());
    for (auto id: rampz_id){
        auto current_ramp = f.find_ramp_by_id(id);
        os << std::endl << "LOADING RAMP #" << (*current_ramp).get_id() << std::endl;
        os << "  Delivery interval: " << (*current_ramp).get_delivery_interval() << std::endl;
        os << "  Receivers:" << std::endl;
        int worker_counter = 1;
        for (auto it = (*current_ramp).receiver_preferences_.begin(), end_it = (*current_ramp).receiver_preferences_.cend(); it != end_it; ++it){
            os << "    worker #" << worker_counter << std::endl;
            ++worker_counter;
        }
    }

    os << std::endl << std::endl <<"== WORKERS ==" << std::endl;

    std::vector<ElementID> workerz_id;
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it){
        workerz_id.push_back(it->get_id());
    }
    std::sort(workerz_id.begin(), workerz_id.end());
    for (ElementID id: workerz_id){
        auto current_worker = f.find_worker_by_id(id);
        os << std::endl << "WORKER #" << (*current_worker).get_id() << std::endl;
        os << "  Processing time: " << (*current_worker).get_processing_duration();
        std::string s;
        if ((*current_worker).get_queue()->get_queue_type() == PackageQueueType::LIFO){
            s = " LIFO";
        }
        else if ((*current_worker).get_queue()->get_queue_type() == PackageQueueType::FIFO){
            s = " FIFO";
        }
        os << "\n  Queue type:" << s << std::endl;

        os << "  Receivers:" << std::endl;
        int worker_counter = 1;
        int storehouse_counter = 1;
        for (auto &receiver: (*current_worker).receiver_preferences_){
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



    os << std::endl << std::endl << "== STOREHOUSES ==" << std::endl;
    std::vector<ElementID> storehouze_id;
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it){
        storehouze_id.push_back(it->get_id());
    }
    std::sort(storehouze_id.begin(), storehouze_id.end());

    for (ElementID id: storehouze_id){
        auto current_storehouze = f.find_storehouse_by_id(id);
        os << "\nSTOREHOUSE #" << (*current_storehouze).get_id() << "\n";
    };
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
        if (!sent_buffer.has_value() or sent_buffer->get_id() <= 0) {
            os << "  SBuffer: (empty)" << std::endl;
        } else {
            os << "  SBuffer: #" << sent_buffer->get_id() << std::endl;
        }
//        if ((*current_worker).get_sending_buffer().has_value()){
//            os << "  SBuffer: #" << (*current_worker).get_sending_buffer()->get_id() << std::endl;
//        } else {
//            os << "  SBuffer: (empty)" << std::endl;
//        }
    }

    std::vector<ElementID> storehouze_id;
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it){
        storehouze_id.push_back(it->get_id());
    }
    std::sort(storehouze_id.begin(), storehouze_id.end());
    os << std::endl << std::endl << "== STOREHOUSES ==" << std::endl;

        for (ElementID id: storehouze_id){
            auto current_storehouze = f.find_storehouse_by_id(id);
            os << std::endl << "STOREHOUSE #" << (*current_storehouze).get_id();
            os << "\n  Stock: ";
            if ((*current_storehouze).cbegin() == (*current_storehouze).cend()){
                os << "(empty)";
            }
            else {
                for (auto &package: (*current_storehouze)) {
                    os << "#" << package.get_id();
                    if (package.get_id() != (*current_storehouze).cend()->get_id()) {
                        os << ", ";
                    }
                }
            }
            os << "\n";

        };
        os << "\n";
}