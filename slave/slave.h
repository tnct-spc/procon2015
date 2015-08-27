#ifndef SLAVE_H
#define SLAVE_H

#include <QWidget>
#include <QString>
#include <QUrl>
#include <net.hpp>
#include <mutex>
#include <QSettings>
#include "takao.hpp"
#include "muen_zuka.hpp"
namespace Ui {

class Slave;
}

class Slave : public QWidget
{
    Q_OBJECT

public:
    explicit Slave(QWidget *parent = 0);
    ~Slave();

private:
    Ui::Slave *ui;
    net *network;
    algorithm_manager *algo_manager;
    std::mutex net_mtx;
    problem_type _problem;
    problem_type _answer;

    QString get_posturl();
    QString get_geturl();
    QSettings* settings;
private slots:
    void clicked_run_button();
    void answer_send(field_type answer);
    void text_box_clear();
    void answer_save_to_file();
    void problem_load_from_file();
    void print_algorithm_message(std::string str);
    void post_button_1_pushed();
    void post_button_2_pushed();
    void get_button_1_pushed();
    void get_button_2_pushed();
    void get_button_3_pushed();
    void post_line_edit_1_changed();
    void post_line_edit_2_changed();
    void get_line_edit_1_changed();
    void get_line_edit_2_changed();
    void get_line_edit_3_changed();
};

#endif // SLAVE_H
