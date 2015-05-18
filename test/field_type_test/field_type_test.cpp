#include <QString>
#include <QtTest>
#define private public
#include "takao.hpp"
#undef private
#include <iostream>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(field_type)
Q_DECLARE_METATYPE(field_type::raw_field_type)
Q_DECLARE_METATYPE(stone_type)
Q_DECLARE_METATYPE(placed_stone_type)

using namespace std::string_literals;

class field_type_test : public QObject
{
        Q_OBJECT

    public:
        field_type_test();

    private Q_SLOTS:
        void construct_test_data();
        void construct_test();

        void get_score_test_data();
        void get_score_test();

        void put_stone_test_data();
        void put_stone_test();

        void remove_stone_test_data();
        void remove_stone_test();

        void is_puttable_test_data();
        void is_puttable_test();

        void is_removable_test_data();
        void is_removable_test();

        void is_placed_test_data();
        void is_placed_test();

        void get_answer_test();

        void get_stone_test_data();
        void get_stone_test();

    private:
        std::string const default_field_text =
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "01000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000100000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000000000001111111111111111\r\n"
            "00000000010000001111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111\r\n"
            "11111111111111111111111111111111"s;

        field_type const default_field = field_type(default_field_text);

        field_type::raw_field_type const default_raw_field = {{
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{ 0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
            {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}};

        std::vector<stone_type> const default_stones = {
            {"01000000\r\n"
            "01000000\r\n"
            "01000000\r\n"
            "01000000\r\n"
            "01000000\r\n"
            "01000000\r\n"
            "01110000\r\n"
            "00000000"s, 1},
            {"00000000\r\n"
            "00010000\r\n"
            "00010000\r\n"
            "01111000\r\n"
            "00000000\r\n"
            "00000000\r\n"
            "00000000\r\n"
            "00000000"s, 2},
            {"00000000\r\n"
            "01100000\r\n"
            "01100000\r\n"
            "01100000\r\n"
            "01100000\r\n"
            "00000000\r\n"
            "00000000\r\n"
            "00000000"s, 3},
            {"10000000\r\n"
             "11000000\r\n"
             "01100000\r\n"
             "00110000\r\n"
             "00011000\r\n"
             "00001100\r\n"
             "00000110\r\n"
             "00000011"s, 4}
        };
};

field_type_test::field_type_test()
{
}

void field_type_test::construct_test_data()
{
    QTest::addColumn<std::string>("field_text");
    QTest::addColumn<field_type::raw_field_type>("raw_field");

    QTest::newRow("1")
        << default_field_text
        << default_raw_field;
}

void field_type_test::construct_test()
{
    QFETCH(std::string, field_text);
    QFETCH(field_type::raw_field_type, raw_field);

    QCOMPARE(field_type(field_text).get_raw_data(), raw_field);
}

void field_type_test::get_score_test_data()
{
    QTest::addColumn<field_type>("field");
    QTest::addColumn<std::size_t>("score");

    QTest::newRow("1")
        << default_field
        << 189ul;
}

void field_type_test::get_score_test()
{
    QFETCH(field_type, field);
    QFETCH(std::size_t, score);

    QCOMPARE(field.get_score(), score);
}

void field_type_test::put_stone_test_data()
{
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<field_type>("field");
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("x");
    QTest::addColumn<field_type::raw_field_type>("result");

    QTest::newRow("1")
        << stone_type(
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01110000\r\n"
           "00000000"s, 1)
        << field_type(default_field_text)
        << 0
        << -1
        << field_type::raw_field_type{{
           {{ 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{ 0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
           {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}}};
}

void field_type_test::put_stone_test()
{
    QFETCH(stone_type, stone);
    QFETCH(field_type, field);
    QFETCH(int, y);
    QFETCH(int, x);
    QFETCH(field_type::raw_field_type, result);

    QCOMPARE(field.put_stone(stone, y, x).get_raw_data(), result);
}

void field_type_test::remove_stone_test_data()
{
    QTest::addColumn<field_type>("field");
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<field_type>("result");

    auto field = default_field;

    QTest::newRow("1")
        << field.put_stone(default_stones[0], 0, -1)
        << default_stones[0]
        << default_field;
}

void field_type_test::remove_stone_test()
{
    QFETCH(field_type, field);
    QFETCH(stone_type, stone);
    QFETCH(field_type, result);

    QCOMPARE(field.remove_stone(stone).get_raw_data(), result.get_raw_data());
}

void field_type_test::is_puttable_test_data()
{
    QTest::addColumn<field_type>("field");
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("x");
    QTest::addColumn<bool>("puttable");

    QTest::newRow("puttable")
        << default_field
        << default_stones[0]
        << 0
        << -1
        << true;

    QTest::newRow("not_puttable")
        << default_field
        << default_stones[0]
        << 0
        << 0
        << false;
}

void field_type_test::is_puttable_test()
{
    QFETCH(field_type, field);
    QFETCH(stone_type, stone);
    QFETCH(int, y);
    QFETCH(int, x);
    QFETCH(bool, puttable);

    QCOMPARE(field.is_puttable(stone, y, x), puttable);
}

void field_type_test::is_removable_test_data()
{
    QTest::addColumn<field_type>("field");
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<bool>("result");

    {
        auto field = default_field;
        QTest::newRow("removable")
            << field.put_stone(default_stones[0], 0, -1)
            << default_stones[0]
            << true;
    }

    QTest::newRow("not_removable_1")
        << default_field
        << default_stones[0]
        << false;

    {
        auto field = default_field;
        QTest::newRow("not_removable_2")
            << field.put_stone(default_stones[0], 0, -1).put_stone(default_stones[1], 4, 0)
            << default_stones[0]
            << false;
    }
}

void field_type_test::is_removable_test()
{
    QFETCH(field_type, field);
    QFETCH(stone_type, stone);
    QFETCH(bool, result);

    QCOMPARE(field.is_removable(stone), result);
}

void field_type_test::is_placed_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<field_type>("field");
    QTest::addColumn<bool>("result");

    auto field = default_field;
    QTest::newRow("not_put")
        << default_stones[0]
        << field
        << false;
    QTest::newRow("put")
        << default_stones[0]
        << field.put_stone(default_stones[0], 0, -1)
        << true;
}

void field_type_test::is_placed_test()
{
    QFETCH(stone_type, stone);
    QFETCH(field_type, field);
    QFETCH(bool, result);

    QCOMPARE(field.is_placed(stone),result);
}

void field_type_test::get_answer_test()
{
    auto stones = default_stones;
    auto field = default_field;

    field.put_stone(stones[0], 2, 3);
    field.put_stone(stones[1].flip().rotate(90), -3, -1);
    field.put_stone(stones[3].rotate(270), 0, 6);

    auto expected = "3 2 H 0\r\n"
                    "-1 -3 T 90\r\n"
                    "\r\n"
                    "6 0 H 270\r\n"s;

    auto result = field.get_answer();
    //std::cerr << result;
    QCOMPARE(result, expected);
}

void field_type_test::get_stone_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("x");
    QTest::addColumn<field_type>("field");
    QTest::addColumn<placed_stone_type>("result");

    auto field = default_field;
    placed_stone_type result (default_stones[0],point_type{0,-1},point_type{0,1});

    QTest::newRow("case1")
            << 0
            << 0
            << field.put_stone(default_stones[0], 0, -1)
            << result;

}

void field_type_test::get_stone_test()
{
    QFETCH(int, y);
    QFETCH(int, x);
    QFETCH(field_type, field);
    QFETCH(placed_stone_type, result);

    QCOMPARE(field.get_stone(y,x),result);
}

QTEST_APPLESS_MAIN(field_type_test)

#include "field_type_test.moc"
