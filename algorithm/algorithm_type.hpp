#ifndef ALGORITHM_TYPE
#define ALGORITHM_TYPE

#include "problem_type.hpp"

// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class algorithm_type
{
    public:
        algorithm_type(problem_type const& _problem);
        virtual ~algorithm_type() = 0;

        virtual field_type get() = 0;
        virtual bool is_continuable() = 0;

    private:
        problem_type problem;
};
#endif // ALGORITHM_TYPE
