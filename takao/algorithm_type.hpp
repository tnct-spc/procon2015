#ifndef ALGORITHM_TYPE
#define ALGORITHM_TYPE
#include "problem_type.hpp"
#include "field_type.hpp"
#include <QThread>
// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class algorithm_type : public QThread
{
    Q_OBJECT
    public:
        //algorithm_type(problem_type const& _problem);
        virtual ~algorithm_type() = 0;
        virtual void run() = 0;
        void print_text(std::string str);
    protected:
        problem_type problem;
        std::string algorithm_name;
    signals:
        void answer_ready(field_type ans);
        //void print_text(std::string str);
        void send_text(std::string str);
};
#endif // ALGORITHM_TYPE
