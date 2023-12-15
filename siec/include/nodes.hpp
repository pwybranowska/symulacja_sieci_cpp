//
// Created by admin on 30.11.2023.
//

#ifndef SIEC_NODES_HPP
#define SIEC_NODES_HPP

#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include <optional>

class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender() = default;

    PackageSender(PackageSender &&package_sender) = default;

    void send_package();

    const std::optional<Package>& get_sending_buffer() {return bufor_;};

protected:
    void push_package(Package &&package) {bufor_.emplace(package.get_id());};
private:
    std::optional<Package> bufor_ = std::nullopt;
};

#endif //SIEC_NODES_HPP
