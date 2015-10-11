#include "stikcy_beam.hpp"
#include "evaluator.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <exception>
#include <stdexcept>
#include <mutex>
#include <boost/format.hpp>
#include <QFile>
#include <QVector>
#include <QIODevice>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentMap>
#include <QFuture>

sticky_beam::sticky_beam(problem_type _problem, evaluator eval) : eval(eval),sticky_problem(_problem)
{
    ALL_STONES_NUM = _problem.stones.size();
    algorithm_name = "sticky_beam";
}

sticky_beam::~sticky_beam()
{
}

void sticky_beam::run()
{
    qDebug("sticky_beam start");
    QVector<std::tuple<int,int,std::size_t,int>> data;
    std::array<int,39> start_y{{24,-7,29,13,-1,9,30,19,15,-4,18,17,20,-2,23,10,4,-6,7,-5,31,3,8,21,5,16,-3,0,11,22,14,27,2,26,28,1,12,6,25}};
    std::array<int,39> start_x{{5,19,13,26,16,20,8,10,3,23,2,27,0,29,6,-5,-7,11,-4,7,22,-3,1,-6,-2,24,-1,30,31,9,14,25,17,28,21,15,18,12,4}};


    for(NOW_FIRST_STONE_NUM = 0; NOW_FIRST_STONE_NUM < sticky_problem.stones.size(); ++NOW_FIRST_STONE_NUM)
    {
        stone_type first_stone = sticky_problem.stones[NOW_FIRST_STONE_NUM];
        for(std::size_t i = 0; i < start_x.size(); ++i) for(int side = 0; side < 2; ++side)
        {
            data.clear();
            for(std::size_t j = 0; j < start_x.size(); ++j) for(std::size_t angle = 0; angle < 360; angle += 90)
            {
                int y = start_y[j];
                int x = start_x[(j+i < start_x.size()) ? j+i : j+i-start_x.size()];
                first_stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
                if(sticky_problem.field.is_puttable_basic(first_stone,y,x) == true)
                {
                    data.push_back(std::make_tuple(y,x,angle,side));
                    //one_try_ins.one_try_run(y,x,angle,side);
                }
            }
            QFuture<void> threads = QtConcurrent::map(
                        data,
                        [this](auto& each_start)
                        {
                            one_try(sticky_problem,eval,this).one_try_run(std::get<0>(each_start),std::get<1>(each_start),std::get<2>(each_start),std::get<3>(each_start));
                        });
                    threads.waitForFinished();
        }
    }
}


sticky_beam::one_try::one_try(problem_type _problem, evaluator _eval, sticky_beam* _parent) : origin_problem(_problem), eval(_eval), parent_class(_parent)
{
    holding_problems.reserve(HOLD_FIELD_NUM);
}

sticky_beam::one_try::~one_try()
{
}

void sticky_beam::one_try::one_try_run(int y, int x, std::size_t angle, int side)
{
    //1つめ
    holding_problems.clear();
    holding_problems.emplace_back(origin_problem,0);
    stone_type& stone = holding_problems.front().problem.stones[parent_class->NOW_FIRST_STONE_NUM].set_angle(angle).set_side(static_cast<stone_type::Sides>(side));
    holding_problems.front().problem.field.put_stone_basic(stone,y,x);

    //石の数ループ
    for(now_put_stone_num = parent_class->NOW_FIRST_STONE_NUM+1; now_put_stone_num < holding_problems[0].problem.stones.size(); ++now_put_stone_num)
    {
        //保持するフィールドの数ループ
        for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num)
        {
            put_a_stone(field_num, now_put_stone_num);
        }

        //次男が居ない
        if(second_sons.size() == 0) continue;
        //今までの最悪値
        auto worst_element = std::min_element(holding_problems.begin(),holding_problems.end(),[](auto const& lhs, auto const&rhs)
        {
            return lhs.score < rhs.score;
        });
        //際優良次男
        auto const& best_second_son = std::max_element(second_sons.begin(),second_sons.end(),[](const auto& lhs, const auto& rhs)
        {
            return lhs.first_put->score < rhs.first_put->score;
        });
        holding_problems[best_second_son->field_num].problem.stones.at(now_put_stone_num).set_side(best_second_son->first_put->side).set_angle(best_second_son->first_put->angle);
        //置き換える場合
        if(worst_element->score < best_second_son->first_put->score)
        {
            worst_element->problem.field = holding_problems[best_second_son->field_num].problem.field;
            worst_element->problem.field.remove_stone_basic();
            holding_problems[best_second_son->field_num].problem.stones.at(now_put_stone_num).set_angle(best_second_son->first_put->angle).set_side(best_second_son->first_put->side);
            worst_element->problem.field.put_stone_basic(holding_problems[best_second_son->field_num].problem.stones.at(now_put_stone_num),
                    best_second_son->first_put->point.y, best_second_son->first_put->point.x);
            worst_element->score = best_second_son->first_put->score;
        }

        second_sons.clear();
//#ifdef QT_DEBUG
//        std::cout << "now_put_stone_num = " << now_put_stone_num << std::endl;
//        std::cout << "holding problems = " << holding_problems.size() << std::endl;
//#endif
    }

    for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num)
    {
        qDebug("emit sticky-beam. score = %3zu",holding_problems[field_num].problem.field.get_score());
        parent_class->answer_send(holding_problems[field_num].problem.field);
    }
}


