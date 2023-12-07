//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_PACKAGE_HPP
#define SIEC_PACKAGE_HPP

#include "types.hpp"


class Package{
    Package();
    Package(ElementID id) : id_(id) { assigned_IDs.insert(id_);}
    Package(Package&& package) : id_(package.id_) {};
    Package& operator = Package&& package;
    ElementID get_id() const { return id_;}
    ~Package();
private:
    ElementID id_;
    std::set<ElementID> assigned_IDs;
    std::set<ElementID> freed_IDs;
};
#endif //SIEC_PACKAGE_HPP
