#include "problem_maker.hpp"
#include <boost/program_options.hpp>
#include <field_type.hpp>
#include <stone_type.hpp>
#include <problem_type.hpp>
#include <array>
#include <random>
#include <iostream>
#include <fstream>
#include <limits>
#include <QCoreApplication>

problem_maker::problem_maker(int argc, char **argv)
{
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    // parse options
    boost::program_options::options_description opt("オプション");
     opt.add_options()
         ("help,h", "ヘルプを表示")
         ("with-answer,w", "答え付きの問題を生成")
         ("ansfile,a", boost::program_options::value<std::string>()->default_value("answer.txt"), "解答ファイル名")
         ("problems,p", boost::program_options::value<int>()->default_value(1), "問題数")
         ("rows,r", boost::program_options::value<int>(), "行数")
         ("cols,c", boost::program_options::value<int>(), "列数")
         ("obstacles,o", boost::program_options::value<int>(), "障害物")
         ("min-zk,z", boost::program_options::value<int>()->default_value(1), "石の面積の下限")
         ("max-zk,Z", boost::program_options::value<int>()->default_value(16), "石の面積の上限");
     boost::program_options::variables_map vm;
     try {
         boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
     } catch(const boost::program_options::error_with_option_name& e) {
         std::cout << e.what() << std::endl;
         QCoreApplication::exit(1);
     }
     boost::program_options::notify(vm);

     // usage
     if (vm.count("help")) {
         std::cout << opt << std::endl;
         error = true;
         return;
     }

     // set problems
     problems = vm["problems"].as<int>();
     std::cout << "problems: " << problems << std::endl;

     // set rows
     if(vm.count("rows")) {
         rows = vm["rows"].as<int>();
     } else {
         std::uniform_int_distribution<int> dist(1, FIELD_SIZE);
         rows = dist(engine);
     }
     std::cout << "rows: " << rows << std::endl;

     //set cols
     if(vm.count("cols")) {
         cols = vm["col"].as<int>();
     } else {
         std::uniform_int_distribution<int> dist(1, FIELD_SIZE);
         cols = dist(engine);
     }
     std::cout << "cols: " << cols << std::endl;

     // set obstacles
     if(vm.count("obstacles")) {
         obstacles = vm["obstacles"].as<int>();
     } else {
         // 1023?
         std::uniform_int_distribution<int> dist(0, std::min(50, rows * cols));
         obstacles = dist(engine);
     }
     std::cout << "obstacles: " << obstacles << std::endl;

     // set min_zk, max_zk
     min_zk = vm["min-zk"].as<int>();
     max_zk = vm["max-zk"].as<int>();

     // set create_answer
     create_answer = vm.count("with-answer");
}

void problem_maker::run()
{
    if(error)
        QCoreApplication::exit(-1);

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    for(int i = 1; i <= problems; ++i) {
        field_type field;
        field.set_random(obstacles, cols, rows);
        std::vector<stone_type> stones;
        int const minimum_zk = field.empty_zk();
        std::cout << "empty blocks: " << minimum_zk << std::endl;
        for(int total_zk = 0; total_zk < minimum_zk; ) {
            std::uniform_int_distribution<int> dist_zk(min_zk, max_zk);
            int const stone_zk = dist_zk(engine);
            stone_type stone(stone_zk);
            total_zk += stone.get_area();
            stones.push_back(stone);
        }
        problem_type problem(field, stones);
        if(create_answer) {
            std::cerr << "not implemented yet" << std::endl;
        } else {
            std::string const filename = "quest" + std::to_string(i) + ".txt";
            std::ofstream ofs(filename);
            if(ofs.fail()) {
                std::cerr << "failed to open " << filename << std::endl;
                QCoreApplication::exit(-1);
            }
            ofs << problem.str();
        }
    }
    QCoreApplication::exit(0);
}
