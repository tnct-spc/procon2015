
#include "utils.hpp"
#include <string>

// 文字列を文字列のデリミタにより分割する
std::vector<std::string> _split(std::string const & target, std::string const & delimiter)
{
    std::size_t begin = 0, end;
    auto const delimiter_length = delimiter.size();
    std::vector<std::string> result;

    while (begin <= target.size()) {
        end = target.find(delimiter, begin);
        if (end == std::string::npos) {
            end = target.size();
        }
        result.push_back(target.substr(begin, end - begin));
        begin = end + delimiter_length;
    }

    return result;
}

