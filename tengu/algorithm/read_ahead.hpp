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

    struct search_type
    {
        field_type field;
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        int score = 0;
        int rank = 1;
    };

private:

    int LAH = 3;
    std::size_t STONE_NUM;
    problem_type pre_problem;
    void one_try(problem_type problem, int y, int x, std::size_t const rotate);
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    void search(std::vector<search_type>& sv, search_type s, std::size_t const ishi);
};

#endif // READAHEAD

