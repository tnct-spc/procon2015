#include "file_export.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <iostream>

file_export::file_export(int const nth, raw_field_type field, std::vector<raw_stone_type> stones)
{
    std::ostringstream file;
    file << "prob";
    file.width(2);
    file.fill('0');
    file << nth << ".txt";

    std::ofstream output_file(file.str());
    std::cout << "file output" << std::endl << "---------------------------" << std::endl;
    for(auto const& cell_row : field)
    {
        for(auto const& cell : cell_row)
        {
            cell == 0 ? output_file << '0' : output_file << '1';
        }
        output_file << "\r\n";
    }
    output_file << "\r\n" << stones.size() << "\r\n";

    for(auto const& each_stone : stones)
    {
        for(auto const& cell_row : each_stone)
        {
            for(auto const& cell : cell_row)
            {
                cell == 0 ? output_file << '0' : output_file << '1';
            }
            output_file << "\r\n";
        }
        output_file << "\r\n";
    }
    output_file.close();
}
