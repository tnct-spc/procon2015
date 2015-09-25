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

    struct stone_info_type
    {
        point_type point;
        std::size_t angle;
        stone_type::Sides side;
    };

    struct search_type
    {
        field_type field;
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        double score = 0;
        int rank = 1;
        int island;
        search_type(field_type field,point_type point,std::size_t rotate,stone_type::Sides flip,double score,int rank,int island):field(field),point(point),rotate(rotate),flip(flip),score(score),rank(rank),island(island){};
        search_type(){};
        friend inline bool operator== (search_type const& lhs, search_type const& rhs)
        {
            return lhs.point == rhs.point &&
                   lhs.rotate == rhs.rotate &&
                   lhs.flip == rhs.flip &&
                   lhs.rank == rhs.rank &&
                   lhs.score == rhs.score;
        }

    };

private:

    int LAH = 3;
    std::size_t STONE_NUM;
    problem_type pre_problem;
    void one_try(problem_type problem, int y, int x, std::size_t const rotate);
    double evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    int search(std::vector<search_type>& sv, search_type s, std::size_t const ishi);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search,stone_type const& stone);
};

#endif // READAHEAD

