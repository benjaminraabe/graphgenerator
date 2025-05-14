#include "../include/GraphGenTypes.h"
#include "chrono"
#include "syncstream"


GraphModel::GraphModel(InputModel m, const long double scalingFactor) {
    if (!m.is_preprocessed)
        m.preprocess();

    // The number of nodes/edges to generate are derived from the number of nodes/edges from
    //    the InputModel and the scalingFactor.
    for (const auto &[color, cts]: m.edge_count) {
        this->nbr_edges[color] = std::floor(cts * scalingFactor);
        this->edges[color] = EdgeDistribution(m.node_types, m.sbm_matrix[color]);
    }

    // Crate the node-distributions. The number of nodes in each bucket needs to be scaled with the given factor.
    Count offset = 0;
    for (auto &[ntype, desired_node_count]: m.node_types) {
        desired_node_count = std::floor(desired_node_count * scalingFactor);
        this->nbr_nodes += desired_node_count;
        std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count> > > in;
        std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count> > > out;

        Count nbr_in_nodes = 0;
        Count nbr_out_nodes = 0;
        for (const auto &[ecolor, _]: m.in_distribution[ntype]) {
            // Scale the distributions
            const auto in_element = m.in_distribution[ntype][ecolor];
            for (const auto &[deg, count]: in_element) {
                if (deg == 0) { continue; }
                Count rounded_nodes = std::floor(count * scalingFactor);
                nbr_in_nodes += rounded_nodes;
                in[ecolor].emplace_back(std::make_pair(deg, rounded_nodes));
            }
            const auto out_element = m.out_distribution[ntype][ecolor];
            for (const auto &[deg, count]: out_element) {
                if (deg == 0) { continue; }
                Count rounded_nodes = std::floor(count * scalingFactor);
                nbr_out_nodes += rounded_nodes;
                out[ecolor].emplace_back(std::make_pair(deg, rounded_nodes));
            }

            // Pad with zero-degree nodes. This compensates for rounding-errors, as well as existing zero-nodes.
            if (nbr_in_nodes < desired_node_count) {
                in[ecolor].emplace_back(std::make_pair(0, desired_node_count-nbr_in_nodes));
            }
            if (nbr_out_nodes < desired_node_count) {
                out[ecolor].emplace_back(std::make_pair(0, desired_node_count-nbr_out_nodes));
            }
        }

        this->nodes[ntype] = NodeType(ntype, offset, desired_node_count, in, out);
        offset += desired_node_count;
    }
}


GraphModel::GraphModel(const std::string &filepath, long double scalingFactor) {
    // TODO: Construction from file
}


void GraphModel::generate(GraphWriter &writer) {
    // Generate k random Edges for every color, with k = this->nbr_edges[color]:
    for (const auto &[color, cts]: this->nbr_edges) {

        #pragma omp parallel for
        for (int i = 0; i < cts; ++i) {
            // Get the Type of the start/endpoint of the edge (Stochastic-Block-Model)
            auto [start_type, end_type] = this->edges[color].getTransition();

            // Get a concrete NodeID for to Nodes of the given type (Degree-Correction)
            writer.writeEdge(color, this->nodes[start_type].get_start_node(color),
                                       this->nodes[end_type].get_target_node(color));

        }

    }

    // Write alle nodes to a file
    for (const auto &[nodetype, node]: this->nodes) {
        for (NodeID i = node.get_offset(); i < node.get_offset() + node.get_size(); ++i) {
            writer.writeNode(nodetype, i);
        }

        std::cout << "Nodetype '" << nodetype << "': "
        << node.get_offset() << "-" << node.get_offset() + node.get_size() - 1 << std::endl;
    }
}
