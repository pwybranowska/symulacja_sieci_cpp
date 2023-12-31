//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_PACKAGE_HPP
#define SIEC_PACKAGE_HPP

#include "types.hpp"

#include <set>

class Package{
public:
    Package();
    Package(ElementID id) : id_(id) { assigned_IDs.insert(id_);}
    Package(Package&& package) : id_(package.id_) {};
    Package& operator= (Package&&) noexcept;
    ElementID get_id() const { return id_;}
    ~Package();
private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;
};
#endif //SIEC_PACKAGE_HPP
