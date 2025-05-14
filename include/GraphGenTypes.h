#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <atomic>
#include <chrono>

#include "AliasTable.h"

using Edgecolor = std::string;
using Nodetype = std::string;

using NodeID = long long;
using Number = long long;
using Count = long long;
using Degree = long long;
using Index = long long;


class InputModel {
public:
    InputModel();

    explicit InputModel(const std::string &filepath);

    void readNode(const std::string &node, const std::string &color);

    void readEdge(const std::string &start, const std::string &end, const Edgecolor &color);

    void preprocess();

    bool save(const std::string &filepath);

    bool is_preprocessed;
    Count node_count{};
    std::unordered_map<Edgecolor, Count> edge_count;

    // Number of Type-Type-Transitions for each Edge-Color
    std::unordered_map<Edgecolor, std::map<std::pair<Nodetype, Nodetype>, Count> > sbm_matrix;

    // Preprocessed Degree-"Buckets"
    std::unordered_map<Nodetype, std::unordered_map<Edgecolor, std::unordered_map<Degree, Count> > > in_distribution;
    std::unordered_map<Nodetype, std::unordered_map<Edgecolor, std::unordered_map<Degree, Count> > > out_distribution;

    // Count the number of occurrences for every Type/Color to calculate a distribution in the end.
    std::unordered_map<Nodetype, Count> node_types;
    std::unordered_set<Edgecolor> edge_colors;

private:
    // Save the nodeType for every read node. Needed later to map the edges to the correct node-type
    std::unordered_map<std::string, Nodetype> nodes_to_types;

    // Count Incoming/Outgoing Edges for every Node
    std::unordered_map<Edgecolor, std::map<std::string, Count> > in_degrees;
    std::unordered_map<Edgecolor, std::map<std::string, Count> > out_degrees;
};


// Abstract Base-Class for any additional readers.
class GraphReader {
public:
    GraphReader(const std::vector<std::string> &nodefile_paths, const std::vector<std::string> &edgefile_paths);

    virtual ~GraphReader() = default;

    virtual void readTo(InputModel &model);

protected:
    std::vector<std::string> nodefiles;
    std::vector<std::string> edgefiles;
};


// Simple Reader for "Tab-Separated-Values"-Files (.tsv)
class TSVReader : public GraphReader {
    using GraphReader::GraphReader;

    public:
        void readTo(InputModel &model) override;
};


// Container for the Transition-Probabilities for a single Type of Edge
class EdgeDistribution {
    private:
        AliasTable<std::pair<Nodetype, Nodetype> > transitions;

    public:
        EdgeDistribution(std::unordered_map<Nodetype, Count> nodes,
                         const std::map<std::pair<Nodetype, Nodetype>, Count> &edges);

        EdgeDistribution();

        ~EdgeDistribution() = default;

        std::pair<Nodetype, Nodetype> getTransition();
};


// Container for the Degree/Attribute-Distribution for a single Type of Node
class NodeType {
    private:
        std::unordered_map<Edgecolor, AliasTable<std::uniform_int_distribution<NodeID> > > in_distribution;
        std::unordered_map<Edgecolor, AliasTable<std::uniform_int_distribution<NodeID> > > out_distribution;

        std::mt19937 rdm_gen;

        std::string type_name;
        Number offset{};
        Number size{};
        std::unordered_map<Edgecolor, std::pair<Number, Number> > hash_a_b;
        Number p{};

    public:
        NodeType(const std::string &name, Number offset, Number nodeCount,
                 std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count> > > in_degrees,
                 std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count> > > out_degrees);

        NodeType() = default;

        ~NodeType() = default;

        NodeID get_start_node(const Edgecolor &color);

        NodeID get_target_node(const Edgecolor &color);

        std::string get_type_name();

        Number get_size() const;

        Number get_offset() const;
};




// Provide an interface (abstract class) to implement specific writers.
// If you decide to implement a new writer, care should be taken to make it thread-safe.
//      Multithreading will be used to generate the data passed to this structure!
class GraphWriter {
public:
    virtual ~GraphWriter();

    virtual void writeEdge(const Edgecolor &color, NodeID startNode, NodeID endNode);
    virtual void writeNode(const Nodetype &nodeType, NodeID node);

protected:
    std::ofstream node_file;
    std::ofstream edge_file;
};


// Simple Writer for "Tab-Separated-Values"-Files (.tsv)
// Thread safety is achieved using std::osyncstream implemented by C++20
class TSVWriter : public GraphWriter {
public:
    TSVWriter(const std::string &node_file_path, const std::string &edge_file_path);
    ~TSVWriter() override;

    void writeEdge(const Edgecolor &color, NodeID startNode, NodeID endNode) override;
    void writeNode(const Nodetype &nodeType, NodeID node) override;
};


// Implement a Writer for Benchmarking
// All Input is discarded, but the size of the output is measured and preserved.
// Optional Parameter "padding_bytes_per_..." adds extra bytes for i.e. spaces or tabs in the "real" writer
class BenchmarkWriter : public GraphWriter {
    public:
        explicit BenchmarkWriter(unsigned int padding_bytes_per_edge = 3, unsigned int padding_bytes_per_node = 2);

        void writeEdge(const Edgecolor &color, NodeID startNode, NodeID endNode) override;
        void writeNode(const Nodetype &nodeType, NodeID node) override;

        void startTimer();
        float stopTimer() const;

        unsigned long long get_generated_edge_size() const;
        unsigned long long get_generated_node_size() const;

    private:
        unsigned int edge_padding;
        unsigned int node_padding;
        std::atomic<unsigned long long> write_size_edges {0};
        std::atomic<unsigned long long> write_size_nodes {0};
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};




// Scaled up representation of the generator, derived from some model.
class GraphModel {
public:
    GraphModel(InputModel m, long double scalingFactor);

    GraphModel(const std::string &filepath, long double scalingFactor);

    void generate(GraphWriter &writer);

private:
    InputModel model;
    Count nbr_nodes{};
    std::unordered_map<Edgecolor, Count> nbr_edges;

    // Reference to NodeType-Objects
    std::unordered_map<Nodetype, NodeType> nodes;

    // Reference to EdgeType-Objects
    std::unordered_map<Edgecolor, EdgeDistribution> edges;
};



