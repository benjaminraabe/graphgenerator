#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <regex>

#ifndef CONFIGPARSER_H
    #define CONFIGPARSER_H

    enum INPUT_TYPE {
      I_EMPTY,
      I_TSV,        // Tab-Seperated-Value files
    };

    enum OUTPUT_TYPE {
      O_EMPTY,
      O_TSV,        // Tab-Seperated-Value files
      O_BENCHMARK,  // Voids any input and provides some analytics
    };

    struct Config {
      std::vector<std::string> node_files;
      std::vector<std::string> edge_files;

      float scalingFactor = 0.0;

      unsigned rng_seed = 0;

      std::string output_file_nodes = "generated_nodes.tsv";
      std::string output_file_edges = "generated_edges.tsv";

      INPUT_TYPE reader_type = I_EMPTY;
      OUTPUT_TYPE writer_type = O_EMPTY;
    };


    // Remove Leading/Trailing spaces and quotes from the given filepath.
    inline std::string clean_string(const std::string &s) {
      // "How to trim a std::string" https://stackoverflow.com/a/77272435/8444627 (User "ricab", acc. 15.05.2025 (Adapted))
      return std::regex_replace(s, std::regex{R"(^[\s\"\']+|[\s\"\']+$)"}, "");
    }


    // Convert a given string to uppercase
    inline std::string to_upper(std::string s) {
      // "Converting [...] to upper case" https://stackoverflow.com/a/735215/8444627 (User "Pierre", acc. 15.05.2025)
      std::transform(s.begin(), s.end(), s.begin(), ::toupper);
      return s;
    }


    // Parse a given string to it's corresponding ENUM, if it exists.
    // Leading/Trailing Whitespaces and Quotes are removed and the string is converted to uppercase
    inline INPUT_TYPE parse_input_type(std::string &s) {
      s = clean_string(s);
      s = to_upper(s);

      if (s == "TSV")
        return INPUT_TYPE::I_TSV;

      return INPUT_TYPE::I_EMPTY;
    }

    inline OUTPUT_TYPE parse_output_type(std::string &s) {
      s = clean_string(s);
      s = to_upper(s);

      if (s == "TSV")
        return OUTPUT_TYPE::O_TSV;
      if (s == "BENCHMARK")
        return OUTPUT_TYPE::O_BENCHMARK;

      return OUTPUT_TYPE::O_EMPTY;
    }


    // Try to parse the given config-file.
    // Provides some error-handling to prevent invalid configurations.
    inline Config readConfig(const std::string& config_file_name){
      Config cfg = Config();

      std::ifstream file(config_file_name);
      if (!file.is_open()) {
        throw std::runtime_error("Could not open config file '" + config_file_name + "'");
      }

      std::string line;
      unsigned int line_no = 1;
      while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';')
          continue;

        std::string attr = to_upper(line.substr(0, line.find('=')));
        line = line.substr(line.find('=') + 1);

        if (attr == "NODE_FILE") {
          cfg.node_files.push_back(clean_string(line));

        } else if (attr == "EDGE_FILE") {
          cfg.edge_files.push_back(clean_string(line));

        } else if (attr == "SCALE") {
          try {
            line = clean_string(line);
            cfg.scalingFactor = std::stof(line);
          } catch (const std::exception& e) {
            std::cerr << "[WARNING] Could not convert scaling factor'" << line << "' to float. (Line " << line_no << ")." << std::endl;
          }

        } else if (attr == "RNG_SEED") {
          try {
            line = clean_string(line);
            cfg.rng_seed = std::stoi(line);
          } catch (const std::exception& e) {
            std::cerr << "[WARNING] Could not convert rng-seed '" << line << "' to unsigned int. (Line " << line_no << ")." << std::endl;
          }

        } else if (attr == "OUTPUT_NODE_FILE") {
          cfg.output_file_nodes = clean_string(line);

        } else if (attr == "OUTPUT_EDGE_FILE") {
          cfg.output_file_edges = clean_string(line);

        } else if (attr == "READER_TYPE") {
          INPUT_TYPE t = parse_input_type(line);
          cfg.reader_type = t;
          if (t == INPUT_TYPE::I_EMPTY) {
            std::cerr << "[WARNING] Unknown reader-type '" << line << "'. (Line " << line_no << ")." << std::endl;
          }

        } else if (attr == "WRITER_TYPE") {
          OUTPUT_TYPE t = parse_output_type(line);
          cfg.writer_type = t;
          if (t == OUTPUT_TYPE::O_EMPTY) {
            std::cerr << "[WARNING] Unknown writer-type '" << line << "'. (Line " << line_no << ")." << std::endl;
          }

        } else {
          // Fallback: The attribute is misspelled or unknown.
          std::cerr << "[WARNING] Unknown key in configuration: '" << attr << "' (Line " << line_no << ")." << std::endl;
        }

      ++line_no;
      }


      // Basic checks to prevent faulty configurations
      std::string err;
      if (cfg.node_files.empty())
        err += "At least one node-file must be provided in the configuration file."
               "Use NODE_FILE=... to specify one or more filepaths.\n\n";
      if (cfg.edge_files.empty())
        err += "At least one edge-file must be provided in the configuration file."
               "Use EDGE_FILE=... to specify one or more filepaths.\n\n";

      if (cfg.scalingFactor <= 0.0)
        err += "Scaling factor must be provided and positive."
               "Use SCALE=X.XX to specify the scaling of the generated graph relative to the input graph.\n\n";
      else if (cfg.scalingFactor <= 1)
        std::cerr << "[WARNING] A scaling-factor >1 is recommended. You might run into runtime-issues or large amounts"
                  << " of duplicate edges. Use at your own risk!" << std::endl;

      if (cfg.rng_seed == 0)
        std::cout << "[INFO] No RNG-Seed given, using system time to initialize randomness instead. "
                  << " If you want reproducible results, set the config option RNG_SEED=... to a non-empty value.\n";

      if (cfg.output_file_nodes.empty())
        err += "An empty string has been passed as the path for the generated node-file."
               "Does your configuration contain a stray 'OUTPUT_NODE_FILE=' without a value?\n\n";
      if (cfg.output_file_edges.empty())
        err += "An empty string has been passed as the path for the generated edge-file."
               "Does your configuration contain a stray 'OUTPUT_EDGE_FILE=' without a value?\n\n";

      if (cfg.reader_type == INPUT_TYPE::I_EMPTY)
        err += "The reader-type has not been set. Use READER_TYPE=TYPE to specify the reader that "
               "should be used to parse the file.\n"
               "\tAvailable Types: 'TSV'\n\n";
      if (cfg.writer_type == OUTPUT_TYPE::O_EMPTY)
        err += "The writer-type has not been set. Use WRITER_TYPE=TYPE to specify the writer that "
               "should be used to produce the output-files in the desired format.\n"
               "\tAvailable Types: 'TSV', 'BENCHMARK'\n\n";

      if (!err.empty())
        throw std::runtime_error("One ore more problems have been encountered while parsing the config-file.\n" + err);

      return cfg;
    }


#endif //CONFIGPARSER_H
