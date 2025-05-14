#include <iostream>
#include "../include/AliasTable.h"


int main(){
    std::vector<std::pair<probability, int>> probs;
        probs.push_back(std::make_pair(0.5, 0));
        probs.push_back(std::make_pair(0.1, 1));
        probs.push_back(std::make_pair(0.1, 2));
        probs.push_back(std::make_pair(0.3, 3));
    

    AliasTable<int> al(probs);
    std::vector<int> counts;
        counts.resize(4, 0);

    for (int i = 0; i < 10000000; i++)
        ++counts[al.getElement()];

    for (int i = 0; i < counts.size(); i++)
        std::cout << "counts[" << i << "] = " << (counts[i] / 10000000.0f) << std::endl;

    return 1;
}