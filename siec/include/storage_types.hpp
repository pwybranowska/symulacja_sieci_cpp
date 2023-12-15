//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_STORAGE_TYPES_HPP
#define SIEC_STORAGE_TYPES_HPP

#include "package.hpp"
#include <list>

enum PackageQueueType{
    FIFO,
    LIFO
};

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;

    virtual bool empty() const = 0;

    virtual std::size_t size() const = 0;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() = 0;
    virtual const_iterator end() = 0;

    virtual ~IPackageStockpile() = default;
};


class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
    ~IPackageQueue() override = default;
};

class PackageQueue : public IPackageQueue{
public:
    using const_iterator = std::list<Package>::const_iterator; //IDK CZY TO POTRZEBNE ALE WYWALA BLAD ZE NIE MA

    explicit PackageQueue(PackageQueueType queue_type): queue_(), queue_type_(queue_type){}
    void push(Package&& package) override {queue_.emplace_back(std::move(package));};

    bool empty() const override {return queue_.empty();}

    std::size_t size() const override { return queue_.size();}

    const_iterator cbegin() const override { return queue_.cbegin();};
    const_iterator cend() const override {return queue_.cend();};
    const_iterator begin() override {return queue_.begin();};
    const_iterator end() override {return queue_.end();};

    Package pop() override;
    PackageQueueType get_queue_type() const override {return queue_type_;}

    ~PackageQueue() override = default;

private:
    std::list<Package> queue_;
    PackageQueueType queue_type_;
};
#endif //SIEC_STORAGE_TYPES_HPP
