#include "BurgerController.h"
#include "SerialPortController.h"
#include "MessageFactory.hpp"
#include "DebugHelper.hpp"
#include <QTimer>
#include <QRandomGenerator>

BurgerController::BurgerController(QObject *parent)
    : QObject(parent), timerPingTimeout_(new QTimer(this))
{        
    timerPingTimeout_->setSingleShot(true);
    connect(timerPingTimeout_, &QTimer::timeout, this, &BurgerController::onPingTimeout);
}

BurgerController::~BurgerController()
{
    qInfo() << "Fermeture du ProtocolController";
    automate_.stop();
    if(serialPortController_ != nullptr) {
        serialPortController_->closePort();
    }
    if(serialPortThread_ != nullptr) {
        serialPortThread_->quit();
    }
}

void BurgerController::initializeStateMachine()
{
    automate_.addState(&stateInitialized_);
    automate_.addState(&etatPortOuvert_);
    automate_.addState(&etatPingEnvoye_);
    automate_.addState(&etatPongRecu_);
    automate_.addState(&etatPret_);
    automate_.addState(&etatBoutonPresse_); 
    automate_.addState(&etatDesactive_);
    automate_.addState(&etatSelectionne_);
    automate_.addState(&etatErreur_);
    automate_.setInitialState(&stateInitialized_);

    stateInitialized_.addTransition(this, &BurgerController::portOpened, &etatPortOuvert_);
    etatPortOuvert_.addTransition(this, &BurgerController::pingSent, &etatPingEnvoye_);
    etatPingEnvoye_.addTransition(this, &BurgerController::pongReceived, &etatPongRecu_);
    etatPongRecu_.addTransition(this, &BurgerController::ready, &etatPret_);
    etatPret_.addTransition(this, &BurgerController::selectionne, &etatSelectionne_);

    connect(&stateInitialized_, &QState::entered, this, &BurgerController::onStateInitialized);
    connect(&etatPortOuvert_, &QState::entered, this, &BurgerController::onEtatPortOpened);
    connect(&etatPingEnvoye_, &QState::entered, this, &BurgerController::onEtatPingSent);
    connect(&etatPongRecu_, &QState::entered, this, &BurgerController::onEtatPongReceived);
    connect(&etatPret_, &QState::entered, this, &BurgerController::onEtatPret);
    connect(&etatBoutonPresse_, &QState::entered, this, &BurgerController::onEtatButtonPressed);
    connect(&etatDesactive_, &QState::entered, this, &BurgerController::onEtatDesactive);
    connect(&etatSelectionne_, &QState::entered, this, &BurgerController::onEtatSelectionne);
    connect(&etatBoutonPresse_, &QState::entered, this, &BurgerController::onEtatButtonPressed);
    connect(&etatErreur_, &QState::entered, this, &BurgerController::onEtatErreur);
}

void BurgerController::findBurger()
{
    if(serialPortController_ == nullptr) {
        serialPortController_ = new SerialPortController;
        serialPortThread_ = new QThread(this);
        connect(serialPortController_, &SerialPortController::portOpened, this, &BurgerController::onPortOpened);
        connect(serialPortController_, &SerialPortController::portNotOpened, this, &BurgerController::onPortNotOpened);
        connect(serialPortController_, &SerialPortController::dataWritten, this, &BurgerController::onDataWritten);
        connect(serialPortController_, &SerialPortController::dataReceived, this, &BurgerController::onDataReceived);

        serialPortController_->moveToThread(serialPortThread_);
        serialPortThread_->start();
    }

    debugMessage("Recherche de burgers connectés pour jouer");

    availablePorts_ = SerialPortController::availablePorts();
    QTimer::singleShot(100, this, SLOT(tryNextPort()));
}

void BurgerController::tryNextPort() {
    if(availablePorts_.isEmpty()) {
        qInfo() << "Aucun port de communication trouvé, nouvel essai dans 2 secondes";
        QTimer::singleShot(2000, this, SLOT(findBurger())); //Si la liste des ports est vide on recommence à 0 dans 2 secondes
        return;
    }

    QString portName = availablePorts_.takeLast();
    qInfo() << "Tentative de connexion au port " << portName;    

    automate_.start();
    serialPortController_->openPort(portName);
}

void BurgerController::onPortOpened()
{
    SerialPortController *ctrl = qobject_cast<SerialPortController*>(QObject::sender());
    qInfo() << "Un appareil est connecté au port " << ctrl->portName() << ". Démarrage de la négociation.";
    burger_.setPortName(ctrl->portName());

    //Activation de la transition
    emit portOpened();
}

void BurgerController::onPortNotOpened()
{
    qInfo() << "Aucun appareil connecté sur ce port";
    QTimer::singleShot(500, this, SLOT(tryNextPort())); //Si le port n'est pas dispo on essaye le suivant dans 500 ms
}

void BurgerController::onDataWritten(quint64 len)
{
    debugMessage(QString("%1 octets écrits sur le port").arg(len));
}

