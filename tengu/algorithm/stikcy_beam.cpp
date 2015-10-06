#include "stikcy_beam.hpp"
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

sticky_beam::sticky_beam(problem_type _problem)
{
    algorithm_name = "sticky_beam";
    ALL_STONES_NUM = _problem.stones.size();
    holding_problems.reserve(HOLD_FIELD_NUM);
    holding_problems.emplace_back(_problem,0);
}

sticky_beam::sticky_beam(problem_type _problem, evaluator eval):eval(eval)
{
    algorithm_name = "sticky_beam";
    ALL_STONES_NUM = _problem.stones.size();
    holding_problems.reserve(HOLD_FIELD_NUM);
    holding_problems.emplace_back(_problem,0);
}

sticky_beam::~sticky_beam()
{
}

/*
void sticky_beam::run(){
    QElapsedTimer timer;
    timer.start();

    std::vector<field_with_score_type> pattern;
    pattern.emplace_back(problem.field,0);
    size_t rem_stone_num = problem.stones.size();

    for(auto stone_itr = problem.stones.begin(); stone_itr != problem.stones.end(); stone_itr++)
    {
        eval_pattern(*stone_itr,*stone_itr);
        print_text(std::to_string(rem_stone_num--));
    }

    field_with_score_type& best_ans = *std::min_element(pattern.begin(),pattern.end(),[](auto  const& lhs, auto const& rhs)
    {
        return lhs.field.get_score() < rhs.field.get_score();
    });

    int time = timer.elapsed();
    print_text(std::to_string(time) + "msかかった");
    answer_send(best_ans.field);
}
*/
/*
void sticky_beam::eval_pattern(stone_type& stone, stone_type& next_stone)
{
    result_stone.clear();
    std::vector<stone_params_type> stone_placement_vector;
    stone_placement_vector.reserve(MAX_SEARCH_WIDTH);
    for(field_with_score_type& _eval_field : holding_fields)
    {
        for(int flip = 0; flip <= 1 ;flip++,stone.flip())for(int angle = 0; angle <= 270;angle += 90,stone.rotate(90))for(int dy=-7;dy<=32;dy++)for(int dx=-7;dx<=32;dx++)
        {
            if(_eval_field.field.is_puttable_basic(stone,dy,dx))
            {
                double score;
                bool should_pass;
                if(non_next_stone){
                    score = eval.move_goodness(_eval_field.field,process_type(stone,{dy,dx}));
                    //score = light_eval(_eval_field.field,process_type(stone,{dy,dx}));
                    should_pass = false;
                }else{
                    score = eval.move_goodness(_eval_field.field,process_type(stone,{dy,dx}),next_stone);
                    //score = light_eval(_eval_field.field,process_type(stone,{dy,dx}));
                    should_pass = eval.should_pass(_eval_field.field,process_type(stone,{dy,dx}),get_rem_stone_zk(stone));
                    //should_pass = false;
                }
                //ビームサーチの幅制限
                if(stone_placement_vector.size() < MAX_SEARCH_WIDTH)
                {
                    if(should_pass){
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field);
                    }else{
                        stone_placement_vector.emplace_back(dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field);
                    }
                }else{
                    auto worst_stone_param = std::min_element(stone_placement_vector.begin(),stone_placement_vector.end(),[](auto const &t1, auto const &t2){return t1.score < t2.score;});
                    if(should_pass){
                        if(_eval_field.score > worst_stone_param->score){
                            (*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field};
                        }
                    }else{
                        if(score > worst_stone_param->score){
                            (*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field};
                        }
                    }
                }
            }
        }
    }
    for(stone_params_type& stone_params : stone_placement_vector){
        if(stone_params.pass){
            result_stone.emplace_back(*(stone_params.field),stone_params.score);
        }else{
            field_type field = *(stone_params.field);
            result_stone.push_back({field.put_stone_basic(stone.set_angle(stone_params.angle).set_side(stone_params.side),stone_params.dy,stone_params.dx),stone_params.score});
        }
    }
}
*/
/*
double sticky_beam::light_eval(field_type &field, process_type process){
    field.put_stone_basic(process.stone,process.position.y,process.position.x);
    double score = field.evaluate_normalized_complexity();
    field.remove_stone_basic();
    return score;
}
*/

void sticky_beam::run()
{
    qDebug("sticky_beam start");

    //石の数ループ
    for(; now_put_stone_num < holding_problems[0].problem.stones.size(); ++now_put_stone_num)
    {
        //保持するフィールドの数ループ
        for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num)
        {
            std::cout << now_put_stone_num << std::endl;
            put_a_stone(holding_problems[field_num].problem, field_num, now_put_stone_num);
        }
        for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num) result_vec[field_num].clear();
        /*
        for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num)
        {
            holding_problems[field_num].problem.field
            std::cout << now_put_stone_num << std::endl;
            put_a_stone(holding_problems[field_num].problem, field_num, now_put_stone_num);
        }
        */
    }

    for(std::size_t field_num = 0; field_num < holding_problems.size(); ++field_num)
    {
        qDebug("emit only one try. score = %3zu",holding_problems[field_num].problem.field.get_score());
        answer_send(holding_problems[field_num].problem.field);
    }
}


//1つの石を置く
void sticky_beam::put_a_stone(problem_type& problem, int field_num, int stone_num)
{
    std::shared_ptr<node> root (new node(NULL,stone_num,{0,0},0,stone_type::Sides::Head,/*eval.min_value*/std::numeric_limits<double>::min(),MAX_SEARCH_DEPTH));

    search(problem.field, field_num, stone_num, root);

    for(std::size_t i = 0; i < result_vec[field_num].size(); ++i)
    {
        auto max = std::max_element(result_vec[field_num].begin(),result_vec[field_num].end(),[](const auto& lhs, const auto& rhs)
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
                            {problem.stones.at(stone_num),{first_put->point.y, first_put->point.x}},
                            get_rem_stone_zk(stone_num+1))== true)
        {
            max->get()->score = /*eval.min_value*/std::numeric_limits<double>::min();
            continue;
        }

        problem.field.put_stone_basic(problem.stones.at(stone_num), first_put->point.y, first_put->point.x);
        break;
    }
}

//おける場所の中から評価値の高いものつをMAX_SEARCH_WIDTH選びsearch_depthまで潜る
int sticky_beam::search(field_type& _field, int field_num, std::size_t const stone_num, std::shared_ptr<node> parent)
{
    stone_type& stone = holding_problems[field_num].problem.stones.at(stone_num);
    std::vector<std::shared_ptr<node>> nodes;
    nodes.reserve(MAX_SEARCH_WIDTH);

    if(result_vec[field_num].size() == 0)

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
                                stone_num == parent->stone_num ? eval.search_depth(_field, {stone,{y,x}}) : parent->search_depth
                                )
                            );
#ifdef QT_DEBUG
                //if(stone_num == parent->stone_num) std::cout << "search_depth = " << eval.search_depth(_field, {stone,{y,x}}) << std::endl;
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
    else if(parent->stone_num - now_put_stone_num >= parent->search_depth - 2 || stone_num >= ALL_STONES_NUM-1)
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
