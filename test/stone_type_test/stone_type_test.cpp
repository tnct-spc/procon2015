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
    QTest::addColumn<std::string>("stone_text");
    QTest::addColumn<stone_type::raw_stone_type>("raw_stone");

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
}

void stone_type_test::construct_test()
{
    QFETCH(std::string, stone_text);
    QFETCH(stone_type::raw_stone_type, raw_stone);

    QCOMPARE(stone_type(stone_text, 0).get_array(), raw_stone);
}

QTEST_APPLESS_MAIN(stone_type_test)

#include "stone_type_test.moc"
