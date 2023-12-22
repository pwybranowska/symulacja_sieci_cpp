#ifndef SIEC_NODES_HPP
#define SIEC_NODES_HPP

#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include "helpers.hpp"

#include <optional>
#include <map>
#include <memory>


enum class ReceiverType{ WORKER, STOREHOUSE };


class IPackageReceiver{
public:
    virtual void receive_package(Package&& package) = 0;
    virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ReceiverType get_receiver_type() const = 0;

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
    const_iterator begin() const { return preferences_t_.cbegin(); }
    const_iterator end() const { return preferences_t_.cend(); }

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

class Storehouse : public IPackageReceiver{
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)){}
    
    void receive_package(Package&& package) override { d_->push(std::move(package));}
    ElementID get_id() const override { return id_; }

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin();}
    IPackageStockpile::const_iterator cend() const override {return d_->cend();}
    IPackageStockpile::const_iterator begin() const override {return d_->begin();}
    IPackageStockpile::const_iterator end() const override {return d_->end();}

    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;

};

class Worker : public PackageSender, public IPackageReceiver{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void receive_package(Package&& package) override { q_->push(std::move(package));}
    ElementID get_id() const override { return id_; }
    TimeOffset get_processing_duration() const { return pd_; }
    Time get_package_processing_start_time() const { return t_; }

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin();}
    IPackageStockpile::const_iterator cend() const override {return q_->cend();}
    IPackageStockpile::const_iterator begin() const override {return q_->begin();}
    IPackageStockpile::const_iterator end() const override {return q_->end();}


    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }


    void do_work(Time t);

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time t_;
    std::optional<Package> buffer_ = std::nullopt;
};
#endif //SIEC_NODES_HPP
