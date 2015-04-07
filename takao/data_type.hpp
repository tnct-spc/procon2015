#ifndef TAKAO_HPP
#define TAKAO_HPP

#include "takao_global.hpp"

// SHARED_EXPORT って書けば外から見える

// 敷地のブロックの状態
enum struct BlockState { BLANK, STONE, OBSTACLE };

// 何らかの座標を表すクラス
class SHARED_EXPORT point_type
{
    public:
        point_type() = default;
        ~point_type() = default;

        int y;
        int x;
};

// 石
class SHARED_EXPORT stone_type
{
    public:
        stone_type() = default;
        ~stone_type() = default;
};

// 敷地に置かれた石の情報
class SHARED_EXPORT placed_stone_type
{
    public:
        placed_stone_type() = default;
        ~placed_stone_type() = default;
};

// 敷地
class SHARED_EXPORT field_type
{
    public:
        field_type() = default;
        ~field_type() = default;
};

// 問題データ
class SHARED_EXPORT problem_type
{
    public:
        problem_type() = default;
        ~problem_type() = default;
};

// 解答データの手順ひとつ分
class SHARED_EXPORT process_type
{
    public:
        process_type() = default;
        ~process_type() = default;
};

// 解答データ
class SHARED_EXPORT answer_type
{
    public:
        answer_type() = default;
        ~answer_type() = default;
};

// アルゴリズムの基底クラス
// テンプレートのほうがいい?
class SHARED_EXPORT algorithm_type
{
    public:
        algorithm_type(problem_type);
        virtual ~algorithm_type() = 0;
};

#endif // TAKAO_HPP
