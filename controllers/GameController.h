#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QString>
#include <QMap>
#include "Burger.h"
#include "SerialPortController.h"
#include "BurgerController.h"

/**
 * @brief The GameController class
 *
 * Ceci est le contrôleur principal du jeu. Il est l'interface entre le matériel (et les joueurs) et la GUI.
 * Ce contrôleur reçoit les signaux liés à la communication avec les burgers et est responsable de l'arbitrage dans la
 * phase de sélection du jeu.
 * Il déclenche le changement d'état de chaque burger et organise les transitions de la GUI.
 *
 * Il est associé aux cas d'utilisation suivants :
 * - Ouverture de deux ports
 * - Négociation avec les dispositifs burger (Ping et Pong)
 * - Activation du jeu quand deux ports sont ouverts
 * - Traitement des automates à état des contrôleurs de protocole sous-jacents
 * - Activation et désactivation des dispositifs
 * - Arbitrage entre les dispositifs burgers
 * - Affichage de la LED du dispositif burger le plus rapide
 */
class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GameState gameState MEMBER gameState_ NOTIFY gameStateChanged)
public:
    explicit GameController(QObject *parent = nullptr);    

    /**
     * @brief Renvoie l'état du jeu
     * @return Vrai si deux burgers sont initialisés et prêts.
     */
    bool isReady() const;

    /**
     * @brief Enumération gérant les différentes phases du jeu
     */
    enum GameState {
        INITIALIZING = 0, /*!< le matériel est en phase d'initialisation */
        INITIALIZED, /*!< le matériel est initialisé, le jeu commence */
        SELECTION, /*!< en attente d'un buzzer */
        TEAM_SELECTED, /*!< un buzzer a été sélectionné */
        FINAL /*!< non utilisé */
    };
    Q_ENUMS(GameState)

private:
    /**
     * @brief Retourne le nombre de buzzer initialisés
     */
    int initializedBurgersCount() const {
        int count = 0;
        if(burger1_->isInitialized())
            count += 1;
        if(burger2_->isInitialized())
            count += 2;
        return count;
    }

signals:
    /**
     * @brief Signal émis lorsque le jeu peut commencer.
     */
    void gameReady();
    /**
     * @brief Signal émis lorsqu'un burger est sélectionné
     */
    void burgerSelected(Burger::Team team);
    /**
     * @brief Signal émis lorsqu'un burger a été initialisé
     * @param team
     */
    void burgerInitialized(Burger::Team team);
    /**
     * @brief Signal émis lorsque l'état du jeu a changé. \see GameState.
     */
    void gameStateChanged();

private slots:
    /**
     * @brief fonction appelée lorsqu'un burger est prêt
     */
    void onBurgerReady();
    /**
     * @brief fonction appelée lorsqu'un burger a buzzé. Elle gère la réentrance et arbitre en deux dispositifs qui buzzent au même moment,
     * cet arbitrage aboutit à une sélection et un changement d'état du jeu et des dispositifs.
     */
    void onButtonPressed();
    /**
     * @brief fonction appelée lors de la recherche asynchrone de dispositifs burgers
     */
    void findSecondBurger();
    /**
     * @brief fonction appelée lorsque l'état du jeu a changé.
     */
    void onGameStateChanged();

public slots:
        /**
         * @brief Cette fonction déclenche l'initialisation du matériel. Elle récupère la liste des ports de communication
         * disponibles et elle teste chaque port en effectuant une bouble asynchrone, jusqu'à ce qu'elle trouve 2 burgers.
         *
         * Seuls les 2 premiers burgers initialisés sont retenus, les autres sont ignorés.
         */
    void initialize();

private:    
    BurgerController *burger1_;
    BurgerController *burger2_;
    GameState gameState_ = INITIALIZING;
};

#endif // GAMECONTROLLER_H
