# include "storage_types.hpp"

Package PackageQueue::pop() {
    Package pop_package;

    if (queue_type_ == LIFO){
        pop_package = std::move(queue_.back());
        queue_.pop_back();
    }
    else if (queue_type_ == FIFO){
        pop_package = std::move(queue_.front());
        queue_.pop_front();
    }
    return pop_package;
}