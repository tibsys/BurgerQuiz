#ifndef BURGER_H
#define BURGER_H

#include <QExplicitlySharedDataPointer>
#include <QQmlEngine>
#include "Constants.h"

/**
 * @brief Cette classe contient uniquement les données gérée par le proxy Burger.
 * \see Burger
 */
class BurgerData : public QSharedData
{    
public:
    BurgerData() {}
    ~BurgerData() {}

public:
    /**
     * @brief Le nom du port de communication tel que retournée par la classe QSerialPortInfo
     */
    QString portName;
    /**
     * @brief L'état courant du burger. \see Burger::BurgerState.
     */
    int state = 0;
    /**
     * @brief L'état courant de la LED du burger. \see Burger::LedState.
     */
    int ledState;
    /**
     * @brief L'équipe à laquelle appartient ce burger. \see Burger::Team.
     */
    int team = 0;
};

/**
 * @brief Cette classe encapsule les données relatives à un burger, son état courant, l'état de sa LED et son équipe.
 * C'est un proxy qui masque les données encapsulées par la classe BurgerData. \see BurgerData.
 * L'instance de BurgerData est stockée sous la forme d'un pointeur partagé sans copie à l'écriture. Ainsi les modifications
 * sur l'état du burger sont reportées partout dans l'application.
 */
class Burger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief L'équipe de jeu
     */
    enum Team {
        INCONNUE = 0, /*!< Aucune équipe définie, état initial */
        MAYO = 1, /*!< Equipe Mayo */
        KETCHUP = 2 /*!< Equipe Ketchup */
    };
    Q_ENUMS(Team)

    explicit Burger();
    virtual ~Burger();
    Burger(const Burger& orig)
        : QObject(orig.parent()), d(orig.d) {}
    Burger(Team team, QString portName);

    /**
     * @brief L'état courant du burger
     */
    enum BurgerState {
        STATE_UNKNOWN = 0, /*!< Etat inconnu, état initial */
        STATE_PINGED, /*!< Le PING a été envoyé */
        STATE_ENABLED, /*!< Le burger est activé */
        STATE_DISABLED, /*!< Le burger est désactivé */
        STATE_ERROR, /*!< Le burger est en erreur */
        STATE_SELECTED /*!< Le burger est sélectionné */
    };

    /**
     * @brief L'état de la LED du burger
     */
    enum LedState {
        LED_UNKNOWN = -1, /*!< Etat inconnnu, état initial */
        LED_ON = 1, /*!< LED allumée */
        LED_OFF = 0, /*!< LED éteinte */
        LED_BLINK = 2 /*!< LED clignotante */
    };    

    /**
     * @brief Retourne le nom du port de ce burger.
     */
    QString portName() const;
    /**
     * @brief Définit le nom du port de ce burger
     */
    void setPortName(QString name);
    /**
     * @brief Retourne l'état de la LED de ce burger.
     */
    LedState ledState() const;
    /**
     * @brief Définit l'état de la LED de ce burger.
     */
    void setLedState(LedState state);
    /**
     * @brief Retourne l'état de ce burger.
     */
    BurgerState burgerState() const;
    /**
     * @brief Définit l'état de ce burger
     */
    void setBurgerState(BurgerState state);
    /**
     * @brief Définit l'équipe associée à ce burger
     */
    void setTeam(Team team);    
    /**
     * @brief Définit l'état de ce burger à ACTIVE
     */
    void enable();
    /**
     * @brief Définit l'état de ce burger à DESACTIVE
     */
    void disable();
    /**
     * @brief Retourne l'équipe associée à ce burger
     */
    Team team() const;
    /**
     * @brief Retourne l'état actuel de ce burger.
     * @return Vrai si l'équipe de ce burger est définie.
     */
    bool valid() const;
    /**
     * @brief Retourne l'état actuel de ce burger.
     * @return Vrai si le burger est valide (\see valid()) et si l'état actuel est ACTIVE.
     */
    bool isReady() const;
    /**
     * @brief Retourne l'état d'activation de ce burger
     */
    bool isEnabled() const;

signals:
    /**
     * @brief Signal émis lorsque l'équipe associée à ce burger est changée
     */
    void teamChanged();

private:
    QExplicitlySharedDataPointer<BurgerData> d;
};
Q_DECLARE_METATYPE(Burger)
Q_DECLARE_METATYPE(Burger::Team)

#endif // BURGER_H
