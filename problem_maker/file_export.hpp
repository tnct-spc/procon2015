#ifndef FILE_EXPORT_HPP
#define FILE_EXPORT_HPP
#include "raw_stone.hpp"
#include "raw_field.hpp"
#include <array>
#include <vector>

class file_export
{
private:
    file_export() = default;

public:
    file_export(int const nth,
                raw_field_type field,
                std::vector<raw_stone_type> stones);
    ~file_export() = default;
};

#endif // FILE_EXPORT_HPP
