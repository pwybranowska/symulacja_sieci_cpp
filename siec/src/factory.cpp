#include "factory.hpp"
#include "types.hpp"
#include "config.hpp"
#include <map>
#include <stdexcept>
#include <sstream>

template<class Node>
void NodeCollection<Node>::remove_by_id(ElementID id){
    auto elem = find_by_id((id));
    if (elem != nodes_.end()){
        nodes_.erase(elem);
    }
}

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if (node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()){
        throw std::logic_error("Sender has no receivers");
    }
    bool sender_has_any_receivers_other_than_self = false;
    for (auto &receiver: sender->receiver_preferences_){
        if (receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            sender_has_any_receivers_other_than_self = true;
        }
        else if (receiver.first->get_receiver_type() == ReceiverType::WORKER){
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendrecv_ptr == sender){
                continue;
            }
            sender_has_any_receivers_other_than_self = true;
            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if (sender_has_any_receivers_other_than_self){
        return true;
    }
    else {
        throw std::logic_error("Sender has no receivers");
    }
};

bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;
    for (auto &ramp: ramps_){
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (auto &worker: workers_){
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    try {
        for (auto &ramp: ramps_){
            has_reachable_storehouse(&ramp, node_colors);
        }
    }
    catch (const std::logic_error& e) {
        return false;
    }
    return true;
};


void Factory::do_deliveries(Time t) {
    for(auto& ramp: ramps_){
        ramp.deliver_goods(t);
    }
};

void Factory::do_work(Time t) {
    for (auto& worker : workers_){
        worker.do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : ramps_){
        ramp.send_package();
    }
}

template<class Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    auto node_it = collection.find_by_id(id);
    if (node_it != collection.end()){
        IPackageReceiver* id_ptr = dynamic_cast<IPackageReceiver*>(&(*node_it));
        for (auto &worker: workers_){
            worker.receiver_preferences_.remove_receiver(id_ptr);
        }
        for (auto& ramp: ramps_) {
            ramp.receiver_preferences_.remove_receiver(id_ptr);
        }
    }
}

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
    workers_.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(storehouses_, id);
    storehouses_.remove_by_id(id);
}


