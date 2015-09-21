#ifndef YRANGE3_HPP

#define YRANGE3_HPP
#include <takao.hpp>
#include <array>

class yrange3 : public algorithm_type
{
    Q_OBJECT
public:
    yrange3(problem_type _problem);
    ~yrange3();
    void run();

private:
    struct search_type
    {
        point_type point;
        std::size_t rotate;
        stone_type::Sides flip;
        int score;
        std::size_t stone;
    };

    problem_type pre_problem;
    void one_try(problem_type& problem, int x, int y, std::size_t const rotate);
    search_type search(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);
    bool pass(double const score, stone_type const& stone, double threshold);
    int nonput_evaluate(field_type const& field, stone_type const& stone, int y, int x) const;
    int evaluate_reverse(field_type const& field, stone_type stone,int const i, int const j)const;
    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    void ur_atack(field_type& field,std::vector<stone_type>& stones);
    void ul_atack(field_type& field, std::vector<stone_type>& stones);
    void dr_atack(field_type& field,std::vector<stone_type>& stones);
    void dl_atack(field_type& field,std::vector<stone_type>& stones);
    void atack(int y, int x, stone_type& stone, std::size_t stone_num, int &count, field_type& field, search_type& best, int wight);
};


#endif // YRANGE3_HPP