//1つの石を置く
void sticky_beam::one_try::put_a_stone(int field_num, int stone_num)
{
    problem_type& problem = holding_problems[field_num].problem;
    std::size_t i = 0;
    std::shared_ptr<node> first_put1;
    std::shared_ptr<node> first_put2;

    std::shared_ptr<node> root (new node(NULL,stone_num,{0,0},0,stone_type::Sides::Head,/*eval.min_value*/std::numeric_limits<double>::min(),MAX_SEARCH_DEPTH));

    search(problem.field, field_num, stone_num, root);

    /*
    for(auto& each : result_vec[field_num])
    {
        first_put1 = each;
        while(first_put1->stone_num > now_put_stone_num)
        {
            first_put1 = first_put1->parent;
        }
        std::cout << "each" << first_put1->point.y << " " << first_put1->point.x << " " << first_put1->angle << " " << std::endl;
    }
*/
    //自分の長男残す
    for(i = 0; i < result_vec[field_num].size(); ++i)
    {
        auto eldest_son = std::max_element(result_vec[field_num].begin(),result_vec[field_num].end(),[](const auto& lhs, const auto& rhs)
        {
            return lhs->score < rhs->score;
        });

        // 親を遡りはじめに置いた石のnodeを得る
        first_put1 = *eldest_son;
        while(first_put1->stone_num > now_put_stone_num)
        {
            first_put1 = first_put1->parent;
        }

        //パスすべきなら次へ
        problem.stones.at(stone_num).set_side(first_put1->side).set_angle(first_put1->angle);
        if(eval.should_pass(problem.field,
                            {problem.stones.at(stone_num),{first_put1->point.y, first_put1->point.x}},
                            parent_class->get_rem_stone_zk(stone_num+1))== true)
        {
            //eldest_son->get()->score -= /*eval.min_value*/std::numeric_limits<double>::min() / 2;
            eldest_son->get()->score -= 500;
            continue;
        }

        //長男を置く
        problem.field.put_stone_basic(problem.stones.at(stone_num), first_put1->point.y, first_put1->point.x);
        holding_problems[field_num].score = first_put1->score;
        //std::cout << "tyounan = " << first_put1->point.y << " " << first_put1->point.x << " " << first_put1->angle << std::endl;
        break;
    }

/*
    for(auto& each : result_vec[field_num])
    {
        first_put2 = each;
        while(first_put2->stone_num > now_put_stone_num)
        {
            first_put2 = first_put2->parent;
        }
        std::cout << "each" << first_put2->point.y << " " << first_put2->point.x << " " << first_put2->angle << " " << each->score << std::endl;
    }
*/
    //次男が居れば保存する
    for(i = 0; i < result_vec[field_num].size(); ++i)
    {
        auto second_son = std::max_element(result_vec[field_num].begin(),result_vec[field_num].end(),[](const auto& lhs, const auto& rhs)
        {
            return lhs->score < rhs->score;
        });

        // 親を遡りはじめに置いた石のnodeを得る
        first_put2= *second_son;
        while(first_put2->stone_num > now_put_stone_num)
        {
            first_put2 = first_put2->parent;
        }

        //std::cout << "jinan kouho = " << first_put2->point.y << " " << first_put2->point.x << " " << first_put2->angle << " " << second_son->get()->score << std::endl;

        //同じ手は要らない
        if(first_put1 == first_put2 || first_put2== NULL)
        {
            second_son->get()->score -= 10000;
            continue;
        }

        //パスすべきなら次へ
        problem.stones.at(stone_num).set_side(first_put2->side).set_angle(first_put2->angle);
        if(eval.should_pass(problem.field,
                            {problem.stones.at(stone_num),{first_put2->point.y, first_put2->point.x}},
                            parent_class->get_rem_stone_zk(stone_num+1))== true)
        {
            second_son->get()->score = std::numeric_limits<double>::min();
            continue;
        }

        second_sons_mutex.lock();
        second_sons.push_back(node_with_field_num{first_put2,field_num});
        second_sons_mutex.unlock();
        break;
    }
    result_vec[field_num].clear();
}

