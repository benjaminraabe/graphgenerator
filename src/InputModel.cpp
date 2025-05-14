#include "../include/GraphGenTypes.h"

#include <iostream>

InputModel::InputModel() {
    this->is_preprocessed = false;
}

InputModel::InputModel(const std::string &filepath) {
    // Todo: Read the raw model from the given file.
    // Todo: Custom-Convert the resulting JSON to the object.
    this->is_preprocessed = true;
}

void InputModel::readNode(const std::string &node, const std::string &node_type) {
    ++this->node_count;

    // Increase the count of the node-color
    ++this->node_types[node_type];

    // Remember this node for future lookups
    this->nodes_to_types[node] = node_type;
}


void InputModel::readEdge(const std::string &start, const std::string &end, const Edgecolor &color) {
    ++this->edge_count[color];

    // Increase the entry in the SBM-Matrix
    Nodetype type_start = this->nodes_to_types[start];
    Nodetype type_end = this->nodes_to_types[end];
    ++this->sbm_matrix[color][std::make_pair(type_start, type_end)];

    // Increase In/Out Degree of the node
    ++this->out_degrees[color][start];
    ++this->in_degrees[color][end];

    // Add the Color to the set of Edge-Colors
    this->edge_colors.insert(color);
}


void InputModel::preprocess() {
    // Clear the data-structures, if it was previously preprocessed
    if (this->is_preprocessed) {
        this->in_distribution.clear();
        this->out_distribution.clear();
    }

    // Data is processed (pivoted) into a container more suitable for further use.
    for (const auto &[color, nodes]: this->in_degrees)
        for (const auto &[id, count]: nodes)
            this->in_distribution[this->nodes_to_types[id]][color][count]++;

    for (const auto &[color, nodes]: this->out_degrees)
        for (const auto &[id, count]: nodes)
            this->out_distribution[this->nodes_to_types[id]][color][count]++;


    // Pad the containers with zero-degree nodes, if other degrees exist.
    for (const auto &[ntype, container]: this->in_distribution) {
        for (const auto &[color, buckets]: container) {
            if (!buckets.empty()) {
                // Count the number of nodes with associated degrees
                long long node_sum = 0;
                for (const auto &[_, count]: buckets)
                    node_sum += count;

                // The number of nodes with 0-degree is the number of nodes in the type, minus nodes with other degrees
                this->in_distribution[ntype][color][0] = this->node_types[ntype] - node_sum;
            }
        }
    }
    for (const auto &[ntype, container]: this->out_distribution) {
        for (const auto &[color, buckets]: container) {
            if (!buckets.empty()) {
                // Count the number of nodes with associated degrees
                long long node_sum = 0;
                for (const auto &[_, count]: buckets)
                    node_sum += count;

                // The number of nodes with 0-degree is the number of nodes in the type, minus nodes with other degrees
                this->out_distribution[ntype][color][0] = this->node_types[ntype] - node_sum;
            }
        }
    }

    this->is_preprocessed = true;
}


bool InputModel::save(const std::string &filepath) {
    if (!this->is_preprocessed) {
        this->preprocess();
    }

    // TODO: Serialization

    return false;
}
