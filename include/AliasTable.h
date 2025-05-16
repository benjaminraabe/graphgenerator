/*
    Implements the "Alias-Method" or "Alias-Table" to efficiently draw elements from a given, discrete, probability-distribution,
    slightly adapted to store arbitrary elements instead of indizes.
    Construction of the tables runs in O(n), drawing elements in O(1).

    Based on the numerically stable algorithm by M.D. Vose [1], as described by Keith Schwarz [2].

    [1] M.D. Vose, "A linear algorithm for generating random numbers with a given distribution" 
        In IEEE Transactions on Software Engineering, Vol. 17, No. 9, pp. 972-975 
        Sept. 1991 
        DOI: 10.1109/32.92917

    [2] Keith Schwarz, "Darts, Dice, and Coins: Sampling from a Discrete Distribution"
        Online: https://www.keithschwarz.com/darts-dice-coins/
        Dec. 2011, Accessed Dec. 2024
*/


#pragma once

#include <vector>
#include <random>
#include <queue>
#include <iostream>
#include <utility>

// Double-Precision Floating-Point-Number representing a probability.
// This must (!) be in the interval [0,1].
using probability = double;

template<typename T>
class AliasTable {
private:
    unsigned long long size;

    std::mt19937 gen;
    std::uniform_real_distribution<double> distr;

    std::vector<std::pair<probability, T> > probTable;
    std::vector<T> aliasTable;

public:
    explicit AliasTable(std::vector<std::pair<probability, T> > elements);

    AliasTable();

    T& getElement();
};

// Default initialization. Initializes the Table with the Default-Value for the given Type T.
// Really only used as a dummy value for member-variables.
template<typename T>
AliasTable<T>::AliasTable() {
    this->probTable.push_back(std::make_pair(1.0f, T()));
    this->size = 1;
}


template<typename T>
AliasTable<T>::AliasTable(std::vector<std::pair<probability, T> > elements) {
    // Initialize the RNG
    this->size = elements.size();
    this->gen = std::mt19937(std::random_device{}());
    this->distr = std::uniform_real_distribution<double>(0.0f, 1.0f); // Half-open intervall [0, 1)

    // Preprocessing: Sort the given Elements into a larger/smaller-queue by the average-probability 1/n.
    std::queue<std::pair<probability, T> > smaller;
    std::queue<std::pair<probability, T> > larger;
    double avg_prob = 1.0f / this->size;

    for (unsigned long long i = 0; i < this->size; i++) {
        if (elements[i].first < avg_prob) {
            smaller.push(elements[i]);
        } else {
            larger.push(elements[i]);
        }
    }

    // Construction of the Tables
    // Default-Case: A smaller and larger element are paired
    while (smaller.size() > 0 && larger.size() > 0) {
        std::pair<probability, T> s = smaller.front();
        std::pair<probability, T> l = larger.front();

        // Construct an entry with a small element in the ProbTable and a large element in the aliasTable
        // Probabilities are scaled by 1/n to normalize to 1.
        this->probTable.push_back(std::make_pair(s.first / avg_prob, s.second));
        this->aliasTable.push_back(l.second);

        // Reclassify the large element into the proper queue, depending on the remaining probability
        l.first = (l.first + s.first) - avg_prob;
        if (l.first < avg_prob) {
            smaller.push(l);
        } else {
            larger.push(l);
        }

        smaller.pop();
        larger.pop();
    }

    // Remainder-Case: Only larger elements remain, which should have exactly the right size.
    // Numerical differences may occur and are ignored.
    while (larger.size() > 0) {
        std::pair<probability, T> l = larger.front();
        this->probTable.push_back(std::make_pair(1.0f, l.second));
        larger.pop();
    }

    // "Impossible"-Case: Only smaller elements remain. This can only occur due to numerical inaccuracies.
    // Any values here should be exactly 1 as well.
    while (smaller.size() > 0) {
        std::pair<probability, T> s = smaller.front();
        this->probTable.push_back(std::make_pair(1.0f, s.second));
        smaller.pop();
    }

    // To preempt any index-errors, the size is set to the size of the probability-table.
    // This *should* not matter, if it does, something went wrong.
    if (this->size != this->probTable.size()) {
        std::clog << "Difference in the expected and actual size of an alias-table: Expected " << this->size <<
                ", actually got " << this->probTable.size() << ".";
    }
    this->size = this->probTable.size();
}


template<typename T>
T& AliasTable<T>::getElement() {
    probability r_idx = this->distr(this->gen);
    probability r_bias = this->distr(this->gen);
    int idx = static_cast<int>(std::floor(this->size * r_idx));

    if (r_bias < this->probTable[idx].first) {
        return this->probTable[idx].second;
    } else {
        return this->aliasTable[idx];
    }
}
