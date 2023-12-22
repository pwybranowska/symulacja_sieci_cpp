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


//ParsedLineData parse_line(const std::string& line) {
//    ParsedLineData parsed_data;
//    std::istringstream token_stream(line);
//    std::vector<std::string> tokens;
//    std::string token;
//
//    if (std::getline(token_stream, token, ' ')) {
//        if (token == "LOADING_RAMP") {
//            parsed_data.element_type = ElementType::RAMP;
//        } else if (token == "WORKER") {
//            parsed_data.element_type = ElementType::WORKER;
//        } else if (token == "STOREHOUSE") {
//            parsed_data.element_type = ElementType::STOREHOUSE;
//        } else if (token == "LINK") {
//            parsed_data.element_type = ElementType::LINK;
//        }
//    }
//
//    while (std::getline(token_stream, token, ' ')) {
//        std::istringstream key_value_stream(token);
//        std::string key, value;
//        if (std::getline(key_value_stream, key, '=')) {
//            if (std::getline(key_value_stream, value)) {
//                parsed_data.map[key] = value;
//            }
//
//        }
//    }
//    return parsed_data;
//}

ParsedLineData parse_line(std::string line){
    ParsedLineData pld;
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char space = ' ';
    char eq = '=';
    std::getline(token_stream, token, space);
    if(token == "LOADING_RAMP"){
        pld.element_type = RAMP;
    }
    if(token == "WORKER"){
        pld.element_type = WORKER;
    }
    if(token == "STOREHOUSE"){
        pld.element_type = STOREHOUSE;
    }
    if(token == "LINK"){
        pld.element_type = LINK;
    }

    while (std::getline(token_stream, token, space)) {
        std::string key = token.substr(0, token.find(eq));
        std::string id = token.substr(token.find(eq) + 1, token.size()-1);
        pld.map.insert(std::make_pair(key,id));
    }
    return pld;
}

std::pair<std::string, std::string> parse_type(std::string line) {
    std::pair<std::string, std::string> key_id;
    std::istringstream token_stream(line);
    std::string token;

    std::getline(token_stream, token, '=');

    std::string key = token.substr(0, token.find('-'));
    std::string id = token.substr(token.find('-') + 1, token.size() - 1);
    key_id = std::make_pair(key, id);
    return key_id;
}

std::list<std::pair<std::string, std::string>> make_pairs_r(const Ramp& sender) {
    std::list<std::pair<std::string, std::string>> paired;

    std::string sender_type = "ramp";
    std::string sender_id = std::to_string(sender.get_id());
    std::string sender_string = sender_type + "-" + sender_id;

    std::map<IPackageReceiver*, double> receiver_list = sender.receiver_preferences_.get_preferences();
    for (auto& receiver : receiver_list) {
        ReceiverType type = receiver.first->get_receiver_type();
        std::string receiver_type;

        if (type == ReceiverType::WORKER) {
            receiver_type = "worker";
        } else {
            receiver_type = "store";
        }

        std::string receiver_id = std::to_string(receiver.first->get_id());
        std::string receiver_string = receiver_type + "-" + receiver_id;

        std::pair<std::string, std::string> pair = std::make_pair(sender_string, receiver_string);
        paired.insert(paired.end(), pair);
    }

    return paired;
}

std::list<std::pair<std::string, std::string>> make_pairs_w(const Worker& sender) {
    std::list<std::pair<std::string, std::string>> paired;

    std::string sender_type = "worker";
    std::string sender_id = std::to_string(sender.get_id());
    std::string sender_string = sender_type + "-" + sender_id;

    std::map<IPackageReceiver*, double> receiver_list = sender.receiver_preferences_.get_preferences();
    for (auto& receiver : receiver_list) {
        ReceiverType type = receiver.first->get_receiver_type();
        std::string receiver_type;

        if (type == ReceiverType::WORKER) {
            receiver_type = "worker";
        } else {
            receiver_type = "store";
        }

        std::string receiver_id = std::to_string(receiver.first->get_id());
        std::string receiver_string = receiver_type + "-" + receiver_id;

        std::pair<std::string, std::string> pair = std::make_pair(sender_string, receiver_string);
        paired.insert(paired.end(), pair);
    }

    return paired;
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
            ElementID id = std::stoi(parsed.map["id"]);
            TimeOffset di = std::stoi(parsed.map["delivery-interval"]);
            factory.add_ramp(Ramp(id, di));
        }
        else if (parsed.element_type == WORKER) {
            ElementID id = std::stoi(parsed.map["id"]);
            TimeOffset di =  std::stoi(parsed.map["processing-time"]);
            PackageQueueType package_type;
            if (parsed.map["queue-type"] == "LIFO") {
                package_type = PackageQueueType::LIFO;
            } else if (parsed.map["queue-type"] == "FIFO") {
                package_type = PackageQueueType::FIFO;
            }

            factory.add_worker(Worker(id, di, std::make_unique<PackageQueue>(PackageQueueType(package_type))));
        }
        else if (parsed.element_type == STOREHOUSE){
            ElementID id = std::stoi(parsed.map["id"]);
            factory.add_storehouse(Storehouse(id));
        }
        else if (parsed.element_type == LINK){
                std::vector<std::string> senders;
                std::vector<std::string> receivers;
                std::string token;
                std::istringstream token_sender(parsed.map["src"]);
                while (std::getline(token_sender, token, '-')){
                    senders.push_back(token);
                }
                std::istringstream token_receivers(parsed.map["dest"]);
                while (std::getline(token_receivers, token, '-')){
                    senders.push_back(token);
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
        os << "delivery-interval=" << rmp.get_delivery_interval();
    };
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), ramp);

    os << std::endl << std::endl <<" == WORKERS ==" << std::endl;
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

    os << std::endl << std::endl << " == STOREHOUSES ==" << std::endl;
    auto str = [&os] (const Storehouse& store){
        os << std::endl << "STOREHOUSE id=" << store.get_id();
    };
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), str);


    os << std::endl <<  std::endl << " == LINKS ==" << std::endl;
    auto from_ramp = [&os] (const Ramp& rmp){
        for (auto& receiver: rmp.receiver_preferences_.get_preferences())
            os << std::endl << "LINK src=ramp-" << rmp. get_id() << "dest=worker-" << receiver.first -> get_id();
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