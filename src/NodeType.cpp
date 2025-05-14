#include <vector>
#include <string>
#include <algorithm>

#include "GraphGenTypes.h"
#include "Prime.cpp"


NodeType::NodeType(const std::string& name, const Number offset, const Number nodeCount,
                   std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count>>> in_degrees,
                   std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count>>> out_degrees)
{
    // Set object attributes
    this->offset = offset;
    this->type_name = name;
    this->size = nodeCount;

    // Initialize Randomness
    this->rdm_gen = std::mt19937(std::random_device{}());

    // Find the first prime number larger than the number of Elements
    this->p = this->size;
    while (!is_prime(this->p))
    {
        this->p++;
    }

    // Custom sorting-function for Pairs: Order by product of first and second element, in descending order
    auto sort_ascending = [] (const auto& l, const auto& r) {
        return l.first*l.second > r.first*r.second;
       };

    // Extract occuring colors from in / out-degrees
    std::unordered_set<Edgecolor> colors{};
    for (const auto& [color, _] : out_degrees)
       colors.insert(color);
    for (const auto& [color, _] : in_degrees)
       colors.insert(color);

    // Construct a pair of alias tables for every occuring edge_color
    for (const Edgecolor& color : colors)
    {
        // We create a fallback, if the given color is not present in the degree-distribution:
        //      There are #node_count nodes, each with degree 1 (e.g. uniformly distributed)
        //      This should never be queried in praxis, but we will handle it nonetheless.
        if (in_degrees.count(color) == 0)
        {
            std::vector<std::pair<Degree, Count>> dummy{std::make_pair(1, this->size)};
            in_degrees[color] = dummy;
        }
        if (out_degrees.count(color) == 0)
        {
            std::vector<std::pair<Degree, Count>> dummy{std::make_pair(1, this->size)};
            out_degrees[color] = dummy;
        }

        // Temporary variables for simplified access
        std::vector<std::pair<Degree, Count>> in = in_degrees[color];
        std::vector<std::pair<Degree, Count>> out = out_degrees[color];

        // Sort degree_buckets by number of elements
        std::sort(in.begin(), in.end(), sort_ascending);
        std::sort(out.begin(), out.end(), sort_ascending);


        // Pad the number of Elements to this prime-number by continuously adding elements to the degree-buckets,
        //    in order of decreasing bucket-size. This is done to minimize the deviation from the actual statistical
        //    expectation for this degree-bucket.
        Number elements_to_pad = this->p - this->size;
        Number index_in = 0;
        Number index_out = 0;
        while (elements_to_pad > 0)
        {
            in[index_in].second++;
            out[index_out].second++;

            // Separate Index-Counters are necessary, because there can be a different number of distinct in/out-degrees.
            index_in = (index_in + 1) % static_cast<Number>(in.size());
            index_out = (index_out + 1) % static_cast<Number>(out.size());
            elements_to_pad--;
        }


        // Construct the Alias-Tables for the ranges of degrees
            // Construct a weighted count to calculate the probabilities for the buckets.
            Number weighted_sum_in = 0;
            for (const auto& [degree, element_count] : in)
                weighted_sum_in = weighted_sum_in + degree*element_count;

            Number weighted_sum_out = 0;
            for (const auto& [degree, element_count] : out)
                weighted_sum_out = weighted_sum_out + degree*element_count;

            // Calculate the probabilities and transfer them to new base-vectors for the Alias-Table.
            // Pre-Initialize Uniform_Distributions in the desired range
            std::vector<std::pair<probability, std::uniform_int_distribution<NodeID>>> in_probabilities;
            std::vector<std::pair<probability, std::uniform_int_distribution<NodeID>>> out_probabilities;

            Number lower_bound = 0;
            for (const auto& [degree, element_count] : in)
            {
                auto prob = static_cast<double>((degree*element_count) / static_cast<long double>(weighted_sum_in));
                Number upper_bound = lower_bound + (element_count-1);
                in_probabilities.emplace_back(prob, std::uniform_int_distribution<NodeID>(lower_bound, upper_bound));
                lower_bound = upper_bound + 1;
            }

            lower_bound = 0;
            for (const auto& [degree, element_count] : out)
            {
                auto prob = static_cast<double>((degree*element_count) / static_cast<long double>(weighted_sum_out));
                Number upper_bound = lower_bound + (element_count-1);
                out_probabilities.emplace_back(prob, std::uniform_int_distribution<NodeID>(lower_bound, upper_bound));
                lower_bound = upper_bound + 1;
            }

            // Construct the Alias-Tables
            this->in_distribution[color] = AliasTable(in_probabilities);
            this->out_distribution[color] = AliasTable(out_probabilities);

        // Draw a random a,b-Hash Function
        Number a = std::uniform_int_distribution<Number>(1, p-1)(this->rdm_gen);
        Number b = std::uniform_int_distribution<Number>(0, p-1)(this->rdm_gen);
        this->hash_a_b[color] = std::make_pair(a, b);
    }
}


NodeID NodeType::get_start_node(const Edgecolor& color){
    NodeID nodeid;
    Number a = this->hash_a_b[color].first;
    Number b = this->hash_a_b[color].second;

    while (true)
    {
        // Roll a range of IDs, pick one ID at uniform from that range
        nodeid = this->out_distribution[color].getElement()(this->rdm_gen);

        // Apply the Permutation-Function (a,b)-Hash to the id
        nodeid = (a*nodeid + b) % this->p;

        // Reroll any ids from the "Overflow-Range" [size, p]
        if (nodeid < this->size)
            return this->offset + nodeid;
    }
}

NodeID NodeType::get_target_node(const Edgecolor& color){
    Number a = this->hash_a_b[color].first;
    Number b = this->hash_a_b[color].second;

    while (true)
    {
        // Roll a range of IDs, pick one ID at uniform from that range
        NodeID nodeid = this->in_distribution[color].getElement()(this->rdm_gen);

        // Apply the Permutation-Function (a,b)-Hash to the id
        nodeid = (a*nodeid + b) % this->p;

        // Reroll any ids from the "Overflow-Range" [size, p]
        if (nodeid < this->size)
            return this->offset + nodeid;
    }
}

std::string NodeType::get_type_name(){
    return this->type_name;
}

Number NodeType::get_size() const{
    return this->size;
}

Number NodeType::get_offset() const{
    return this->offset;
}

