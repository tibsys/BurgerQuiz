#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QSerialPort>
#include <QMap>
#include <QDebug>
#include <QTimer>
#include <QThread>

/**
 * @brief Cette classe est chargée de la communication série avec le dispositif Burger.
 *
 * Elle est associée aux cas d'utilisation suivants :
 *
 */
class SerialPortController : public QObject
{
    Q_OBJECT
public:    
    SerialPortController(QObject* parent = nullptr);
    ~SerialPortController();

    /**
     * @brief Récupère la liste des ports de communication série disponibles sur la machine.
     * La liste est filtrée sur le mot clé STM32 STLink (par expression régulière).
     * @return une liste de noms de ports disponibles.
     */
    static QStringList availablePorts();
    /**
     * @brief Retourne le nom du port de communication ouvert.
     */
    QString portName() const { return currentPort_->portName(); }

    /**
     * @brief Déclenche l'ouverture du port de communication. Cette fonction est un proxy vers la fonction internalOpenPort() et permet
     * une exécution dans le thread du contrôleur et non dans celui de l'appelant.
     * \see internalPortOpen
     * @param Le nom du port à ouvrir
     */
    void openPort(QString portName) {
        QMetaObject::invokeMethod(this, "internalOpenPort", Qt::QueuedConnection, Q_ARG(QString, portName));
    }
    /**
     * @brief Ferme le port de communication actuellement ouvert.
     */
    void closePort();
    /**
     * @brief Ecrit des données dans le buffer de communication du port série. Cette fonction est un proxy vers la fonction internalWriteDataOnPort() et permet
     * une exécution dans le thread du contrôleur et non dans celui de l'appelant.
     */
    void writeDataOnPort(QByteArray data) {
        QMetaObject::invokeMethod(this, "internalWriteDataOnPort", Qt::QueuedConnection, Q_ARG(QByteArray, data));
    }

signals:    
    /**
     * @brief Signal émis lorsque le port a été correctement ouvert.
     */
    void portOpened();
    /**
     * @brief Signal émis lorsque le port n'a pas pu être ouvert.
     * \see internalOpenPort()
     */
    void portNotOpened();
    /**
     * @brief Signal émis lorsque des données ont été reçues sur le port de communication.
     */
    void dataReceived(QByteArray);
    /**
     * @brief Signal émis lorsque des données ont été écrites dans le buffer du port.
     * @param len la longueur (en octets) des données écrites
     */
    void dataWritten(qint64 len);

private slots:
    /**
     * @brief Ecrit des données dans le buffer du port de communication/
     */
    void internalWriteDataOnPort(QByteArray);
    /**
     * @brief Effectue une tentative d'ouverture du port de communication. Cette fonction est bloquante et est la raison pour laquelle
     * tout ce contrôleur est exécuté dans un thread.
     * @param portName
     */
    void internalOpenPort(QString portName);
    /**
     * @brief fonction appelée lorsque le port de communication a des données en tampon et qu'il est prêt à les envoyer.
     */
    void onReadyRead();

private:    
    /**
     * @brief Le port série géré par ce contrôleur
     */
    QSerialPort *currentPort_ = nullptr;    
};

#endif // SERIALPORTCONTROLLER_H
