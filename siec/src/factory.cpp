#include "factory.hpp"

template<class Node>
void NodeCollection<Node>::remove_by_id(ElementID id){
    auto elem = find_by_id((id));
    if (elem != nodes_.end()){
        nodes_.erase(elem);
    }
}