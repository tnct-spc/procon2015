#include <QString>
#include <QtTest>
#define private public
#include "data_type.hpp"
#undef private

class field_type_test : public QObject
{
        Q_OBJECT

    public:
        field_type_test();

    private Q_SLOTS:
        void testCase1();
};

field_type_test::field_type_test()
{
}

void field_type_test::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(field_type_test)

#include "field_type_test.moc"
