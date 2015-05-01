#ifndef ALGORITHM_TYPE
#define ALGORITHM_TYPE

class problem_type;
class answer_type;

// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class SHARED_EXPORT algorithm_type
{
    public:
        algorithm_type(problem_type const& _problem);
        virtual ~algorithm_type() = 0;

        virtual answer_type get() = 0;
        virtual bool is_continuable() = 0;

    private:
        problem_type problem;
};

algorithm_type::algorithm_type(problem_type const& _problem): problem(_problem)
{
}

#endif // ALGORITHM_TYPE
