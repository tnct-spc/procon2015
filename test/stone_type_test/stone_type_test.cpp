#include <QString>
#include <QtTest>
#define private public
#include "data_type.hpp"
#undef private
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(stone_type::raw_stone_type)

class stone_type_test : public QObject
{
        Q_OBJECT

    public:
        stone_type_test();

    private Q_SLOTS:
        void construct_test_data();
        void construct_test();
};

stone_type_test::stone_type_test()
{
}

void stone_type_test::construct_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<std::string>("raw_stone");
    QTest::addColumn<stone_type::raw_stone_type>("result");

    QTest::newRow("1")
        << "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01110000\r\n"
           "00000000"s
        << stone_type::raw_stone_type{{
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 1, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}}}};
    QTest::newRow("2")
        << "00000000\r\n"
           "01100000\r\n"
           "01100000\r\n"
           "01100000\r\n"
           "01100000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000"s
        << stone_type::raw_stone_type{{
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}}}};
    QTest::newRow("3")
        << "00000000\r\n"
           "00010000\r\n"
           "00010000\r\n"
           "01111000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000"s
        << stone_type::raw_stone_type{{
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 1, 0, 0, 0, 0}},
           {{0, 0, 0, 1, 0, 0, 0, 0}},
           {{0, 1, 1, 1, 1, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}},
           {{0, 0, 0, 0, 0, 0, 0, 0}}}};
    QTest::newRow("4")
        << "10000000\r\n"
           "11000000\r\n"
           "01100000\r\n"
           "00110000\r\n"
           "00011000\r\n"
           "00001100\r\n"
           "00000110\r\n"
           "00000011"s
        << stone_type::raw_stone_type{{
           {{1, 0, 0, 0, 0, 0, 0, 0}},
           {{1, 1, 0, 0, 0, 0, 0, 0}},
           {{0, 1, 1, 0, 0, 0, 0, 0}},
           {{0, 0, 1, 1, 0, 0, 0, 0}},
           {{0, 0, 0, 1, 1, 0, 0, 0}},
           {{0, 0, 0, 0, 1, 1, 0, 0}},
           {{0, 0, 0, 0, 0, 1, 1, 0}},
           {{0, 0, 0, 0, 0, 0, 1, 1}}}};
}

void stone_type_test::construct_test()
{
    QFETCH(std::string, raw_stone);
    QFETCH(stone_type::raw_stone_type, result);

    QCOMPARE(stone_type(raw_stone, 0).get_array(), result);
}

QTEST_APPLESS_MAIN(stone_type_test)

#include "stone_type_test.moc"
