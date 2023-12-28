#include "reports.hpp"
#include <iostream>
#include <string>

void generate_structure_report(const Factory& f, std::ostream& os){
    os << "== LOADING RAMPS ==" << std::endl;

    auto ramp = [&os] (const Ramp& rmp) {
        os << std::endl << "LOADING RAMP #" << rmp.get_id() << std::endl;
        os << "  Delivery interval: " << rmp.get_delivery_interval();
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
            s = "LIFO";
        }
        else if (wrkr.get_queue()->get_queue_type() == PackageQueueType::FIFO){
            s = "FIFO";
        }
        os << "  Queue type:" << s << std::endl;

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
        os << std::endl << "STOREHOUSE #" << store.get_id();
    };
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), str);


//    os << std::endl <<  std::endl << "; == LINKS ==" << std::endl;
//    auto from_ramp = [&os] (const Ramp& rmp){
//        for (auto& receiver: rmp.receiver_preferences_.get_preferences())
//            os << std::endl << "LINK src=ramp-" << rmp. get_id() << " dest=worker-" << receiver.first -> get_id();
//    };
//    std::for_each(f.ramp_cbegin(), f.ramp_cend(), from_ramp);

//    auto from_worker = [&os](const Worker& wrkr){
//        for (auto& receiver: wrkr.receiver_preferences_.get_preferences()){
//            if (receiver.first -> get_receiver_type() == ReceiverType::WORKER){
//                os << std::endl << "LINK src=worker-" << wrkr.get_id() << " dest=worker-" << receiver.first->get_id();
//            } else if (receiver.first -> get_receiver_type() == ReceiverType::STOREHOUSE){
//                os << std::endl << "LINK src=worker-" << wrkr.get_id() << " dest=store-" << receiver.first -> get_id();
//            }
//        }
//    };
//    std::for_each(f.worker_cbegin(), f.worker_cend(), from_worker);
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n" << std::endl;

    os << "== WORKERS ==" << std::endl;

    auto worker = [&os](const Worker &wrkr) {
        os << std::endl << "WORKER #" << wrkr.get_id() << std::endl;

        auto &processed_buffer = wrkr.get_processing_buffer();
        if (processed_buffer) {
            os << "  PBuffer: #" << processed_buffer->get_id() << " (pt = " << wrkr.get_processing_duration() << ")"
               << std::endl;
        } else {
            os << "  PBuffer: (empty)" << std::endl;
        }

        const auto &queue = wrkr.get_queue();
        os << "  Queue: ";
        if (!queue->empty()) {
            for (const auto &package: *queue) {
                os << "#" << package.get_id() << ", ";
            }
            os << std::endl;
        } else {
            os << "(empty)" << std::endl;
        }

        auto sent_buffer = wrkr.get_processing_buffer()->get_id();
        if (sent_buffer) {
            os << "  SBuffer: #" << sent_buffer << " (pt = " << wrkr.get_processing_duration() << ")" << std::endl;
        } else {
            os << "  SBuffer: (empty)" << std::endl;
        }

//        std::string s;
//        if (wrkr.get_queue()->get_queue_type() == PackageQueueType::LIFO){
//            s = "LIFO";
//        }
//        else if (wrkr.get_queue()->get_queue_type() == PackageQueueType::FIFO){
//            s = "FIFO";
//        }
//        os << "  Queue type:" << s << std::endl;
//
//        os << "  Receivers:" << std::endl;
//        int i = 1;
//        auto receiver = wrkr.receiver_preferences_.get_preferences(){
//            os << "    worker #" << i << std::endl;
//            i++;
//        }
//        std::for_each(.cbegin(), .cend(), receiver);
//    };
//    std::for_each(f.worker_cbegin(), f.worker_cend(), worker);


        os << std::endl << std::endl << "== STOREHOUSES ==" << std::endl;
        auto str = [&os](const Storehouse &store) {
            os << std::endl << "STOREHOUSE #" << store.get_id() << std::endl;
            os << "Stock: ";
            for (auto &st: store) {
                os << "#" << st.get_id();
                if (st.get_id() != store.cend()->get_id()) {
                    os << ", ";
                }
            }

//        auto elem = [&os] (const IPackageStockpile& st){
//            os << "#" << st
//        };
//        std::for_each(store.cbegin(), store.cend(), elem);
        };
        std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), str);
    }
}

