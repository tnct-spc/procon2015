#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <vector>
#include <algorithm>
#include <mutex>

struct answer_data_type{
    QString answer_raw_data;
    int answer_point;
    QString problem_number;
};

//GLOBAL VAR
extern QString g_token_name;
extern std::vector<answer_data_type> g_answer_data;

#endif // GLOBAL_H
