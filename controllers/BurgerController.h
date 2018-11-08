#ifndef BURGERCONTROLLER_H
#define BURGERCONTROLLER_H

#include <QObject>
#include <QStateMachine>
#include <QDebug>
#include <QThread>

#include "Burger.h"
#include "SerialPortController.h"
#include "Constants.h"
#include "DebugHelper.hpp"

/**
 * @brief The BurgerController class
 * Basé sur les spécifications du document 2018_10_SpecBurgerDialog.docx
 *
 * Ce contrôleur gère le protocole de communication avec un dispositif "Burger" et les différentes phases du
 * jeu sous la forme d'un automate à états.
 * une application comporte deux dispositifs Burger, chacun étant géré par une instance spécifique de ProtocolController sur un port spécifique.
 *
 * Il ne gère pas la communication, celle-ci est gérée par la classe SerialPortController. La communication avec le port série est déplacée dans
 * un thread dédié qui rend non bloquante la phase d'ouverture du port lorsqu'aucun dispositif n'est connecté.
 * En revanche il effectue une découverte des dispositifs burgers connectés sur un port de communication série.
 *
 * Ce contrôleur est associé aux cas d'utilisation suivants :
 * -
 *
 * Automate à états :
 * [] est un état
 * () est une transition
 *
 * [Etat initial] = [Non initialisé] -> (init) -> [Initialisation] -> (port ouvert) -> [Port ouvert] -> (envoi du ping)
 * -> [Ping envoyé] -> (réception du pong) -> [Prêt] -> (activation du jeu) -> [En écoute] -> (bouton pressé)
 * -> [Bouton pressé] -> (émission signal) -> [En écoute]
 *
 * Les cas de sortie (erreur ou valides) de l'automate sont liés à
 * - une transition invalide (une demande d'activation alors que l'automate n'est pas à l'état prêt)
 * - une erreur matérielle
 * - une erreur logicielle
 * Ces cas aboutissent à l'émission d'un signal d'erreur (non implanté actuellement).
 *
 * Format des trames :
 * SOH=Start Of Header
 * La taille des données transmises est variable et multiple de 8 bits
 *
 * ------------------------------------------------------------------
 * |  Nom du champ | SOH | ID msg | Taille | Payload  | EOT |
 * ------------------------------------------------------------------
 * |  Taille (bits)        |    8   |      8      |     8   | variable  |    8   |
 * -------------------------------------------------------------------
 *
 * Messages :
 * ***********************************************************************
 * | Identifiant | Valeur | Taille | Remarques
 * | Ping          | 0x16     | -        | pas de payload
 * | Pong         | 0x06    | -        | pas de payload
 * | LED           | 0xED    | 1        | 0x00 (OFF), 0x01 (ON), 0x02 (Blink)
 * | Button      | 0xB0     | -        | pas de payload
 * ************************************************************************
 *
 * Exemple de messages :
 * Ping : 01 16 00 04
 * Ping : 01 06 00 04
 * LED ON : 01 ED 01 01 04
 * LED OFF : 01 ED 01 00 04
 * LED BLINK : 01 ED 01 02 04
 * Button pressed : 01 B0 00 04
 */
class BurgerController : public QObject
{
    Q_OBJECT
public:
    explicit BurgerController(QObject *parent = nullptr);
    ~BurgerController();

    /**
     * @brief Initialise l'automate a états
     */
    void initializeStateMachine();

    /**
     * @brief Formate et affiche un message de débogage dans la log
     * @param message Le message brut à afficher
     */
    void debugMessage(QString message) {
        qInfo() << QString("Equipe %1 : %3.").arg(DebugHelper::teamToString(burger_.team())).arg(message);
    }

    /**
     * @brief Retourne l'état du dispositif burger.
     * @return Vrai si le burger est initialité et prêt.
     */
    bool isInitialized() const {
        return (burger_.valid() && burger_.isReady()); //+vérif state machine
    }

    /**
     * @brief Retourne l'instance de burger associée à ce contrôleur
     */
    Burger burger() const { return burger_; }

    /**
     * @brief Définit l'équipe associée au dispositif Burger contrôlé
     */
    void setTeam(Burger::Team team) { burger_.setTeam(team); }

    /**
     * @brief Retourne l'équipe associée au dispositif Burger contrôlé
     */
    Burger::Team team() const { return burger_.team(); }

    /**
     * @brief Désactive le dispositif burger contrôlé
     */
    void disableBurger() { burger_.disable(); }

