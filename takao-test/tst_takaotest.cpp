#include <QString>
#include <QtTest>

#define private public
#include "data_type.hpp"
#undef private

class TakaoTest : public QObject
{
        Q_OBJECT

    public:
        TakaoTest();

    private Q_SLOTS:
        void _split_test();
};

TakaoTest::TakaoTest()
{
}

void TakaoTest::_split_test()
{
    using namespace std::string_literals;
    auto sample = "123,,456,,789,,12,34,56,78,90,,abc,,def,,ghi,,,,,"s;
    std::vector<std::string> result = _split(sample, ",,"s);
    std::vector<std::string> expected = {"123"s,"456"s,"789"s, "12,34,56,78,90"s, "abc"s, "def"s, "ghi"s, ""s, ","s};
    QCOMPARE(expected, result);
}

QTEST_APPLESS_MAIN(TakaoTest)

#include "tst_takaotest.moc"
