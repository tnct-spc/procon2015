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

private:

    struct search_type
    {
        field_type field;
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        int score = 0;
        int t_score = 0;
        int rank = 1;
    };

    problem_type pre_problem;
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    //std::queue<read_ahead::search_type> search(std::queue<search_type> queue, field_type& _field, int const ishi);
    void search(std::vector<search_type>& sv, search_type s, std::size_t const ishi);
};

#endif // READAHEAD

