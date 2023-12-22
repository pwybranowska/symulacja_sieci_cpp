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

template <class Node>
class NodeCollection {
public:
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

class Factory {
public:

    void add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); };
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); };
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); };
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); };
    NodeCollection<Ramp>::const_iterator ramp_cbegin() { return ramps_.cbegin(); };
    NodeCollection<Ramp>::const_iterator ramp_cend() { return ramps_.cend(); };


    void add_worker(Worker&& worker) { workers_.add(std::move(worker)); }
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); };
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator worker_cbegin() { return workers_.cbegin(); };
    NodeCollection<Worker>::const_iterator worker_cend() { return workers_.cend(); };


    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); };
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() { return storehouses_.cbegin(); };
    NodeCollection<Storehouse>::const_iterator storehouse_cend() { return storehouses_.cend(); };

    bool is_consistent();

    void do_deliveries(Time t);

    void do_work(Time t);
    void do_package_passing();

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    template<class Node>
            void remove_receiver(NodeCollection<Node>& collection, ElementID id);
};

enum ElementType {
    RAMP, WORKER, STOREHOUSE, LINK,
};

struct ParsedLineData{
    ElementType element_type;
    std::map<std::string, std::string> mapped;
};

ParsedLineData parse_line(std::string line);

Factory load_factory_structure(std::istream& input_stream);

void save_factory_structure(Factory& factory, std::ostream& output_stream);

#endif //SIEC_FACTORY_HPP
