#include "package.hpp"

std::set<ElementID> Package::assigned_IDs = {};
std::set<ElementID> Package::freed_IDs = {};

Package::~Package(){
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);

}
//Gdy zachodzi konieczność przydzielenia ID dla nowo tworzonego półproduktu,
// w pierwszej kolejności rozpatrywane są identyfikatory ze zbioru freed_IDs;
// jeśli brak takich – wówczas przydzielany jest nowy identyfikator, o 1
//większy od największego identyfikatora w zbiorze assigned_IDs (każdorazowo zbiór
// assigned_IDs jest odpowiednio aktualizowany).
Package::Package() {
    if (freed_IDs.empty() and assigned_IDs.empty()) {
        id_ = 1;
    } else if (!freed_IDs.empty()) {
        id_ = *freed_IDs.begin();
        freed_IDs.erase(freed_IDs.begin());
    } else if (!assigned_IDs.empty()) {
        id_ = *assigned_IDs.rbegin() + 1;
    }
    assigned_IDs.insert(id_);
}


Package& Package::operator=(Package&& package) noexcept {
    if (this == &package)
        return *this;

    assigned_IDs.erase(this->id_);
    freed_IDs.insert(this->id_);

    this->id_ = package.id_;

    assigned_IDs.insert(this->id_);

    return *this;
}
