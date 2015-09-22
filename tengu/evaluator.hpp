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
    evaluator(double w_complexity_, double w_contact_move_, double w_nextbranches_, double w_rem_stone_zk_, double w_rem_space_) :
        w_complexity(w_complexity_),
        w_contact_move(w_contact_move_),
        w_nextbranches(w_nextbranches_),
        w_rem_stone_zk(w_rem_stone_zk_),
        w_rem_space(w_rem_space_)
    {
    }

    // おいた時
    // 引数: 置く前のフィールド、行おうとしている操作
    double inline move_goodness(field_type field, process_type process)
    {
        return w_complexity * field.evaluate_normalized_complexity();
    }

    // パスするとき
    // 引数: 操作前のフィールド、行おうとしている操作、残りの石のzk数(暫定)
    // (残りの石のzk数: 行おうとしている操作の後の残りの石のzkの合計)
    bool inline should_pass(field_type field, process_type process, size_t rem_stone_zk)
    {
        return rem_stone_zk > field.empty_zk();
    }

private:
    // 重み
    double const w_complexity; // 場の複雑さ(正規化)
    double const w_contact_move; // 接辺の数(正規化)
    double const w_nextbranches; // 次の石がおける数
    double const w_rem_stone_zk; // 残りの石のzk数
    double const w_rem_space; // 残りの空白

    // 評価関数
    double normalized_contact(field_type const& field, process_type const& process); // 接辺の数(正規化)
    int nextbranches(field_type const& field, stone_type const& stone); // 次の石がおける数
};

#endif // EVALUATOR_HPP

