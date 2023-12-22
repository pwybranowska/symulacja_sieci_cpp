#include "factory.hpp"
#include <map>
#include <stdexcept>

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
};