#include "problem_maker.hpp"
#include <field_type.hpp>
#include <stone_type.hpp>
#include <problem_type.hpp>
#include <algorithm_type.hpp>
#include <tengu.hpp>
#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <iostream>
#include <fstream>

void problem_maker::run()
{
    if (error)
        QCoreApplication::exit(-1);

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    for (int i = 1; i <= problems; ++i) {
        field_type field;
        field.set_random(obstacles, cols, rows);
        std::vector<stone_type> stones;
        int const minimum_zk = field.empty_zk();
        std::cout << "empty blocks: " << minimum_zk << std::endl;
        for(int total_zk = 0; total_zk < minimum_zk; ) {
            std::uniform_int_distribution<int> dist_zk(min_zk, max_zk);
            int const zk = dist_zk(engine);
            stone_type stone(zk);
            total_zk += stone.get_area();
            stones.push_back(stone);
        }
        field.set_provided_stones(stones.size());
        problem_type problem(field, stones);
        std::cerr << "stones in problem: " << problem.stones.size() << std::endl;
        //std::cerr << "problem field:" << std::endl << field.str();
        if (create_answer) {
            std::cerr << "with-answer: not implemented" << std::endl;
            QCoreApplication::exit(-1);
            /*
            qRegisterMetaType<field_type>();
            qRegisterMetaType<std::string>();

            // 1st pass
            std::cout << "1st pass" << std::endl;

            test_algo *algo = new test_algo(problem);
            algo->setParent(this);
            connect(algo, &test_algo::answer_ready, [this](field_type f){ans_field = f; std::cout << "answer ready!" << std::endl;});
            connect(algo, &test_algo::send_text, [](std::string s){std::cout << s << std::endl;});
            connect(algo, &test_algo::finished, algo, &test_algo::deleteLater);
            algo->start();
            algo->wait();

            //std::cerr << "result: " << std::endl << ans_field.str();
            std::cerr << "stones in answer: " << ans_field.list_of_stones().size() << std::endl;
            int const remainder = ans_field.empty_zk();
            std::cerr << "empty: " << remainder << std::endl;
            for(int i = 0; i < remainder; i++)
                stones.push_back(stone_type(1));
            field.set_provided_stones(stones.size());

            problem_type final_problem(field, stones);
            problem = final_problem;;
            std::cerr << "stones in problem: " << problem.stones.size() << std::endl;


            // 2nd pass (always succeeds with score 0)
            std::cout << "2nd pass" << std::endl;

            algo = new test_algo(problem);
            algo->setParent(this);
            connect(algo, &test_algo::answer_ready, [this](field_type f){ans_field = f; std::cout << "answer ready!" << std::endl;});
            connect(algo, &test_algo::send_text, [](std::string s){std::cout << s << std::endl;});
            connect(algo, &test_algo::finished, &test_algo::deleteLater);
            algo->start();
            algo->wait();

            //std::cerr << "result: " << std::endl << ans_field.str();
            std::cerr << "stones in answer: " << ans_field.list_of_stones().size() << std::endl;

            std::cerr << "score: " << ans_field.get_score() << std::endl;
            std::ofstream ofs(ansfilename);
            ofs << ans_field.get_answer();
            */
        }
        std::string const filename = "quest" + std::to_string(i) + ".txt";
        std::ofstream ofs(filename);
        if(ofs.fail()) {
            std::cerr << "failed to open " << filename << std::endl;
            QCoreApplication::exit(-1);
        }
        ofs << problem.str();
    }
    QCoreApplication::exit(0);
}
