#include <vector>
#include <unordered_map>
#include <chrono>

#include "../src/NodeType.cpp"


int main() {
    // Create Test-Nodes
    std::vector<std::pair<Degree, Count>> in = {
        std::make_pair(7, 1000),
        std::make_pair(5, 4000),
        std::make_pair(2, 3000),
        std::make_pair(1, 2000)
    };

    std::vector<std::pair<Degree, Count>> out = {
        std::make_pair(7, 1000),
        std::make_pair(5, 4000),
        std::make_pair(2, 3000),
        std::make_pair(1, 2000)
    };

    std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count>>> cl_in = {
        {"Red", in},
        {"Blue", in},
    };
    std::unordered_map<Edgecolor, std::vector<std::pair<Degree, Count>>> cl_out = {
        {"Red", out},
        {"Blue", std::vector<std::pair<Degree, Count>>{std::make_pair(1, 10000)}},
    };

    auto start = std::chrono::steady_clock::now();
    NodeType test = NodeType("Test_Blau", 0, 10000, cl_in, cl_out);
    auto finish = std::chrono::steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
    std::cout << "Konstruktion: " << elapsed_seconds << "s." << std::endl;

    // Simulate some results, tabulate the overall results
    int N = 10000000;
    std::unordered_map<int, int> results_start = {};
    std::unordered_map<int, int> results_target = {}; 

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < N; i++)
    {
        NodeID id_start = test.get_start_node("Red");
        NodeID id_target = test.get_target_node("Red");

        // results_start[id_start]++;
        // results_target[id_target]++;
    }
    finish = std::chrono::steady_clock::now();
    elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
    std::cout << "Abfragen: " << elapsed_seconds << "s.";
    
    return 0;
}


// Request-Time scales only with the number of Requests.
// The amount of Buckets (And Bucket-Size) has practically no impact on request-times.