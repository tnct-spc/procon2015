#include <QCoreApplication>
#include "problem_maker.hpp"
#include "raw_stone.hpp"
#include "raw_field.hpp"
#include "file_export.hpp"
#include <array>
#include <random>
#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char **argv)
{
    boost::program_options::options_description opt("オプション");
    opt.add_options()
        ("help,h", "ヘルプを表示")
        ("with-answer,w", "答え付きの問題を生成")
        ("ansfile,a", boost::program_options::value<std::string>()->default_value("ans.txt"), "解答ファイル名")
        ("num-of-problem,n", boost::program_options::value<int>()->default_value(1), "問題数")
        ("cut-row,r", boost::program_options::value<int>()->default_value(-1), "削る横列")
        ("cut-column,c", boost::program_options::value<int>()->default_value(-1), "削る縦列")
        ("obstacle,o", boost::program_options::value<int>()->default_value(-1), "障害物");


    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
    }
    catch(const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << opt << std::endl;
    }

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_row(0, FIELD_SIZE / 2);
    std::uniform_int_distribution<> dist_col(0, FIELD_SIZE / 2);

    int const nop = vm["num-of-problem"].as<int>();
    //std::cout << "num-of-problem = " << nop << std::endl;

    int const row = vm["cut-row"].as<int>() < 1 ? dist_row(engine) : vm["cut-row"].as<int>();
    std::cout << "cut-row = " << row << std::endl;

    int const column = vm["cut-column"].as<int>() < 1 ? dist_col(engine) : vm["cut-column"].as<int>();
    std::cout << "cut-column = " << column << std::endl;

    //ルール上はこうだけどあまりに多いので
    //std::uniform_int_distribution<> dist_obs(0, 1023-row-column);
    std::uniform_int_distribution<> dist_obs(0, 50);
    int const obstacle = vm["obstacle"].as<int>() < 1 ? dist_obs(engine) : vm["obstacle"].as<int>();
    std::cout << "obstacle = " << obstacle << std::endl;

    for(int i = 1; i <= nop; ++i)
    {
        raw_field rf(obstacle,column,row);
        raw_stone rs(1,rf.get_empty_zk());
        file_export fe(i,rf.field,rs.data);
    }
    std::cout << "Completion" << std::endl;

    return 0;
}
