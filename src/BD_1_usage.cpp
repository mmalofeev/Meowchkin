#include <iostream>
#include "bd_observer.hpp"

int main() {
    StatisticObserver obs;
    obs.display_table();
//    obs.increase_count_of_usage(9);
//    obs.increase_count_of_usage(9);
    for (auto p : obs.get_frequency_of_usage()) {
        std::cout << "Card " + std::to_string(p.second) + " was used " + std::to_string(p.first) + " times\n";
    }
    obs.display_table();
}