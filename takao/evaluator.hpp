#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP
#include <field_type.hpp>
#include <process_type.hpp>
#include <limits>

/*
 * 評価関数クラス
 * コンストラクタでパラメータ(重み)を受け取る
 * コメントにある仕様は暫定
 */

class evaluator
{
public:
    evaluator() :
        w_complexity(0),
        w_contact_move(0),
        w_nextbranches(0),
        t_contact_pass(0)
    {
    }
    /*
     * コンストラクタ(値を直接受け取るバージョン)
     */
    evaluator(double w_complexity_, double w_contact_move_,
              double w_nextbranches_, double t_contact_pass_) :
        w_complexity(w_complexity_),
        w_contact_move(w_contact_move_),
        w_nextbranches(w_nextbranches_),
        t_contact_pass(t_contact_pass_)
    {
    }
    /*
     * コンストラクタ(値を比で受け取るバージョン)
     * 0.0 <= w_{complexity,contact_move}_rel <= 1.0 かつ
     * 0.0 <= w_complexity_rel + w_contact_move_rel <= 1.0
     */
    evaluator(double w_complexity_rel_,
              double w_contact_move_rel_,
              double t_contact_pass_) :
        w_complexity(-w_complexity_rel_),
        w_contact_move(w_contact_move_rel_),
        w_nextbranches(1.0 - w_complexity_rel_ - w_contact_move_rel_),
        t_contact_pass(t_contact_pass_)
    {
    }

    // 定数
    double static constexpr max_value = std::numeric_limits<double>::max();
    double static constexpr min_value = std::numeric_limits<double>::min();

    // おいた時
    // 引数: 置く前のフィールド、行おうとしている操作、そのさらに次の石
    // fieldを内部で変更するけど元に戻すからヘーキヘーキ
    // bit version
    // 最後の石も渡してOK
    double inline move_goodness(field_type &field,
                                std::vector<stone_type> &stones,
                                bit_process_type const process) const
    {
        double ret = 0.0;
        ret += w_contact_move * normalized_contact(field, stones, process);
        field.put_stone_basic(stones[process.nth - 1], process.position.y, process.position.x);
        ret += w_complexity * field.evaluate_normalized_complexity();
        if(process.nth < stones.size()) // 最後の石以外
            ret += w_nextbranches * nextbranches(field, stones[process.nth]);

        field.remove_stone_basic(stones[process.nth - 1]);
        return ret;
    }

    // パスするとき
    // 引数: 操作前のフィールド、行おうとしている操作、残りの石のzk数(暫定)
    // (残りの石のzk数: 行おうとしている操作の後の残りの石のzkの合計)
    // bit version
    bool inline should_pass(field_type const& field,
                            std::vector<stone_type> &stones,
                            bit_process_type const process,
                            std::size_t rem_stone_zk) const
    {
        if(rem_stone_zk < field.empty_zk())
            return false;
        return normalized_contact(field, stones, process);
    }

    // 先読みの深さ
    // bit version
    int inline search_depth(field_type const& field,
                            std::vector<stone_type> &stones,
                            bit_process_type const process)
    {
        double x = (1.0 - normalized_contact(field, stones, process));
        return max_search_depth * x * x;
    }
    // 評価関数
    double normalized_contact(field_type const& field, std::vector<stone_type> &stones, bit_process_type process) const; // bit version


private:
    // 重み
    double const w_complexity; // 場の複雑さ(正規化)
    double const w_contact_move; // 接辺の数(正規化)
    double const w_nextbranches; // 次の石がおける数
    // 閾値
    double const t_contact_pass; // 接辺の数(正規化)
    // 上限
    int const max_search_depth = 15; // 先読みの深さ上限
    // 評価関数
    int nextbranches(field_type const& field, stone_type &stone) const; // 次の石がおける数、stoneを内部で変更するけど元に戻すからヘーキヘーキ
    int footprint(field_type const& field, std::vector<stone_type> const& stones, bit_process_type process) const;
};

#endif // EVALUATOR_HPP
