//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_FACTORY_HPP
#define SIEC_FACTORY_HPP

#include "types.hpp"
#include "package.hpp"
#include "nodes.hpp"
#include "storage_types.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>

template <typename Node>
class NodeCollection {
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { nodes_.push_back(std::move(node));}
    void remove_by_id(ElementID id);

    NodeCollection<Node>::iterator find_by_id(ElementID id) {return std::find_if(nodes_.begin(), nodes_.end(), [id](Node& elem) {return elem.get_id()==id;});}
    NodeCollection<Node>::const_iterator find_by_id(ElementID id) const {return std::find_if(nodes_.cbegin(), nodes_.cend(), [id] (const Node& elem) {return elem.get_id()==id;});}


    iterator begin() { return nodes_.begin() ;}
    iterator end() {return nodes_.end();}

    const_iterator cbegin() const {return nodes_.cbegin();}
    const_iterator cend() const {return nodes_.cend();}
    const_iterator begin() const {return nodes_.begin();}
    const_iterator end() const {return nodes_.end();}
private:
    container_t nodes_;

};

#endif //SIEC_FACTORY_HPP
