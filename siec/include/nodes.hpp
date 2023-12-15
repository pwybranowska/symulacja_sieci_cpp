#ifndef SIEC_NODES_HPP
#define SIEC_NODES_HPP

#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include "helpers.hpp"
#include <optional>
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

class ReceiverPreferences {
public:
    ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(std::move(pg)) {}
    using preferences_t = std::map<IPackageReceiver *, double>;

    void add_receiver(IPackageReceiver* receiver);
    void remove_receiver(IPackageReceiver* receiver);
    IPackageReceiver *choose_receiver();
    const preferences_t &get_preferences() const { return preferences_t_; }

    using const_iterator = preferences_t::const_iterator;
    const_iterator cbegin() const { return preferences_t_.cbegin(); }
    const_iterator cend() const { return preferences_t_.cend(); }
    const_iterator begin() { return preferences_t_.cbegin(); }
    const_iterator end() { return preferences_t_.cend(); }

private:
    preferences_t preferences_t_;
    ProbabilityGenerator pg_;
};


class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender() = default;

    PackageSender(PackageSender &&package_sender) = default;

    void send_package();

    const std::optional<Package>& get_sending_buffer() {return buffer_;};

protected:
    void push_package(Package &&package) {buffer_.emplace(package.get_id());};
private:
    std::optional<Package> buffer_ = std::nullopt;

};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) : PackageSender(), id_(id), di_(di) {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const {return di_;};

    ElementID get_id() const {return id_;};

private:
    ElementID id_;
    TimeOffset di_;
    Time t_;
    std::optional<Package> buffer_ = std::nullopt;
};

#endif //SIEC_NODES_HPP
