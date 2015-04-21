#include <QString>
#include <QtTest>
#define private public
#include "data_type.hpp"
#undef private

class stone_type_test : public QObject
{
        Q_OBJECT

    public:
        stone_type_test();

    private Q_SLOTS:
        void test_case_1();
};

stone_type_test::stone_type_test()
{
}

void stone_type_test::test_case_1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(stone_type_test)

#include "stone_type_test.moc"
