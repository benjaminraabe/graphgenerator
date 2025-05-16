#include "GraphGenTypes.h"

#include <fstream>
#include <iostream>
#include <filesystem>


GraphReader::GraphReader(const std::vector<std::string> &nodefile_paths,
                         const std::vector<std::string> &edgefile_paths){
    this->nodefiles = nodefile_paths;
    this->edgefiles = edgefile_paths;
}

void GraphReader::readTo(InputModel& model){}





void TSVReader::readTo(InputModel& model){
    // Read all Nodefiles
    std::cout << "\tReading Nodes..." << std::endl;
    for (const std::string& filename : this->nodefiles)
    {
        // Every File is parsed Line-By-Line
        if (std::ifstream file(filename); file.is_open()) {
            std::filesystem::path path{filename};
            std::cout << "\t\tReading \"" << path.string() << "\" (" << std::filesystem::file_size(path) << "bytes)" << std::endl;
            long long nodecount = 0;

            std::string line;
            std::getline(file, line); // Skip first line, this defines the structure of the file
            while (std::getline(file, line)) {
                auto p1 = line.find('\t');
                if (p1 == std::string::npos){std::cout << "\t\tSkipping invalid line: '" << line << "'" << std::endl; continue;}

                auto p2 = line.find('\t', p1+1);
                if (p2 == std::string::npos){std::cout << "\t\tSkipping invalid line: '" << line << "'" << std::endl; continue;}

                // Split the string on tabs, read the first and second component
                std::string     id    = line.substr(0, p1);
                Nodetype        nType = line.substr(p1+1, p2-(p1+1));
                model.readNode(id, nType);
                ++nodecount;
            }
            file.close();
            std::cout << "\t\tRead: " << nodecount << " Nodes." << std::endl;
        } else {
            throw std::runtime_error("\tError opening nodefile '" + filename + "'.");
        }
    }


    // Read all Edge-Files
    std::cout << "\tReading Edgefiles:" << std::endl;
    for (const std::string& filename : this->edgefiles)
    {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::filesystem::path path{filename};
            std::cout << "\t\tReading \"" << path.string() << "\" (" << std::filesystem::file_size(path) << "bytes)" << std::endl;
            long long edgecount = 0;

            std::string line;
            std::getline(file, line); // Skip first line, this defines the structure of the file
            while (std::getline(file, line)) {
                auto p1 = line.find('\t');
                if (p1 == std::string::npos){std::cout << "\t\tSkipping invalid line: '" << line << "'" << std::endl; continue;}

                auto p2 = line.find('\t', p1+1);
                if (p2 == std::string::npos){std::cout << "\t\tSkipping invalid line: '" << line << "'" << std::endl; continue;}

                auto p3 = line.find('\t', p2+1);
                if (p3 == std::string::npos){std::cout << "\t\tSkipping invalid line: '" << line << "'" << std::endl; continue;}

                // Split the string on tabs, read the first and second component as IDs,
                //      the third component as the color of the edge.
                std::string     start = line.substr(0, p1);
                std::string     end   = line.substr(p1+1, p2-(p1+1));
                Edgecolor       color = line.substr(p2+1, p3-(p2+1));

                model.readEdge(start, end, color);
                ++edgecount;
            }
            file.close();
            std::cout << "\t\tRead: " << edgecount << " Edges." << std::endl;
        }  else {
            throw std::runtime_error("\tError opening nodefile '" + filename + "'.");
        }
    }
}