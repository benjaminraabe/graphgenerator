#include <GraphGenTypes.h>
#include <ConfigParser.h>


int main(int argc, char *argv[]){
    // Enforce passing precisely one argument.
    if (argc != 2) {
        throw std::invalid_argument("Expected 1 argument (filepath to configuration) but got " + std::to_string(argc-1));
    }

    // Try to parse the config file.
    Config cfg = readConfig(argv[1]);

    // RNG-Seed-Initialization
    // TODO: Consider the effect of initializing all rdm_sources from the same seed.
    // TODO:    There might be some nasty correlation hidden here - a possible fix would be to advance the seed once
    // TODO:    before the next initialization.
    if (cfg.rng_seed == 0)
        cfg.rng_seed = std::time(nullptr);
    std::srand(cfg.rng_seed);
    std::cout << "[INFO] Initialized randomness with seed '" << cfg.rng_seed << "'" << std::endl;


    std::cout << "[1/4] Initializing..." << std::endl;
    InputModel in_model = InputModel();

    std::cout << "[2/4] Reading Data..." << std::endl;
    switch (cfg.reader_type) {
        case(INPUT_TYPE::I_TSV): {
            auto tsv_reader = TSVReader(cfg.node_files, cfg.edge_files);
            tsv_reader.readTo(in_model);
            break;
        }
        case(INPUT_TYPE::I_EMPTY):
            throw std::invalid_argument("The reader-type was not recognized. Fix the validation in your config!");
    }

    std::cout << "[3/4] Preprocessing..." << std::endl;
    in_model.preprocess();

    std::cout << "[4/4] Generating..." << std::endl;
    GraphModel graph = GraphModel(in_model, cfg.scalingFactor);

    switch (cfg.writer_type) {
        case(OUTPUT_TYPE::O_TSV): {
            auto tsv_writer = TSVWriter(cfg.output_file_nodes, cfg.output_file_edges);
            graph.generate(tsv_writer);
            break;
        }
        case(OUTPUT_TYPE::O_BENCHMARK): {
            auto bench_writer = BenchmarkWriter(0, 0);
            bench_writer.startTimer();
            graph.generate(bench_writer);
            bench_writer.info(bench_writer.stopTimer());
            break;
        }
        case(OUTPUT_TYPE::O_EMPTY):
            throw std::invalid_argument("The reader-type was not recognized. Fix the validation in your config!");
    }

    std::cout << "Done!" << std::endl;
}