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

    virtual size_t size() const = 0;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() = 0;
    virtual const_iterator end() = 0;

    virtual ~IPackageStockpile() = default;
};


class IPackageQueue : IPackageStockpile{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue{
public:
    PackageQueueType
};
#endif //SIEC_STORAGE_TYPES_HPP
