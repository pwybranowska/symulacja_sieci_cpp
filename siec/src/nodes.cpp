#include "nodes.hpp"
#include "package.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* receiver){
    double n = double(preferences_t_.size());

    if (n == 0){
        preferences_t_[receiver] = 1.0;
    }
    else {
        for (auto &i: preferences_t_){
            i.second = 1/(n+1);
        }
        preferences_t_[receiver] = 1/(n+1);
    }
}


void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver){
    double n = double(preferences_t_.size());

    if(n > 1){
        for(auto& el : preferences_t_){
            if (el.first != receiver){
                el.second = 1 / (n - 1);
            }
        }
    }
    preferences_t_.erase(receiver);
}

IPackageReceiver* ReceiverPreferences::choose_receiver(){
    double probability = pg_();
    if(probability >= 0 and probability <= 1.0){
        double probability_distribution = 0.0;
        for(auto& el : preferences_t_){
            probability_distribution += el.second;
            if(probability_distribution < 0 or probability_distribution > 1){
                return nullptr;
            }
            if (probability <= probability_distribution) {
                return el.first;
            }
        }
        return nullptr;
    }
    return nullptr;
}

void PackageSender::send_package() {
    IPackageReceiver *receiver;
    if(buffer_){
        receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}

void Ramp::deliver_goods(Time t){
    if (!buffer_){
        push_package(Package());
        buffer_.emplace(id_);
        t_ = t;
    } else {
        if (t - di_ == t_){
            push_package(Package());
        }
    }

}

void Worker::do_work(Time t){
    if (!buffer_ and !q_->empty()){
        buffer_.emplace(q_->pop());
        t_ = t;
    }
    else {
        if (t_ - t + 1 == pd_){
            push_package(Package(buffer_.value().get_id()));
            buffer_.reset();

            if(!q_->empty()){
                buffer_.emplace(q_->pop());
            }
        }
    }
}