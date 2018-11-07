#include <QtTest>
#include <QSerialPortInfo>

// add necessary includes here
#include "MessageFactory.hpp"

class TestMessageFactory : public QObject
{
    Q_OBJECT

public:
    TestMessageFactory();
    ~TestMessageFactory();

private slots:
    void test_caseMessageHeader();
    void test_caseMessageTerminator();
    void test_caseGetCommandId();
    void test_caseMakePingMessage();
    void test_caseMakePongMessage();
    void test_caseMakeButtonPressedMessage();
    void test_caseMakeLedOnMessage();
    void test_caseMakeLedOffMessage();
    void test_caseMakeLedBlinkMessage();
    void test_caseIsValidMessage();
    void test_caseMessageTypeFromData();
};

TestMessageFactory::TestMessageFactory()
{

}

TestMessageFactory::~TestMessageFactory()
{

}

void TestMessageFactory::test_caseMessageHeader()
{
    QByteArray d = MessageFactory::messageHeader();
    QCOMPARE(d, QByteArray(1,1));
}

void TestMessageFactory::test_caseMessageTerminator()
{
    QByteArray d = MessageFactory::messageTermination();
    QCOMPARE(d, QByteArray(1,4));
}

void TestMessageFactory::test_caseGetCommandId()
{
    QCOMPARE(MessageFactory::getCommandId(MessageFactory::PING), QByteArray(1, 0x16));
    QCOMPARE(MessageFactory::getCommandId(MessageFactory::PONG), QByteArray(1, 0x6));
    QCOMPARE(MessageFactory::getCommandId(MessageFactory::LED), QByteArray(1, 0xED));
    QCOMPARE(MessageFactory::getCommandId(MessageFactory::BUTTON), QByteArray(1, 0xB0));
}

void TestMessageFactory::test_caseMakePingMessage()
{
    QByteArray msg = MessageFactory::makePingMessage();
    QVERIFY(msg.size() == 4);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01160004");
}

void TestMessageFactory::test_caseMakePongMessage()
{
    QByteArray msg = MessageFactory::makePongMessage();
    QVERIFY(msg.size() == 4);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01060004");
}

void TestMessageFactory::test_caseMakeButtonPressedMessage()
{
    QByteArray msg = MessageFactory::makeButtonPressedMessage();
    QVERIFY(msg.size() == 4);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01B00004");
}

void TestMessageFactory::test_caseMakeLedOnMessage()
{
    QByteArray msg = MessageFactory::makeLedMessage(Burger::LED_ON);
    QVERIFY(msg.size() == 5);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01ED010104");
}

void TestMessageFactory::test_caseMakeLedOffMessage()
{
    QByteArray msg = MessageFactory::makeLedMessage(Burger::LED_OFF);
    QVERIFY(msg.size() == 5);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01ED010004");
}

void TestMessageFactory::test_caseMakeLedBlinkMessage()
{
    QByteArray msg = MessageFactory::makeLedMessage(Burger::LED_BLINK);
    QVERIFY(msg.size() == 5);
    QCOMPARE(DebugHelper::byteArrayToString(msg), "01ED010204");
}

void TestMessageFactory::test_caseIsValidMessage()
{
    QByteArray msg = MessageFactory::makePingMessage();
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = MessageFactory::makePongMessage();
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = MessageFactory::makeButtonPressedMessage();
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = MessageFactory::makeLedMessage(Burger::LED_ON);
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = MessageFactory::makeLedMessage(Burger::LED_OFF);
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = MessageFactory::makeLedMessage(Burger::LED_BLINK);
    QVERIFY(MessageFactory::isMessageValid(msg));

    msg = QString::fromLatin1("randommsg").toLatin1();
    QVERIFY(!MessageFactory::isMessageValid(msg));
}

void TestMessageFactory::test_caseMessageTypeFromData()
{
    QByteArray d = QByteArray::fromHex("01060004");
    MessageFactory::Message msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::PONG);

    d = QByteArray::fromHex("01160004");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::PING);

    d = QByteArray::fromHex("01ED010104");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::LED);

    d = QByteArray::fromHex("01ED010004");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::LED);

    d = QByteArray::fromHex("01ED010204");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::LED);

    d = QByteArray::fromHex("01B00004");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::BUTTON);

    d = QByteArray::fromHex("01120004");
    msg = MessageFactory::messageTypeFromData(d);
    QCOMPARE(msg, MessageFactory::MESSAGE_UNKNOWN);
}

QTEST_APPLESS_MAIN(TestMessageFactory)

#include "tst_testmessagefactory.moc"
