#include <syncstream>
#include "../include/GraphGenTypes.h"


// Abstract-Class "GraphWriter"
GraphWriter::~GraphWriter() = default;
void GraphWriter::writeEdge(const Edgecolor &color, NodeID startNode, NodeID endNode){};
void GraphWriter::writeNode(const Nodetype &nodeType, NodeID node){}




// Implementation for Tab-Seperated-Value files
TSVWriter::TSVWriter(const std::string &node_file_path, const std::string &edge_file_path) {
    this->node_file.open(node_file_path, std::ofstream::out | std::ofstream::trunc);
    this->edge_file.open(edge_file_path, std::ofstream::out | std::ofstream::trunc);

    if (!this->node_file.is_open()) {
        throw std::runtime_error("Could not open node-file for writing.");
    }
    if (!this->edge_file.is_open()) {
        throw std::runtime_error("Could not open edge-file for writing.");
    }
}

TSVWriter::~TSVWriter() {
    this->node_file.close();
    this->edge_file.close();
}

void TSVWriter::writeEdge(const Edgecolor &color, const NodeID startNode, const NodeID endNode) {
    std::osyncstream(this->edge_file) << startNode << "\t" << endNode << "\t" << color << std::endl;
}

void TSVWriter::writeNode(const Nodetype &nodeType, const NodeID node) {
    std::osyncstream(this->edge_file) << node << "\t" << nodeType << std::endl;
}




// Implementation for a mocking/benchmark writer.
BenchmarkWriter::BenchmarkWriter(const unsigned int padding_bytes_per_edge,
                                 const unsigned int padding_bytes_per_node):GraphWriter() {
    this->edge_padding = padding_bytes_per_edge;
    this->node_padding = padding_bytes_per_node;
}

void BenchmarkWriter::writeNode([[maybe_unused]] const Nodetype &nodeType,
                                [[maybe_unused]] NodeID node) {
    // NOLINTNEXTLINE(bugprone-sizeof-container)
    this->write_size_nodes += sizeof(nodeType) + sizeof(std::to_string(node)) + this->node_padding;
}

void BenchmarkWriter::writeEdge([[maybe_unused]] const Edgecolor &color,
                                [[maybe_unused]] NodeID startNode,
                                [[maybe_unused]] NodeID endNode) {
    // NOLINTNEXTLINE(bugprone-sizeof-container)
    this->write_size_edges += sizeof(color) + sizeof(std::to_string(startNode)) + sizeof(std::to_string(endNode))
                                            + this->edge_padding;
}

void BenchmarkWriter::startTimer() {
    this->start_time = std::chrono::high_resolution_clock::now();
}

float BenchmarkWriter::stopTimer() const {
    const std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<float>>(end - this->start_time).count();
}

void BenchmarkWriter::info(const float elapsed_time) const {
    double rate = static_cast<float>(this->get_generated_edge_size() + this->get_generated_node_size()) / elapsed_time / 1000000000.0;
    std::cout << "\nEdge-Generation took " << elapsed_time << " seconds @ " << rate << " GB/s." << std::endl;
}

unsigned long long BenchmarkWriter::get_generated_edge_size() const {
    return this->write_size_edges;
}
unsigned long long BenchmarkWriter::get_generated_node_size() const {
    return this->write_size_nodes;
}
