#ifndef RAQ_HPP

#define RAQ_HPP
#include <takao.hpp>
#include <array>

class raq : public algorithm_type
{
    Q_OBJECT
public:
    raq(problem_type _problem);
    ~raq();
    void run();

private:
    struct search_type
    {
        field_type field;
        int score;
    };

    problem_type pre_problem;
    void one_step(field_type& _field, int ishi,std::vector<search_type>& sv);
    void steps(std::vector<raq::search_type> result);



    int evaluate(field_type const& field, stone_type stone,int const i, int const j)const;
    int evaluate(field_type const& field, int const n,int const i, int const j)const;
    search_type search(field_type& _field, stone_type& stone);
    int get_island(field_type::raw_field_type field);
    bool pass(search_type const& search, stone_type const& stone);
    bool pass(double score, stone_type const& stone);
};


#endif // RAQ_HPP

