#include "GraphGenTypes.h"

GraphModel::GraphModel(InputModel m, double scalingFactor) {

    // ### Build the Data-Structure for IN-Degrees. ###
    for (const auto& [color, nodemap] : m.in_degrees)
    {
        std::unordered_map<Nodetype, std::unordered_map<int, Count>> sorted_degrees;
        
        // Find the node-types and match the elements accordingly
        for (const auto& [id, deg] : nodemap)
            ++sorted_degrees[m.nodes_to_types[id]][deg];


        // Normalize the counts to a probability for the given color and type and parse them to AliasTables
        for (const auto& [type, degmap] : sorted_degrees)
        { 
            double edge_sum = 0.0f;            
            for (const auto& [deg, count] : degmap)
                edge_sum += deg*count;
                
            // Nodes with higher degrees are more likely to generate an edge.
            // If there are [c] nodes with degree [d], the probability to pick this group
            //      should be [c]*[d] divided by the sum of all the groups.
            std::vector<std::pair<probability, DegreeContainer>> result;
            int range_start = 0;
            for (const auto& [deg, count] : degmap)
            {
                DegreeContainer cont;
                    cont.degree = deg;
                    cont.offset = range_start;
                    cont.nodeCount = count;

                result.push_back(
                    {
                        (deg*count) / edge_sum, // Probability for this entry
                        cont
                    }
                );
                range_start += count;
            }

            // Generate the Alias-Table
            this->in_degrees[color][type] = AliasTable(result);
        }
    }

    // ### Build the Data-Structure for IN-Degrees. ###
    for (const auto& [color, nodemap] : m.in_degrees)
    {
        std::unordered_map<Nodetype, std::unordered_map<int, Count>> sorted_degrees;
        
        // Find the node-types and match the elements accordingly
        for (const auto& [id, deg] : nodemap)
            ++sorted_degrees[m.nodes_to_types[id]][deg];


        // Normalize the counts to a probability for the given color and type and parse them to AliasTables
        for (const auto& [type, degmap] : sorted_degrees)
        { 
            double edge_sum = 0.0f;            
            for (const auto& [deg, count] : degmap)
                edge_sum += deg*count;
                
            // Nodes with higher degrees are more likely to generate an edge.
            // If there are [c] nodes with degree [d], the probability to pick this group
            //      should be [c]*[d] divided by the sum of all the groups.
            std::vector<std::pair<probability, DegreeContainer>> result;
            int range_start = 0;
            for (const auto& [deg, count] : degmap)
            {
                DegreeContainer cont;
                    cont.degree = deg;
                    cont.offset = range_start;
                    cont.nodeCount = count;

                result.push_back(
                    {
                        (deg*count) / edge_sum, // Probability for this entry
                        cont
                    }
                );
                range_start += count;
            }

            // Generate the Alias-Table
            this->in_degrees[color][type] = AliasTable(result);
        }
    }
    


    // Calculate SBM-Probabilites as a map of Alias-Tables
    for (const auto& [color, matrix] : m.sbm_matrix)
    {
        // Construct the resulting vector
        std::vector<std::pair<double, std::pair<Nodetype, Nodetype>>> result;
        int count = 0.0f;
        for (const auto& [pairing, amount] : matrix) {
            count += amount;

            result.push_back(std::make_pair(amount, pairing));
        }

        // Divide the count on every element by the sum of all counts to calculate the probabilities.
        for (int i = 0; i < result.size(); i++)
            result[i].first = result[i].first / count;
        this->sbm_probabilities.emplace(color, AliasTable<std::pair<Nodetype, Nodetype>>(result));
    }

    // Calculate the probability of a given degree occuring per edge_color and type

    // Construct the AliasTable for a given Edge being a certain color
    // int edge_sum = 0.0f;
    // for (const auto& [color, amount] : m.edge_colors)
    //     edge_sum += amount;
    
    // std::vector<std::pair<double, Edgecolor>> result;
    // for (const auto& [color, amount] : m.edge_colors)
    //     result.push_back(std::make_pair(amount / edge_sum, color));
    // this->edge_colors = AliasTable<Edgecolor>(result);
}


GraphModel::GraphModel(std::string filepath, double scalingFactor){}


void GraphModel::generate(){}