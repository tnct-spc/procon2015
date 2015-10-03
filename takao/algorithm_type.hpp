#ifndef ALGORITHM_TYPE
#define ALGORITHM_TYPE
#include "problem_type.hpp"
#include "field_type.hpp"
#include <QThread>
#include <mutex>
#include "evaluator.hpp"
// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class algorithm_type : public QThread
{
    Q_OBJECT
    public:
        //algorithm_type(problem_type const& _problem);
        //virtual ~algorithm_type() = 0;
        virtual void run() = 0;
        void print_text(std::string str);
        //Qtのemitが遅いのでalgorithm自身が送信するかどうか判断するためのClass変数
        static int _best_score;
    protected:
        problem_type problem;
        std::string algorithm_name;
        static std::mutex ans_emit_mtx;
        evaluator _eval;
        //static int _best_score;
        void answer_send(field_type ans);
        int get_rem_stone_zk(int stone_num);
    signals:
        void answer_ready(field_type ans);
        //void print_text(std::string str);
        void send_text(std::string str);
};
#endif // ALGORITHM_TYPE
