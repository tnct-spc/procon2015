#ifndef FILE_EXPORT_H
#define FILE_EXPORT_H
#include "main.hpp"
#include <array>
#include <vector>

class file_export
{
private:
    typedef std::array<std::array<int,FIELD_SIZE>,FIELD_SIZE> raw_field_type;

    file_export() = default;
    ~file_export() = default;

public:
    file_export(int const nth,
                raw_field_type field,
                std::vector<raw_stone_type> stones);
}

#endif // FILE_EXPORT_H
