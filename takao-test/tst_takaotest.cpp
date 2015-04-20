#include <QString>
#include <QtTest>

class TakaoTest : public QObject
{
        Q_OBJECT

    public:
        TakaoTest();

    private Q_SLOTS:
        void testCase1();
};

TakaoTest::TakaoTest()
{
}

void TakaoTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TakaoTest)

#include "tst_takaotest.moc"
