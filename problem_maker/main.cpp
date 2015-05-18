#include <QCoreApplication>
#include "problem_maker.hpp"
#include "raw_stone.hpp"
#include <array>
#include <random>
#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char **argv)
{
    boost::program_options::options_description opt("オプション");
    opt.add_options()
        ("help,h", "ヘルプを表示")
        ("num-of-problem,n", boost::program_options::value<int>()->default_value(10), "問題数")
        ("cut-row,r", boost::program_options::value<int>()->default_value(-1), "削る横列")
        ("cut-column,c", boost::program_options::value<int>()->default_value(-1), "削る縦列");

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

    const int nop = vm["num-of-problem"].as<int>();
    std::cout << "num-of-problem = " << nop << std::endl;

    const int row = vm["cut-row"].as<int>();
    std::cout << "cut-row = " << row << std::endl;

    const int column = vm["cut-column"].as<int>();
    std::cout << "cut-column = " << column << std::endl;

    std::cout << "Hello, world" << std::endl;

    //QCoreApplication a(argc, argv);

    //raw_stone stone;
    //stone.create();

    //return a.exec();

    return 0;
}
