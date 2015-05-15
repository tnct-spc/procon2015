#ifndef FILE_EXPORT_HPP
#define FILE_EXPORT_HPP
#include "problem_maker.hpp"
#include <array>
#include <vector>

class file_export
{
private:
    file_export() = default;
    ~file_export() = default;

public:
    file_export(int const nth,
                raw_field_type field,
                std::vector<raw_stone_type> stones);
};

#endif // FILE_EXPORT_HPP
