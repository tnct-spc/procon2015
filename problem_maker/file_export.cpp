#include "file_export.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <iostream>

file_export::file_export(int const nth, raw_field_type field, std::vector<raw_stone_type> stones)
{
    /* filename = questXX.txt */
    std::ostringstream file;
    file << "quest";
    file.width(2);
    file.fill('0');
    file << nth << ".txt";
    std::ofstream output_file(file.str());

    std::cout << "file output" << std::endl << "---------------------------" << std::endl;

    /* (1)敷地情報 */
    for(auto const& cell_row : field) {
        for(auto const& cell : cell_row) {
            output_file << cell; /* cell == 0 or 1 */
        }
        output_file << "\r\n";
    }

    /* (2)石情報 */
    /* (a)石の個数(半角数字) */
    output_file << "\r\n" << stones.size();

    /* (b)各石の形状 */
    for(auto const& each_stone : stones) {
        output_file << "\r\n";
        for(auto const& cell_row : each_stone) {
            for(auto const& cell : cell_row) {
                output_file << cell; /* cell == 0 or 1 */
            }
            output_file << "\r\n";
        }
    }

    output_file.close();
}
