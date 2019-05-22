#include <QtTest>
#include <QSerialPortInfo>

// add necessary includes here
#include "DebugHelper.hpp"

class TestDebugHelper : public QObject
{
    Q_OBJECT

public:
    TestDebugHelper();
    ~TestDebugHelper();

private slots:
    void test_caseByteArrayToString();
    void test_caseTeamToString();

};

TestDebugHelper::TestDebugHelper()
{

}

TestDebugHelper::~TestDebugHelper()
{

}

void TestDebugHelper::test_caseByteArrayToString()
{
    QByteArray b;
    b.append(0xA).append(1).append(2).append(0xac);
    QCOMPARE(DebugHelper::byteArrayToString(b), "0A0102AC");
}

void TestDebugHelper::test_caseTeamToString()
{
    QCOMPARE(DebugHelper::teamToString(Burger::MAYO), "Mayo");
    QCOMPARE(DebugHelper::teamToString(Burger::KETCHUP), "Ketchup");
    QCOMPARE(DebugHelper::teamToString(Burger::INCONNUE), "Inconnue");
    QCOMPARE(DebugHelper::teamToString(static_cast<Burger::Team>(5)), "Inconnue");
}

QTEST_APPLESS_MAIN(TestDebugHelper)

#include "tst_testdebughelper.moc"
