#include "SerialPortController.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include "DebugHelper.hpp"

SerialPortController::SerialPortController(QObject* parent)
    : QObject(parent)
{}

SerialPortController::~SerialPortController()
{
}

QStringList SerialPortController::availablePorts()
{
    qInfo() << "Recherche des ports série...";
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    qInfo() << ports.size() << " ports trouvés";

    QStringList portsNames;
    foreach(QSerialPortInfo port, ports) {
        //Filtrage des ports non STM32
        QRegExp regexp("STM32 STLink|USB Serial Device");

        //qInfo() << port.description() << "," << port.vendorIdentifier();
        if(regexp.indexIn(port.description()) == -1) {
            //qInfo() << "Port " << port.portName() << " filtré";
            continue;
        }

        //qInfo() << "Port : " << port.portName() << ", description : " << port.description() << ", location : " << port.systemLocation();
        portsNames << port.portName();
    }

    return portsNames;
}

void SerialPortController::internalOpenPort(QString portName)
{
    qInfo() << "Tentative d'ouverture du port " << portName;

    currentPort_ = new QSerialPort(portName, this); //Cette instance doit absolument être créée dans le bon thread
    if(currentPort_ == nullptr) {
        qWarning() << "Le port n'a pas été créé.";
        return;
    }

    bool ok = currentPort_->open(QIODevice::ReadWrite);

    if(ok) {
        qInfo() << "Port ouvert.";
        connect(currentPort_, &QSerialPort::readyRead, this, &SerialPortController::onReadyRead);
        currentPort_->setBaudRate(QSerialPort::Baud115200);
        currentPort_->setDataBits(QSerialPort::Data8);
        currentPort_->setParity(QSerialPort::NoParity);
        currentPort_->setStopBits(QSerialPort::OneStop);
        emit portOpened();
    } else {
        emit portNotOpened();
        qWarning() << "Le port " << portName << " n'a pas été ouvert. Code=" << currentPort_->error() << ". Erreur=" << currentPort_->errorString();
    }
}

void SerialPortController::internalWriteDataOnPort(QByteArray data)
{
    if(!currentPort_->isOpen()) {
        qWarning() << "Le port n'est pas ouvert";
        return;
    }

    if(currentPort_ == nullptr) {
        qWarning() << "Le port n'est pas initialisé";
    } else {
        qint64 res = currentPort_->write(data);
        if(res == -1) {
            qWarning() << "Une erreur est survenue lors de l'écriture sur le port " << currentPort_->portName() << ". Code=" << currentPort_->error() << ". Erreur=" << currentPort_->errorString();
        } else {
            emit dataWritten(res);
        }
    }
}

void SerialPortController::internalClosePort()
{
    if(currentPort_) {
        qInfo() << "Fermeture du port";
        currentPort_->close();
    }
}

static QMutex readMutex_;
void SerialPortController::onReadyRead()
{
    //Gère la ré-entrance de la fonction
    QMutexLocker lock(&readMutex_);

    qInfo() << "Données reçues sur le port " << currentPort_->portName();    
    QByteArray data = currentPort_->readAll();
    qInfo() << " >>> (hexa)" << DebugHelper::byteArrayToString(data);
    qInfo() << " >>> (raw)" << data;
    emit dataReceived(data);
}