void BurgerController::onDataReceived(QByteArray data)
{    
    //Ici nous devons considérer que les données arrivent par morceaux
    //Le protocole indique qu'un message utile commence toujours par l'en-tête 0x01 (SOH)
    //se termine toujours par 0x04 (EOT)
    //Cependant le message utile peut-être précédé de messages de debug qui peuvent
    //être ignorés (selon la spécification du protocole) mais nous allons les
    //afficher directement en log
    recvBuffer_.append(data);

    //TODO: extraire cette fonction dans la factory pour la couvrir
    if(recvBuffer_.contains(MessageFactory::messageTermination()) && recvBuffer_.contains(MessageFactory::messageHeader())) {
        debugMessage(QString("Nous avons reçu un message : %1").arg(QString::fromLatin1(recvBuffer_)));
        int hdrStart = recvBuffer_.indexOf(MessageFactory::messageHeader());
        int msgEnd = recvBuffer_.indexOf(MessageFactory::messageTermination());
        qInfo() << "hdrStart=" << hdrStart << ", msgEnd=" << msgEnd;
        QString debugMsg = QString::fromUtf8(recvBuffer_.left(hdrStart)).trimmed();
        QByteArray command = recvBuffer_.mid(hdrStart, msgEnd-hdrStart+1);

        debugMessage(QString("Infos de debug : %1").arg(debugMsg));
        debugMessage("Commande : ");
        qInfo() << command;

        //Enfin, on laisse le surplus de données dans le buffer, il appartient peut-être à un prochain message
        recvBuffer_.remove(0, msgEnd+1);

        if(!MessageFactory::isMessageValid(command)) {
            debugMessage("Le message est invalide... Abandon");
            //TODO: Traiter l'erreur en fonction de l'état actuel du jeu
            return;
        }

        //Ici on traite le message en fonction de son type
        if(MessageFactory::messageTypeFromData(command) == MessageFactory::PONG) {
            debugMessage("Activation du burger");
            burger_.setBurgerState(Burger::STATE_ENABLED);

            //Déclenche la transition
            emit pongReceived();
            return;
        } else if(MessageFactory::messageTypeFromData(command) == MessageFactory::BUTTON) {
            debugMessage("Bouton pressé");
            emit buttonPressed();
        }

    }
}

void BurgerController::onStateInitialized()
{    
    debugMessage("Etat courant: initialisé");
}

void BurgerController::onEtatPortOpened()
{
    debugMessage(QString("Le port %1 a été correctement ouvert. Tentative d'activation d'un burger").arg(burger_.portName()));

    debugMessage("Exctinction de la LED");
    serialPortController_->writeDataOnPort(MessageFactory::makeLedMessage(Burger::LED_OFF));

    debugMessage("Envoi du ping");
    if(!burger_.portName().isEmpty()) {        

        QByteArray pingMessage = MessageFactory::makePingMessage();
        serialPortController_->writeDataOnPort(pingMessage);
        burger_.setBurgerState(Burger::STATE_PINGED);

        //Démarrage d'un timer : on laisse 2000 ms au dispositif pour répondre Pong, au-délà
        //on considère que ca n'est pas un burger
        timerPingTimeout_->start(2000);

        emit pingSent();
    }
}

void BurgerController::onEtatPingSent()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: PING envoyé";
    qInfo() << "En attente du PONG";
}

void BurgerController::onPingTimeout()
{
    qInfo() << "Le dispositif n'a pas répondu dans le délai imparti. Abandon.";
    serialPortController_->closePort();
    automate_.stop();
    QTimer::singleShot(10, this, SLOT(tryNextPort()));
}

void BurgerController::onEtatPongReceived()
{
    timerPingTimeout_->stop();

    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: PONG reçu";
    burger_.setBurgerState(Burger::STATE_ENABLED);
    emit ready();    
}

void BurgerController::onEtatPret()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: prêt";
}

void BurgerController::onEtatButtonPressed()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: boutton pressé";
}

void BurgerController::onEtatDesactive()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: désactivé";
    burger_.setBurgerState(Burger::STATE_DISABLED);
}

void BurgerController::onEtatSelectionne()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: sélectionné";
    burger_.setBurgerState(Burger::STATE_SELECTED);
    QTimer::singleShot(1, this, SLOT(startBlinking()));
}

void BurgerController::onEtatErreur()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Etat courant: erreur";
}

void BurgerController::startBlinking()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Clignotement de la LED";
    serialPortController_->writeDataOnPort(MessageFactory::makeLedMessage(Burger::LED_BLINK));
    //On arrête le clignotement ou bout de 2 secondes
    QTimer::singleShot(2000, this, SLOT(stopBlinking()));
}

void BurgerController::stopBlinking()
{
    qInfo() << "Burger " << DebugHelper::teamToString(burger_.team()) << ". Arrêt du clignotement de la LED";
    serialPortController_->writeDataOnPort(MessageFactory::makeLedMessage(Burger::LED_ON));
}
