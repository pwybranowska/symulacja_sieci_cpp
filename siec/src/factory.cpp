#include "factory.hpp"
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


ParsedLineData parse_line(const std::string& line) {
    ParsedLineData parsed_data;
    std::istringstream token_stream(line);
    std::vector<std::string> tokens;
    std::string token;

    if (std::getline(token_stream, token, ' ')) {
        if (token == "LOADING_RAMP") {
            parsed_data.element_type = ElementType::RAMP;
        } else if (token == "WORKER") {
            parsed_data.element_type = ElementType::WORKER;
        } else if (token == "STOREHOUSE") {
            parsed_data.element_type = ElementType::STOREHOUSE;
        } else if (token == "LINK") {
            parsed_data.element_type = ElementType::LINK;
        }
    }

    while (std::getline(token_stream, token, ' ')) {
        std::istringstream key_value_stream(token);
        std::string key, value;
        if (std::getline(key_value_stream, key, '=')) {
            if (std::getline(key_value_stream, value)) {
                parsed_data.map[key] = value;
            }

        }
    }
    return parsed_data;
}