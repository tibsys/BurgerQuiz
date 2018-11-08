#include <QtTest>
#include <QRandomGenerator>

// add necessary includes here
#include "BurgerController.h"
#include "MessageFactory.hpp"

class BurgerControllerWrapper : public BurgerController
{
public:
    //Changement de la visibilité de cette fonction pour les besoins du test
    void onDataReceived(QByteArray data) {
        BurgerController::onDataReceived(data);
    }
};

class TestBurgerController: public QObject
{
    Q_OBJECT

public:
    TestBurgerController();
    ~TestBurgerController();

private slots:
    void test_caseIncomingDataNotValid();
    void test_caseIncomingDataNotFragmented();
    void test_caseIncomingDataFragmented();
    void test_caseIncomingDataValidButNoCommand();
    void test_caseIncomingDataFragmentedMultipleCommands();
};

TestBurgerController::TestBurgerController()
{

}

TestBurgerController::~TestBurgerController()
{

}

void TestBurgerController::test_caseIncomingDataNotValid()
{
    QByteArray d = DebugHelper::hexStringToByteArray("4C4544205B3120646174615D203030200A50696E67205B3020646174615D200A01070004");

    BurgerControllerWrapper ctrl;
    QSignalSpy spyPongReceived(&ctrl, &BurgerController::pongReceived);
    QSignalSpy spyButtonPressed(&ctrl, &BurgerController::buttonPressed);

    ctrl.onDataReceived(d);

    spyPongReceived.wait(500);
    spyButtonPressed.wait(500);

    QVERIFY(spyPongReceived.count() == 0);
    QVERIFY(spyButtonPressed.count() == 0);
}

void TestBurgerController::test_caseIncomingDataNotFragmented()
{
    QByteArray d = DebugHelper::hexStringToByteArray("4C4544205B3120646174615D203030200A50696E67205B3020646174615D200A01060004");

    BurgerControllerWrapper ctrl;
    QSignalSpy spyPongReceived(&ctrl, &BurgerController::pongReceived);
    QSignalSpy spyButtonPressed(&ctrl, &BurgerController::buttonPressed);

    ctrl.onDataReceived(d);

    spyPongReceived.wait(500);

    QVERIFY(spyPongReceived.count() == 1);
}

void TestBurgerController::test_caseIncomingDataValidButNoCommand()
{
    QByteArray d = DebugHelper::hexStringToByteArray("4C4544205B3120646174615D203032200A");

    BurgerControllerWrapper ctrl;
    QSignalSpy spyPongReceived(&ctrl, &BurgerController::pongReceived);
    QSignalSpy spyButtonPressed(&ctrl, &BurgerController::buttonPressed);

    ctrl.onDataReceived(d);

    spyPongReceived.wait(500);
    spyButtonPressed.wait(500);

    QVERIFY(spyPongReceived.count() == 0);
    QVERIFY(spyButtonPressed.count() == 0);
}

void TestBurgerController::test_caseIncomingDataFragmented()
{
    QByteArray d = DebugHelper::hexStringToByteArray("4C4544205B3120646174615D203030200A50696E67205B3020646174615D200A01060004");

    BurgerControllerWrapper ctrl;
    QSignalSpy spyPongReceived(&ctrl, &BurgerController::pongReceived);
    QSignalSpy spyButtonPressed(&ctrl, &BurgerController::buttonPressed);

    //We fragment data into packet of random size < 8 bytes
    while(!d.isEmpty()) {
        int len = QRandomGenerator::global()->generate() % 8;
        qDebug() << "Packet size : " << len;
        QByteArray subd = d.left(len);
        ctrl.onDataReceived(subd);
        d.remove(0, len);
    }

    spyPongReceived.wait(500);
    QVERIFY(spyPongReceived.count() == 1);
}

void TestBurgerController::test_caseIncomingDataFragmentedMultipleCommands()
{
    QByteArray d = DebugHelper::hexStringToByteArray("4C4544205B3120646174615D203030200A50696E67205B3020646174615D200A010600044C4544205B01B0000401B0000401B0000401B00004");

    BurgerControllerWrapper ctrl;
    QSignalSpy spyPongReceived(&ctrl, &BurgerController::pongReceived);
    QSignalSpy spyButtonPressed(&ctrl, &BurgerController::buttonPressed);

    //We fragment data into packet of random size < 8 bytes
    while(!d.isEmpty()) {
        int len = QRandomGenerator::global()->generate() % 8;
        qDebug() << "Packet size : " << len;
        QByteArray subd = d.left(len);
        ctrl.onDataReceived(subd);
        d.remove(0, len);
    }

    spyPongReceived.wait(500);
    spyButtonPressed.wait(500);
    QVERIFY(spyPongReceived.count() == 1);
    QCOMPARE(spyButtonPressed.count(), 4);
}

QTEST_MAIN(TestBurgerController)

#include "tst_testburgercontroller.moc"
