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
        algorithm_type(problem_type const& _problem);
        virtual ~algorithm_type() = 0;

        //virtual field_type get() = 0;
        //virtual bool is_continuable() = 0;
        //void run()
    private:
        problem_type problem;
    signals:
        void answer_ready(field_type);
};
#endif // ALGORITHM_TYPE
