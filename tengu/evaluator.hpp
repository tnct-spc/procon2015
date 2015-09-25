#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP
#include <QObject>
#include <field_type.hpp>

/*
 * 評価関数クラス
 * コンストラクタでパラメータ(重み)を受け取る
 * コメントにある仕様は暫定
 */

class evaluator : public QObject
{
    Q_OBJECT
public:
    evaluator(double w_complexity_, double w_contact_move_, double w_nextbranches_, double t_contact_pass_) :
        w_complexity(w_complexity_),
        w_contact_move(w_contact_move_),
        w_nextbranches(w_nextbranches_),
        t_contact_pass(t_contact_pass_)
    {
    }

    // おいた時
    // 引数: 置く前のフィールド、行おうとしている操作、そのさらに次の石
    // fieldを内部で変更するけど元に戻すからヘーキヘーキ
    double inline const move_goodness(field_type &field, process_type const& process, stone_type &next_stone) const
    {
        double ret;
        ret += w_contact_move * normalized_contact(field, process);
        field.put_stone(process.stone, process.position.y, process.position.x);
        ret += w_nextbranches * nextbranches(field, next_stone);
        ret += w_complexity * field.evaluate_normalized_complexity();
        field.remove_large_most_number_and_just_before_stone(); // ?????
        return ret;
    }

    // パスするとき
    // 引数: 操作前のフィールド、行おうとしている操作、残りの石のzk数(暫定)
    // (残りの石のzk数: 行おうとしている操作の後の残りの石のzkの合計)
    bool inline const should_pass(field_type field, process_type process, size_t rem_stone_zk) const
    {
        if(rem_stone_zk < field.empty_zk())
            return false;
        return normalized_contact(field, process) < t_contact_pass;
    }

private:
    // 重み
    double const w_complexity; // 場の複雑さ(正規化)
    double const w_contact_move; // 接辺の数(正規化)
    double const w_nextbranches; // 次の石がおける数
    // 閾値
    double const t_contact_pass; // 接辺の数(正規化)
    // 評価関数
    double normalized_contact(field_type const& field, process_type const& process) const; // 接辺の数(正規化)
    int nextbranches(field_type const& field, stone_type &stone) const; // 次の石がおける数、stoneを内部で変更するけど元に戻すからヘーキヘーキ
};

#endif // EVALUATOR_HPP

