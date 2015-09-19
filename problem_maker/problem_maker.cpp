#include "problem_maker.hpp"
#include <boost/program_options.hpp>
#include <QCoreApplication>
#include <random>
#include <iostream>
#include <limits>

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
     if (vm.count("rows")) {
         rows = vm["rows"].as<int>();
     } else {
         std::uniform_int_distribution<int> dist(1, FIELD_SIZE);
         rows = dist(engine);
     }
     std::cout << "rows: " << rows << std::endl;

     //set cols
     if (vm.count("cols")) {
         cols = vm["cols"].as<int>();
     } else {
         std::uniform_int_distribution<int> dist(1, FIELD_SIZE);
         cols = dist(engine);
     }
     std::cout << "cols: " << cols << std::endl;

     // set obstacles
     if (vm.count("obstacles")) {
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

     // set ansfilename
     ansfilename = vm["ansfile"].as<std::string>();
}
