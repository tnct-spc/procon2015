#include "algorithm_type.hpp"

/*
algorithm_type::algorithm_type(problem_type const& _problem): problem(_problem)
{
}
*/
algorithm_type::~algorithm_type(){

}
void algorithm_type::print_text(std::string str){
    emit send_text("<" + algorithm_name + ">" + str);
}
