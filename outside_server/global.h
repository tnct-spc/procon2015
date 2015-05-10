#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <vector>
#include <algorithm>

struct user_data_type{
    QString userid;
    int append_stage_number;
    bool is_renewal;
    //answer
    int answer_flow[256][4];
    int answer_num;
    int answer_point;
};

//GLOBAL VAR
extern bool g_stage_state_[48][48];
extern bool g_stone_state_[256][8][8];
extern int g_stone_num_;
extern std::vector<user_data_type> g_user_data;
extern bool g_user_data_updated;

#endif // GLOBAL_H
