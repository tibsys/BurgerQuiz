#include <QtTest>
#include <QSerialPortInfo>

// add necessary includes here
#include "SerialPortController.h"

class TestSerialPortController : public QObject
{
    Q_OBJECT

public:
    TestSerialPortController();
    ~TestSerialPortController();

private slots:
    void test_caseGetAvailablePorts();

};

TestSerialPortController::TestSerialPortController()
{

}

TestSerialPortController::~TestSerialPortController()
{

}

void TestSerialPortController::test_caseGetAvailablePorts()
{
    QList<QSerialPortInfo> ports;
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        if(port.description().contains("STM32")) {
            ports << port;
        }
    }

    QStringList portsNames = SerialPortController::availablePorts();
    QCOMPARE(ports.count(), portsNames.count());
    foreach(QSerialPortInfo info, ports) {
        QVERIFY(portsNames.contains(info.portName()));
    }
}

QTEST_APPLESS_MAIN(TestSerialPortController)

#include "tst_testserialportcontroller.moc"
