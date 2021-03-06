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

    only_one_try(origin_problem);

}

//はじめに置く石から探索を開始する
void new_beam::only_one_try(problem_type problem)
{
    for(std::size_t stone_num = 0; stone_num < problem.stones.size(); ++stone_num)
    {
        std::shared_ptr<node> root (new node(NULL,stone_num,{0,0},0,stone_type::Sides::Head,std::numeric_limits<double>::min(),MAX_SEARCH_DEPTH));

        search(problem.field, stone_num, root);

        for(std::size_t i = 0; i < result_vec.size(); ++i)
        {
            auto max = std::max_element(result_vec.begin(),result_vec.end(),[](const auto& lhs, const auto& rhs)
            {
                return lhs->score < rhs->score;
            });

            // 親を遡りはじめに置いた石のnodeを得る
            auto first_put= *max;
            while(first_put->stone_num > now_put_stone_num)
            {
                first_put = first_put->parent;
            }

            problem.stones.at(stone_num).set_side(first_put->side).set_angle(first_put->angle);
            if(eval.should_pass(problem.field,
                                origin_problem.stones,
                                bit_process_type(stone_num+1,static_cast<int>(first_put->side),first_put->angle,first_put->point),
                                get_rem_stone_zk(stone_num+1))== true)
            {
                max->get()->score = std::numeric_limits<double>::min();
#ifdef QT_DEBUG
                if(i == result_vec.size() - 1) std::cout << stone_num << "th stone passed" << std::endl;
#endif
                continue;
            }
            problem.field.put_stone_basic(problem.stones.at(stone_num), first_put->point.y, first_put->point.x);
#ifdef QT_DEBUG
            std::cout << stone_num << "th stone putted" << std::endl;
#endif
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

    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            // move_goodnessは2種類ある　最後の石かどうか判定が必要
            double const score = eval.move_goodness(_field,
                                                    origin_problem.stones,
                                                    bit_process_type(stone_num+1,side,angle,point_type{y,x}));
            //MAX_SEARCH_WIDTH個貯まるまでは追加する
            if(nodes.size() < MAX_SEARCH_WIDTH)
            {
                nodes.emplace_back(
                            new node(
                                parent,
                                stone_num,
                                point_type{y,x},
                                angle,
                                static_cast<stone_type::Sides>(side),
                                score,
                                stone_num == parent->stone_num ? eval.search_depth(_field,origin_problem.stones,bit_process_type(stone_num+1,angle,side,point_type{y,x})) : parent->search_depth
                                )
                            );
#ifdef QT_DEBUG
                if(stone_num == parent->stone_num) std::cout << "search_depth = " << eval.search_depth(_field,origin_problem.stones,bit_process_type(stone_num+1,angle,side,point_type{y,x})) << std::endl;
#endif
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

    if(nodes.size() == 0) return 0;
    //探索の最下層だったら結果をresult_vec入れる
    else if(parent->stone_num - now_put_stone_num >= parent->search_depth - 2 || stone_num >= ALL_STONES_NUM-1)
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
            _field.remove_stone_basic(stone);
        }
    }
    return nodes.size();
}