ParsedLineData parse_line(std::string line) {
   ParsedLineData parsed_data;

   std::vector<std::string> tokens;
   std::string token;
   std::istringstream token_stream(line);

   while (std::getline(token_stream, token, ' ')) {
       tokens.push_back(token);
   }
    if (tokens[0] == "LOADING_RAMP") {
        parsed_data.element_type = ElementType::RAMP;
    } else if (tokens[0] == "WORKER") {
        parsed_data.element_type = ElementType::WORKER;
    } else if (tokens[0] == "STOREHOUSE") {
        parsed_data.element_type = ElementType::STOREHOUSE;
    } else if (tokens[0] == "LINK") {
        parsed_data.element_type = ElementType::LINK;
    }

   std::string str;
   for (auto s = tokens.begin() + 1; s != tokens.end(); s++){
       std::istringstream tok_s(*s);
       std::vector<std::string> vect_s;
       while (std::getline(tok_s, str, '=')){
           vect_s.push_back(str);
       }
       parsed_data.mapped.insert(std::make_pair(vect_s[0], vect_s[1]));
   }
   return parsed_data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;
    while (std::getline(is, line)) {
        if(line[0] == ';' or line.empty()){
            continue;
        }
        auto parsed = parse_line(line);
        if (parsed.element_type == RAMP) {
            ElementID id = std::stoi(parsed.mapped["id"]);
            TimeOffset di = std::stoi(parsed.mapped["delivery-interval"]);
            factory.add_ramp(Ramp(id, di));
        }
        else if (parsed.element_type == WORKER) {
            ElementID id = std::stoi(parsed.mapped["id"]);
            TimeOffset di =  std::stoi(parsed.mapped["processing-time"]);
            PackageQueueType package_type;
            if (parsed.mapped["queue-type"] == "LIFO") {
                package_type = PackageQueueType::LIFO;
            } else if (parsed.mapped["queue-type"] == "FIFO") {
                package_type = PackageQueueType::FIFO;
            }

            factory.add_worker(Worker(id, di, std::make_unique<PackageQueue>(PackageQueueType(package_type))));
        }
        else if (parsed.element_type == STOREHOUSE){
            ElementID id = std::stoi(parsed.mapped["id"]);
            factory.add_storehouse(Storehouse(id));
        }
        else if (parsed.element_type == LINK){
                std::vector<std::string> senders;
                std::vector<std::string> receivers;
                std::string token;
                std::istringstream token_sender(parsed.mapped["src"]);
                while (std::getline(token_sender, token, '-')){
                    senders.push_back(token);
                }
                std::istringstream token_receivers(parsed.mapped["dest"]);
                while (std::getline(token_receivers, token, '-')){
                    receivers.push_back(token);
                }

                if(senders[0] == "ramp" and receivers[0] == "worker"){
                    auto parsed_sender = factory.find_ramp_by_id(std::stoi(senders[1]));
                    auto parsed_receiver = factory.find_worker_by_id(std::stoi(receivers[1]));
                    parsed_sender -> receiver_preferences_.add_receiver(&*parsed_receiver);
                }
                else if(senders[0] == "worker" and receivers[0] == "worker"){
                    auto parsed_sender = factory.find_worker_by_id(std::stoi(senders[1]));
                    auto parsed_receiver = factory.find_worker_by_id(std::stoi(receivers[1]));
                    parsed_sender -> receiver_preferences_.add_receiver(&*parsed_receiver);
                }

                else if(senders[0] == "worker" and receivers[0] == "store"){
                    auto parsed_sender = factory.find_worker_by_id(std::stoi(senders[1]));
                    auto parsed_receiver = factory.find_storehouse_by_id(std::stoi(receivers[1]));
                    parsed_sender -> receiver_preferences_.add_receiver(&*parsed_receiver);
                }
                else if(senders[0] == "ramp" and receivers[0] == "store"){
                    auto parsed_sender = factory.find_ramp_by_id(std::stoi(senders[1]));
                    auto parsed_receiver = factory.find_storehouse_by_id(std::stoi(receivers[1]));
                    parsed_sender -> receiver_preferences_.add_receiver(&*parsed_receiver);
                }
        }
    }
    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os) {
    os << "; == LOADING RAMPS ==" << std::endl;
    auto ramp = [&os] (const Ramp& rmp){
        os << std::endl << "LOADING_RAMP id=" << rmp.get_id();
        os << " delivery-interval=" << rmp.get_delivery_interval();
    };
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), ramp);

    os << std::endl << std::endl <<"; == WORKERS ==" << std::endl;
    auto worker = [&os](const Worker& wrkr){
        os << std::endl << "WORKER id=" << wrkr.get_id();
        os << " processing-time=" << wrkr.get_processing_duration();
        std::string s;
        if (wrkr.get_queue()->get_queue_type() == PackageQueueType::LIFO){
            s = "LIFO";
        }
        else if (wrkr.get_queue()->get_queue_type() == PackageQueueType::FIFO){
            s = "FIFO";
        }
        os << " queue-type=" << s;
    };
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), worker);

    os << std::endl << std::endl << "; == STOREHOUSES ==" << std::endl;
    auto str = [&os] (const Storehouse& store){
        os << std::endl << "STOREHOUSE id=" << store.get_id();
    };
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), str);


    os << std::endl <<  std::endl << "; == LINKS ==" << std::endl;
    auto from_ramp = [&os] (const Ramp& rmp){
        for (auto& receiver: rmp.receiver_preferences_.get_preferences())
            os << std::endl << "LINK src=ramp-" << rmp. get_id() << " dest=worker-" << receiver.first -> get_id();
    };
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), from_ramp);

    auto from_worker = [&os](const Worker& wrkr){
        for (auto& receiver: wrkr.receiver_preferences_.get_preferences()){
            if (receiver.first -> get_receiver_type() == ReceiverType::WORKER){
                os << std::endl << "LINK src=worker-" << wrkr.get_id() << " dest=worker-" << receiver.first->get_id();
            } else if (receiver.first -> get_receiver_type() == ReceiverType::STOREHOUSE){
                os << std::endl << "LINK src=worker-" << wrkr.get_id() << " dest=store-" << receiver.first -> get_id();
            }
        }
    };
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), from_worker);
}