    /**
     * @brief Active le dispositif burger contrôlé
     */
    void enableBurger() { burger_.enable(); }

    /**
     * @brief Retourne l'état de sélection du dispositif contrôlé
     */
    void setSelectionne() { emit selectionne(); }

signals:
    /**
     * @brief Signal émis au cas où une erreur se produit (matérielle ou fonctionnelle). Non implanté.
     */
    void error(QString);

    /**
     * @brief Signal émis après l'initialisation du dispositif
     */
    void initialized();

    /**
     * @brief Signal émis après l'ouverture du port de communication série vers le dispositif
     */
    void portOpened();

    /**
     * @brief Signal émis lorsque le PING a été écrit dans le tampon du port de communication série
     */
    void pingSent();

    /**
     * @brief Signal émis lorsqu'un PONG a été reçu sur le port de communication série
     */
    void pongReceived();

    /**
     * @brief Signal émis lorsque le dispositif est prêt, c'est-à-dire lorsque l'automate est entré dans l'état PRET.
     */
    void ready();

    /**
     * @brief Signal émis lorsque le bouton du dispositif a été pressé.
     */
    void buttonPressed();    

    /**
     * @brief Signal émis lorsque le dispositif a été défini comme sélectionné, c'est-à-dire lorsqu'il a été le plus rapide à buzzer.
     */
    void selectionne();

public slots:
    /**
     * @brief Déclenche la phase d'initialisation du dispositif.
     * Cela signifie que l'automate a état est mis dans son état initial et que le contrôleur va chercher un port sur lequel se trouve
     * un matériel compatible. Si un matériel compatible est trouvé, un PING est envoyé.
     */
    void findBurger();
    /**
     * @brief Déclenche le clignotement de la LED du dispositif connecté
     */
    void startBlinking();

protected slots:
    /**
     * @brief fonction appelée lorsque le port de communication est ouvert.
     */
    void onPortOpened();
    /**
     * @brief fonction appelée lorsque le prot de communication n'a pas pu être ouvert.
     */
    void onPortNotOpened();
    /**
     * @brief fonction d'itération sur la liste des ports détectés, dans le but de découvrir un dispositif burger.
     */
    void tryNextPort();
    /**
     * @brief fonction appelée lorsque des données ont été reçues depuis le port de communication
     */
    void onDataReceived(QByteArray);
    /**
     * @brief fonction appelée lorsque des données ont été écrites dans le buffer du port de communication.
     */
    void onDataWritten(quint64);
    /**
     * @brief fonction appelée lorsque le PING n'a pas reçu de réponse au bout du délai imparti (voir code source).
     */
    void onPingTimeout();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état INITIALISE
     */
    void onStateInitialized();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état PORT OUVERT
     */
    void onEtatPortOpened();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état PING ENVOYE
     */
    void onEtatPingSent();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état PONG RECU
     */
    void onEtatPongReceived();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état PRET
     */
    void onEtatPret();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état BOUTON PRESSE
     */
    void onEtatButtonPressed();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état DESACTIVE (osbolète).
     */
    void onEtatDesactive();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état SELECTIONNE
     */
    void onEtatSelectionne();
    /**
     * @brief fonction appelée lorsque l'automate entre dans l'état ERREUR (non implanté)
     */
    void onEtatErreur();
    /**
     * @brief interrompt le clognotement de la LED. Cette fonction est appelée automatiquement par le timer déclenché lors
     * de l'appel à la fonction startBlinking()
     */
    void stopBlinking();

private:
    //Variables membres liées à l'automate à états
    QStateMachine automate_;
    QState stateInitialized_;
    QState etatPortOuvert_;
    QState etatPingEnvoye_;
    QState etatPongRecu_;
    QState etatPret_;
    QState etatBoutonPresse_;
    QState etatDesactive_;
    QState etatSelectionne_;
    QState etatErreur_;

    //Dispositif burger (encapsulation)
    Burger burger_;

    //Liste des ports série détectés, utilisée pour l'itération asynchrone
    QStringList availablePorts_;

    //Contrôleur du port série
    SerialPortController *serialPortController_ = nullptr;
    //Thread du contrôleur de port série
    QThread *serialPortThread_ = nullptr;
    //Timer permettant de gérer le timeout pour le PING
    QTimer *timerPingTimeout_ = nullptr;
    //Tampon de réception des données depuis le port série
    QByteArray recvBuffer_;
};

#endif // BURGERCONTROLLER_H
