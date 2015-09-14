#include "algorithm_evaluater.hpp"
algorithm_evaluater::algorithm_evaluater(QObject *parent) :
    QObject(parent)
{
}
std::vector<problem_type> algorithm_evaluater::load_problem_fires(){
    std::vector<problem_type> problem_vector;
    QDir q_dir("../problems");
    if(!q_dir.exists()){
        q_dir.mkpath("../problems");
        std::cout << "ディレクトリがねぇよ" << std::endl;
        return problem_vector;
    }
    filelist = q_dir.entryList();
    QStringList::const_iterator f_itr;
    //qDebug() << filelist;
    for (f_itr = filelist.constBegin(); f_itr != filelist.constEnd(); ++f_itr){
        QFile file(QString("../problems/").append(*f_itr));
        //qDebug() << file.fileName();
        if(!file.open(QIODevice::ReadOnly))continue;
        qDebug() << file.fileName();
        QTextStream in(&file);
        problem_vector.push_back(problem_type(in.readAll().toStdString()));
     }
     return problem_vector;
}
void algorithm_evaluater::save_answer(field_type answer){
    QStringList::iterator f_itr;
    for (f_itr = filelist.begin(); f_itr != filelist.end(); ++f_itr){
        QFile file(QString().append(*f_itr).append("answer.txt"));
        if(!file.open(QIODevice::WriteOnly))return;
        QTextStream out(&file);
        out << answer.get_answer().c_str();
    }
    return;
}
field_type algorithm_evaluater::evaluate(problem_type problem){
    _algorithm = new simple_algorithm(problem);
    return problem.field;
}
void algorithm_evaluater::save_record(){

}
