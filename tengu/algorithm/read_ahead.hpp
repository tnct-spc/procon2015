#ifndef READAHEAD
#define READAHEAD
#include <takao.hpp>
#include <array>
#include <queue>

class read_ahead : public algorithm_type
{
    Q_OBJECT
public:
    read_ahead(problem_type _problem);
    ~read_ahead();
    void run();

    struct stones_info_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
        stones_info_type(point_type point, std::size_t angle, stone_type::Sides side):point(point),angle(angle),side(side){};
        stones_info_type(){};
    };

    struct search_type
    {
        field_type field;
        std::vector<stones_info_type> iv;
        double score = 0;
        int island;
        search_type(field_type field,std::vector<stones_info_type> iv,double score,int island):field(field),iv(iv),score(score),island(island){};
        search_type(){};
        friend inline bool operator== (search_type const& lhs, search_type const& rhs)
        {
            return lhs.iv[0].point == rhs.iv[0].point &&
                   lhs.iv[0].angle == rhs.iv[0].angle &&
                   lhs.iv[0].side == rhs.iv[0].side &&
                   lhs.iv.size() == rhs.iv.size() &&
                   lhs.score == rhs.score;
        }

    };

private:

    std::size_t LAH = 3;
    std::size_t STONE_NUM;
    problem_type pre_problem;
    void one_try(problem_type problem, int y, int x, std::size_t const rotate);
    double evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    int search(std::vector<search_type>& sv, search_type s, std::size_t const ishi);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search,stone_type const& stone);
};

#endif // READAHEAD

