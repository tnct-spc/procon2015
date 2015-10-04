#ifndef PROCESS_TYPE_HPP
#define PROCESS_TYPE_HPP

class bit_process_type
{
public:
    unsigned int nth : 9;
    bool flip : 1;
    signed int rotate : 2;
    point_type position;
};

// 解答データの手順ひとつ分
// DEPRECATED
class process_type
{
public:
    process_type() = delete;
    ~process_type() = default;

    process_type(stone_type const & _stone,
                 point_type const & _position)
    : stone(_stone), position(_position) {}

    process_type & operator=(process_type const & other)
    {
        const_cast<stone_type &>(stone) = other.stone;
        const_cast<point_type &>(position) = other.position;
        return *this;
    }

    stone_type const stone;
    point_type const position;
};

#endif // PROCESS_TYPE_HPP
