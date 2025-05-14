#include <GraphGenTypes.h>


int main(){
    std::vector<std::string> node_files = {"../datasets/stark-prime/nodes.tsv"};
    std::vector<std::string> edge_files = {"../datasets/stark-prime/edges.tsv"};

    std::cout << "Initializing Models..." << std::endl;
    TSVReader reader = TSVReader(node_files, edge_files);
    InputModel i = InputModel();

    std::cout << "Reading Data..." << std::endl;
    reader.readTo(i);

    std::cout << "Preprocessing..." << std::endl;
    i.preprocess();

    std::cout << "Starting Generation..." << std::endl;


    GraphModel graph = GraphModel(i, 10.0);
    BenchmarkWriter writer = BenchmarkWriter(0, 0);

    writer.startTimer();
    graph.generate(writer);

    auto elapsed = writer.stopTimer();
    std::cout << "Done." << std::endl << std::endl;
    std::cout << "Edge-Generation took " << elapsed << " seconds @ ";
    std::cout << static_cast<float>(writer.get_generated_edge_size() + writer.get_generated_node_size()) / elapsed / 1000000000.0
    << " GB/s." << std::endl;
}