//おける場所の中から評価値の高いものつをMAX_SEARCH_WIDTH選びsearch_depthまで潜る
int sticky_beam::one_try::search(field_type& _field, int field_num, std::size_t const stone_num, std::shared_ptr<node> parent)
{
    if(parent->search_depth == 0) return 0;
    //std::cout << "now_put_stone_num = " << now_put_stone_num << " stone_num" << stone_num << " parent->search_dpeth = " << parent->search_depth << std::endl;
    stone_type& stone = holding_problems[field_num].problem.stones.at(stone_num);
    std::vector<std::shared_ptr<node>> nodes;
    nodes.reserve(MAX_SEARCH_WIDTH);

    //if(result_vec[field_num].size() == 0)
    //おける可能性がある場所すべてにおいてみる
    for(int y = 1 - STONE_SIZE; y < FIELD_SIZE; ++y) for(int x = 1 - STONE_SIZE; x < FIELD_SIZE; ++x) for(std::size_t angle = 0; angle < 360; angle += 90) for(int side = 0; side < 2; ++side)
    {
        stone.set_angle(angle).set_side(static_cast<stone_type::Sides>(side));

        if(_field.is_puttable_basic(stone,y,x) == true)
        {
            // move_goodnessは2種類ある　最後の石かどうか判定が必要
            const double score = stone_num == holding_problems[field_num].problem.stones.size() - 1 ?
                        eval.move_goodness(_field,{stone,{y,x}}) :
                        eval.move_goodness(_field,{stone,{y,x}},holding_problems[field_num].problem.stones.at(stone_num+1));

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
                                now_put_stone_num == stone_num ? eval.search_depth(_field, {stone,{y,x}}) : parent->search_depth
                                )
                            );
#ifdef QT_DEBUG
                //if(now_put_stone_num == stone_num ) std::cout << "field_num = " << field_num << "search_depth = " << eval.search_depth(_field, {stone,{y,x}}) << std::endl;
#endif
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

    if(nodes.size() == 0) return 0;
    //探索の最下層だったら結果をresult_vec入れる
    else if(stone_num - now_put_stone_num >= parent->search_depth - 1 || stone_num >= parent_class->ALL_STONES_NUM-1)
    {
        auto max = std::max_element(nodes.begin(),nodes.end(),[](auto const&lhs, auto const& rhs)
        {
            return lhs->score < rhs->score;
        });
        if(*max != NULL && max->get()->stone_num == stone_num) result_vec[field_num].push_back(std::move(*max));
    }
    //そうでなければ石を置いて潜る、帰ってきたら石を取り除く
    else
    {
        for(auto& each_node : nodes)
        {
            stone.set_angle(each_node->angle).set_side(each_node->side);
            _field.put_stone_basic(stone,each_node->point.y,each_node->point.x);
            if(search(_field, field_num, stone_num+1, each_node) == 0)
            {
                result_vec[field_num].emplace_back(each_node);
            }
            _field.remove_stone_basic();
        }
    }
    //std::cout << "depth = " << parent->stone_num - now_put_stone_num + 1 << " branch = " << nodes.size() << std::endl;
    return nodes.size();
}
