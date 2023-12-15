#ifndef SIEC_NODES_HPP
#define SIEC_NODES_HPP

#include "package.hpp"
#include "storage_types.hpp"
#include <map>

class IPackageReceiver{
public:
    virtual void receive_package(Package&& package) = 0;
    virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() = 0;
    virtual IPackageStockpile::const_iterator end() = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences{
public:
    ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(std::move(pg)) {}

    using preferences_t = std::map<IPackageReceiver *, double>;

    void add_receiver(IPackageReceiver* receiver);
    void remove_receiver(IPackageReceiver* receiver);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const {return preferences_t_;}

    using const_iterator = preferences_t::const_iterator;
    const_iterator cbegin() const {return preferences_t_.cbegin();}
    const_iterator cend() const {return preferences_t_.cend();}
    const_iterator begin() const {return preferences_t_.cbegin();}
    const_iterator end() const {return preferences_t_.cend(); }

private:
    preferences_t preferences_t_;
    ProbabilityGenerator pg_;
};

#endif //SIEC_NODES_HPP
