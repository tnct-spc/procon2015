#include "strange.hpp"
#include <limits>
#include <sstream>
#include <algorithm>
#include <random>
#include "QDebug"
#include "QString"
#include "QElapsedTimer"
#include <boost/range/algorithm_ext/erase.hpp>
#include "field_type.hpp"
#include "QtConcurrent/QtConcurrent"

strange::strange(problem_type _problem) :
    origin_problem(_problem), problem(_problem)
{
    algorithm_name = "strange";
}
strange::strange(problem_type _problem, evaluator _eval) :
    _evaluator(_eval), origin_problem(_problem), problem(_problem)
{
    algorithm_name = "strange";
}
strange::~strange()
{
}

std::vector<stranger::field_with_score_type>
strange::force_put(stone_type& stone, std::vector<stranger::field_with_score_type> pattern, bit_process_type process)
{
    int bak_angle = stone.get_angle();
    stone_type::Sides bak_side = stone.get_side();

    stone.set_angle(process.angle);
    stone.set_side(static_cast<stone_type::Sides>(process.flip));

    field_type field = pattern.at(0).field;
    field.put_stone_basic(stone, process.position.y, process.position.x);

    stone.set_angle(bak_angle);
    stone.set_side(bak_side);

    pattern.emplace_back(field, 0.0);
    return std::move(pattern);
}

std::vector<stranger::field_with_score_type>
strange::eval_pattern(stone_type& stone, std::vector<stranger::field_with_score_type> pattern, int search_width)
{
    std::vector<stranger::stone_params_type> beam_params; // stone_placement_vector
    beam_params.reserve(search_width);
    for(stranger::field_with_score_type& _eval_field : pattern) {
        for(int flip = 0; flip <= 1 ; flip++, stone.flip()) for(int angle = 0; angle <= 270; angle += 90, stone.rotate(90)) for(int dy = -7; dy <= 32; dy++) for(int dx = -7; dx <= 32; dx++) {
            if(_eval_field.field.is_puttable_basic(stone, dy, dx)) {
                bool should_pass = false;
                //should_pass = _evaluator.should_pass(_eval_field.field,origin_problem.stones,bit_process_type(stone.get_nth(),flip,angle,{dy,dx}),get_rem_stone_zk(stone));
                double score = _evaluator.move_goodness(_eval_field.field,
                                                        origin_problem.stones,
                                                        bit_process_type(stone.get_nth(), flip, angle, {dy, dx}));
                //ビームサーチの幅制限
                if(beam_params.size() < (std::size_t)search_width) {
                    if(should_pass) {
                        beam_params.emplace_back(dy, dx, angle, static_cast<stone_type::Sides>(flip), _eval_field.score, true, &_eval_field.field);
                    } else {
                        beam_params.emplace_back(dy, dx, angle, static_cast<stone_type::Sides>(flip), score, false, &_eval_field.field);
                    }
                } else {
                    // 最悪な要素と差し替え
                    // 追加して最悪な要素1つを削除するのと等価
                    auto worst_stone_param =
                            std::min_element(beam_params.begin(),
                                             beam_params.end(),
                                             [](auto const &t1, auto const &t2) { return t1.score < t2.score; });
                    if(should_pass) {
                        if(_eval_field.score > worst_stone_param->score) {
                            //(*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),_eval_field.score,true,&_eval_field.field};
                            (*worst_stone_param) = stranger::stone_params_type(dy, dx, angle, static_cast<stone_type::Sides>(flip), _eval_field.score, true, &_eval_field.field);
                        }
                    } else if(score > worst_stone_param->score) {
                            //(*worst_stone_param) = {dy,dx,angle,static_cast<stone_type::Sides>(flip),score,false,&_eval_field.field};
                            (*worst_stone_param) = stranger::stone_params_type(dy, dx, angle, static_cast<stone_type::Sides>(flip), score, false, &_eval_field.field);
                    }
                }
            }
        }
    }

    // stone_params_typeからfield_with_score_typeに変換
    std::vector<stranger::field_with_score_type> fields;
    for(stranger::stone_params_type& stone_params : beam_params) {
        if(stone_params.pass) {
            fields.emplace_back(*(stone_params.field), stone_params.score);
        } else {
            field_type field = *(stone_params.field);
            field.put_stone_basic(stone.set_angle(stone_params.process.angle).set_side(static_cast<stone_type::Sides>(stone_params.process.flip)), stone_params.process.position.y, stone_params.process.position.x);
            if(field.processes.size() == 1)
                field.init_route_map();
            fields.push_back({field, stone_params.score});
        }
    }
    if(fields.size() == 0)
        return std::move(pattern);
    return std::move(fields);
}

void strange::run()
{
    std::vector<bit_process_type> init_moves = [this]() -> std::vector<bit_process_type>
    {
        std::vector<bit_process_type> ret;
        field_type& field = problem.field;
        stone_type& stone = problem.stones.at(0);
        stone_type::Sides bak_side = stone.get_side();
        int bak_angle = stone.get_angle();
        for(int x = -7; x < 32; x++)
            for(int y = -7; y < 32; y++)
                for(int flip = 0; flip < 2; flip++) {
                    stone.set_side(static_cast<stone_type::Sides>(flip));
                    for(int rotate = 0; rotate < 4; rotate++) {
                        stone.set_angle(rotate * 90);
                        if(field.is_puttable_basic(stone, y, x))
                            ret.emplace_back(1, flip, rotate * 90, point_type{y, x});
                    }
                }
        stone.set_side(bak_side);
        stone.set_angle(bak_angle);
        return std::move(ret);
    }();
    std::shuffle(init_moves.begin(), init_moves.end(), std::mt19937());

    unsigned int best_score = FIELD_SIZE * FIELD_SIZE; // smaller is better
    for(int i = 0; i < init_moves.size(); i++) {
        print_text(std::to_string(i + 1) + "/" + std::to_string(init_moves.size()));
        QElapsedTimer timer;
        timer.start();
        std::vector<stranger::field_with_score_type> pattern;
        pattern.emplace_back(problem.field, 0);

        // saisho no ishi
        pattern = force_put(problem.stones.at(0), std::move(pattern), init_moves.at(i));

        // 2banme ikou no ishi
        for(auto it = problem.stones.begin() + 1; it != problem.stones.end(); it++) {
            pattern = eval_pattern(*it, std::move(pattern), SEARCH_WIDTH);
        }

        stranger::field_with_score_type best_ans =
                *std::min_element(pattern.begin(), pattern.end(),
                                  [](auto  &t1, auto  &t2) { return t1.field.get_score() < t2.field.get_score(); });

        int time = timer.elapsed();
        print_text(std::to_string(time) + "msかかった");
        if(best_ans.field.get_score() < best_score) { // u r great!
            best_score = best_ans.field.get_score();
            answer_send(best_ans.field);
            print_text("sent, score = " + std::to_string(best_score));
        } else { // too bad...
            print_text("not sent, score = " + std::to_string(best_ans.field.get_score()));
        }
    }
}
