#include "nodes.hpp"

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
    for(probability >= 0 and probability <= 1.0){
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