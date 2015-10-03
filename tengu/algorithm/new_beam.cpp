#include "new_beam.hpp"
#include "evaluator.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <exception>
#include <stdexcept>
#include <boost/format.hpp>
#include <QFile>
#include <QVector>
#include <QIODevice>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QFuture>

new_beam::new_beam(problem_type _problem)
{
    origin_problem = _problem;
    algorithm_name = "new_beam";
    ALL_STONES_NUM = origin_problem.stones.size();

}
new_beam::new_beam(problem_type _problem, evaluator eval):eval(eval)
{
    origin_problem = _problem;
    algorithm_name = "new_beam";
    ALL_STONES_NUM = origin_problem.stones.size();
}

new_beam::~new_beam()
{
}



void new_beam::run()
{
    qDebug("new_beam start");
/*
    //はじめに置く石の場所、角度、反転分のループ
    for(int y = 1-STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1-STONE_SIZE; x  < FIELD_SIZE; ++x) for(int angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        origin_problem.stones.at(0).set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
        //置ければ始める
        if(origin_problem.field.is_puttable(origin_problem.stones.front(),y,x) == true)
        {
            one_try(origin_problem,y,x,angle,side);
        }
    }
*/
    only_one_try(origin_problem);

}

//はじめに置く石から探索を開始する
void new_beam::only_one_try(problem_type problem)
{
    std::cout << "start only one try." << std::endl;
    for(std::size_t stone_num = 0; stone_num < problem.stones.size(); ++stone_num)
    {
        std::shared_ptr<node> root (new node(NULL,stone_num,{0,0},0,stone_type::Sides::Head,eval.min_value));
        //std::cout << "stone_num = " << stone_num << std::endl;

        search(problem.field, stone_num, root);
        //std::cout << "再帰抜けた result_vec.size() = " << result_vec.size() << std::endl;

        for(std::size_t i = 0; i < result_vec.size(); ++i)
        {
            auto max = std::max_element(result_vec.begin(),result_vec.end(),[](const auto& lhs, const auto& rhs)
            {
                return lhs->score < rhs->score;
            });
            //std::cout << "max score = " << max->get()->score << " decied max" << std::endl;
/*
            if(max->get()->stone_num < stone_num)
            {
                std::cout << "dame" << std::endl;
                continue;
            }
*/
            // 親を遡りはじめに置いた石のnodeを得る
            auto first_put= *max;
            /*
            if(first_put == NULL)
            {
                std::cout << "first _put = NULL" << std::endl;
                continue;
            }
            */
            //std::cout << first_put->stone_num << std::endl;
            while(first_put->stone_num > now_put_stone_num)
            {
                //std::cout << first_put->stone_num << std::endl;
                first_put = first_put->parent;
            }

            problem.stones.at(stone_num).set_side(first_put->side).set_angle(first_put->angle);
            if(eval.should_pass(problem.field,
                                {problem.stones.at(stone_num),{first_put->point.y, first_put->point.x}},
                                get_rem_stone_zk(stone_num+1))== true)
            {
                max->get()->score = eval.min_value;
                //std::cout << "pass" << std::endl;
                continue;
            }
            /*
            if(problem.field.is_puttable_basic(problem.stones.at(stone_num), first_put->point.y, first_put->point.x) == false)
            {
                std::cout << "can't put" << std::endl;
            }
            */
            problem.field.put_stone_basic(problem.stones.at(stone_num), first_put->point.y, first_put->point.x);
            std::cout << stone_num << "th stone putted" << std::endl;
            break;
        }
        result_vec.clear();
        now_put_stone_num++;
    }
    qDebug("emit only one try. score = %3zu",problem.field.get_score());
    answer_send(problem.field);
}

//おける場所の中から評価値の高いもの3つを選びsearch_depthまで潜る
int new_beam::search(field_type& _field, std::size_t const stone_num, std::shared_ptr<node> parent)
{
    stone_type& stone = origin_problem.stones.at(stone_num);
    std::vector<std::shared_ptr<node>> nodes;
    nodes.reserve(MAX_SEARCH_WIDTH);

    if(result_vec.size() == 0)

    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            //if(stone_num >= 33) std::cout << "haitta" << std::endl;
            //const double score = stone_num == origin_problem.stones.size() - 1 ? eval.move_goodness(_field,{stone,{y,x}}) : eval.move_goodness(_field,{stone,{y,x}},origin_problem.stones.at(stone_num+1));
            double score;
            if(stone_num == origin_problem.stones.size() - 1)
            {
                //std::cout << "last stone" << std::endl;
                score = eval.move_goodness(_field,{stone,{y,x}});
            }
            else
            {
                score = eval.move_goodness(_field,{stone,{y,x}},origin_problem.stones.at(stone_num+1));
            }

            //置けたら接してる辺を数えて配列に挿入
            if(nodes.size() < MAX_SEARCH_WIDTH) //MAX_SEARCH_WIDTH個貯まるまでは追加する
            {
                nodes.emplace_back(
                            new node(
                                parent,
                                stone_num,
                                point_type{y,x},
                                angle,
                                static_cast<stone_type::Sides>(side),
                                score)
                            );
                //if(stone_num < now_put_stone_num) throw std::runtime_error("This stone is wrong");
            }
            else
            {
                //保持している中での最悪手
                auto worst = std::min_element(nodes.begin(),nodes.end(),[](auto const&lhs, auto const& rhs)
                    {
                        return lhs->score < rhs->score;
                    });
                if(worst->get()->score < score) //保持している中の最悪手より良い
                {
                    worst->get()->point = {y,x};
                    worst->get()->angle = angle;
                    worst->get()->side = static_cast<stone_type::Sides>(side);
                    worst->get()->score = score;
                }
            }
        }
    }

    //for(auto& each : nodes) if(each->stone_num < stone_num) throw std::runtime_error("This element eroor");

    //探索の最下層だったら結果をresult_vec入れる
    if(parent->stone_num - now_put_stone_num >= MAX_SEARCH_DEPTH - 2 || stone_num >= ALL_STONES_NUM-1)
    {
        auto max = std::max_element(nodes.begin(),nodes.end(),[](auto const&lhs, auto const& rhs)
        {
            return lhs->score < rhs->score;
        });
        if(*max != NULL && max->get()->stone_num == stone_num) result_vec.push_back(std::move(*max));
    }
    //そうでなければ石を置いて潜る、帰ってきたら石を取り除く
    else
    {
        for(auto& each_node : nodes)
        {
            stone.set_angle(each_node->angle).set_side(each_node->side);
            _field.put_stone_basic(stone,each_node->point.y,each_node->point.x);
            if(search(_field, stone_num+1, each_node) == 0)
            {
                result_vec.emplace_back(each_node);
            }
            _field.remove_stone_basic();
        }
    }
    //std::cout << "depth = " << parent->stone_num - now_put_stone_num + 1 << " branch = " << nodes.size() << std::endl;
    return nodes.size();
}

int new_beam::get_rem_stone_zk(int stone_num)
{
    int sum = 0;
    for(std::size_t i = stone_num; i < origin_problem.stones.size(); ++i)
    {
        sum += origin_problem.stones.at(i).get_area();
    }
    return sum;
}
