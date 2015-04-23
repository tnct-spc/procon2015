#include <QString>
#include <QtTest>
#define private public
#include "takao.hpp"
#undef private
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(stone_type)
Q_DECLARE_METATYPE(stone_type::raw_stone_type)

class stone_type_test : public QObject
{
        Q_OBJECT

    public:
        stone_type_test();

    private Q_SLOTS:
        void construct_test_data();
        void construct_test();

        void rotate_test_data();
        void rotate_test();

        void flip_test_data();
        void flip_test();

        void get_area_test_data();
        void get_area_test();
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

    QCOMPARE(stone_type(stone_text, 0).get_raw_data(), raw_stone);
}

void stone_type_test::rotate_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<stone_type>("rotated90");
    QTest::addColumn<stone_type>("rotated180");
    QTest::addColumn<stone_type>("rotated270");

    QTest::newRow("1")
        << stone_type(
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01110000\r\n"
           "00000000"s, 0)
        << stone_type(
           "00000000\r\n"
           "01111111\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000"s, 0)
        << stone_type(
           "00000000\r\n"
           "00001110\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010"s, 0)
        << stone_type(
           "00000000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000000\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "11111110\r\n"
           "00000000"s, 0);
}

void stone_type_test::rotate_test()
{
    QFETCH(stone_type, stone);
    QFETCH(stone_type, rotated90);
    QFETCH(stone_type, rotated180);
    QFETCH(stone_type, rotated270);

    QCOMPARE(stone.rotate(90), rotated90);
    QCOMPARE(stone.rotate(180), rotated180);
    QCOMPARE(stone.rotate(270), rotated270);
    QCOMPARE(stone.rotate(360), stone);
    QCOMPARE(stone.rotate(450), rotated90);
    QCOMPARE(stone.rotate(-90), rotated270);
    QCOMPARE(stone.rotate(-180), rotated180);
    QCOMPARE(stone.rotate(-270), rotated90);
    QCOMPARE(stone.rotate(-360), stone);
    QCOMPARE(stone.rotate(-450), rotated270);
}

void stone_type_test::flip_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<stone_type>("flipped");

    QTest::newRow("1")
        << stone_type(
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01110000\r\n"
           "00000000"s, 0)
        << stone_type(
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00000010\r\n"
           "00001110\r\n"
           "00000000"s, 0);
}

void stone_type_test::flip_test()
{
    QFETCH(stone_type, stone);
    QFETCH(stone_type, flipped);

    QCOMPARE(stone.flip(), flipped);
}

void stone_type_test::get_area_test_data()
{
    using namespace std::string_literals;
    QTest::addColumn<stone_type>("stone");
    QTest::addColumn<std::size_t>("size");

    QTest::newRow("1")
        << stone_type(
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01000000\r\n"
           "01110000\r\n"
           "00000000"s, 0)
        << 9;
}

void stone_type_test::get_area_test()
{
    QFETCH(stone_type, stone);
    QFETCH(std::size_t, size);

    QCOMPARE(stone.get_area(), size);
}

QTEST_APPLESS_MAIN(stone_type_test)

#include "stone_type_test.moc"
