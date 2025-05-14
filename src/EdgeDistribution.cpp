#include "../include/GraphGenTypes.h"


EdgeDistribution::EdgeDistribution(std::unordered_map<Nodetype, Count> nodes,
                                   const std::map<std::pair<Nodetype, Nodetype>, Count> &edges) {
    // Count the overall occurrences of NodeTypes, IF they habe outgoing edges.
    //      NodeTypes w/o outgoing edges will not be considered for the later process.
    // Count the overall outgoing Edges per NodeType of the start node.
    long long node_sum = 0;
    std::unordered_map<Nodetype, long double> filtered_nodes;
    std::unordered_map<Nodetype, Count> edge_sums;

    for (const auto &[key, value]: edges) {
        if (value > 0) {
            Nodetype start_node_type = key.first;
            if (filtered_nodes.count(start_node_type) == 0) {
                node_sum += nodes[start_node_type];
                filtered_nodes[start_node_type] = nodes[start_node_type]; // This is inefficient...
            }
            edge_sums[start_node_type] += value;
        }
    }

    // Transform the NodeTypes-counts to the probabilities of a randomly picked node to be of this type.
    for (const auto &[key, value]: filtered_nodes)
        filtered_nodes[key] = value / node_sum;

    // Calculate the probability of a given edge-transition occurring:
    //      P(A->B) = P(Node of Type A) * P(Transition to B | Node of Type A)
    //      As "No-Transition" NodeTypes are removed beforehand, the resulting Probabilities should sum to 1.
    std::vector<std::pair<probability, std::pair<Nodetype, Nodetype> > > edge_probabilities;
    for (const auto &[key, value]: edges) {
        if (value > 0) {
            Nodetype start_node_type = key.first;
            auto prob = static_cast<double>(
                (filtered_nodes[start_node_type] / node_sum)
                * (static_cast<long double>(value) / edge_sums[start_node_type])

            );

            edge_probabilities.emplace_back(prob, key);
        }
    }

    // Construct the Alias-Table for the Transitions
    this->transitions = AliasTable(edge_probabilities);
}


// Default initialization. Initializes the Table with the Default-Value for the given Type T.
// Really only used as a dummy value for member-variables.
EdgeDistribution::EdgeDistribution() {
    this->transitions = AliasTable<std::pair<Nodetype, Nodetype> >();
}


std::pair<Nodetype, Nodetype> EdgeDistribution::getTransition() {
    return transitions.getElement();
}
