#include <field_type.hpp>
#include <stone_type.hpp>
#include <problem_type.hpp>
#include <array>
#include <random>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <limits>

int main(int argc, char **argv)
{
    boost::program_options::options_description opt("オプション");
    opt.add_options()
        ("help,h", "ヘルプを表示")
        ("with-answer,w", "答え付きの問題を生成")
        ("ansfile,a", boost::program_options::value<std::string>()->default_value("answer.txt"), "解答ファイル名")
        ("num-of-problem,n", boost::program_options::value<int>()->default_value(1), "問題数")
        ("row,r", boost::program_options::value<int>(), "横列")
        ("column,c", boost::program_options::value<int>(), "縦列")
        ("obstacle,o", boost::program_options::value<int>(), "障害物");


    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
    } catch(const boost::program_options::error_with_option_name& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << opt << std::endl;
        return 0;
    }

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist_row(1, FIELD_SIZE);
    std::uniform_int_distribution<> dist_col(1, FIELD_SIZE);

    int const nop = vm["num-of-problem"].as<int>();
    std::cout << "problems: " << nop << std::endl;

    int const row = vm.count("row") ?
                vm["row"].as<int>() : dist_row(engine);
    std::cout << "row: " << row << std::endl;

    int const column = vm.count("column") ?
                vm["column"].as<int>() : dist_col(engine);
    std::cout << "column: " << column << std::endl;

    //ルール上はこうだけどあまりに多いので
    //std::uniform_int_distribution<> dist_obs(0, 1023-row-column);
    std::uniform_int_distribution<> dist_obs(0, std::min(50, row * column));
    int const obstacle = vm.count("obstacle") ?
                vm["obstacle"].as<int>() : dist_obs(engine);
    std::cout << "obstacles: " << obstacle << std::endl;

    int const create_answer = vm.count("with-answer");

    for(int i = 1; i <= nop; ++i) {
        if(create_answer) {
            std::cerr << "not implemented yet" << std::endl;
            field_type f;
            f.set_random(obstacle, column, row);
        } else {
            field_type field;
            field.set_random(obstacle, column, row);
            std::vector<stone_type> stones;
            int const minimum_zk = field.empty_zk();
            std::cout << "empty blocks: " << minimum_zk << std::endl;
            for(int total_zk = 0; total_zk < minimum_zk; ) {
                std::uniform_int_distribution<int> dist_zk(1, 16);
                int const stone_zk = dist_zk(engine);
                stone_type stone(stone_zk);
                total_zk += stone.get_area();
                stones.push_back(stone);
            }
            problem_type problem(field, stones);
            std::ofstream ofs("quest" + std::to_string(i) + ".txt");
            if(ofs.fail()) {
                std::cerr << "failed to open file" << std::endl;
                return -1;
            }
            ofs << problem.str();
        }
    }
    return 0;
}
