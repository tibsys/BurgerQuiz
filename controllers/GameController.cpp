#include "GameController.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QSound>
#include "MessageFactory.hpp"
#include "DebugHelper.hpp"
#include "Burger.h"

GameController::GameController(QObject *parent)
    : QObject(parent), burger1_(new BurgerController(this)), burger2_(new BurgerController(this))
{
    connect(burger1_, &BurgerController::ready, this, &GameController::onBurgerReady);
    connect(burger2_, &BurgerController::ready, this, &GameController::onBurgerReady);
    connect(burger1_, &BurgerController::buttonPressed, this, &GameController::onButtonPressed);
    connect(burger2_, &BurgerController::buttonPressed, this, &GameController::onButtonPressed);
    burger1_->setTeam(Burger::MAYO);
    burger2_->setTeam(Burger::KETCHUP);
    burger1_->initializeStateMachine();
    burger2_->initializeStateMachine();
    connect(this, &GameController::gameStateChanged, this, &GameController::onGameStateChanged);
}

bool GameController::isReady() const {
    return burger1_->burger().isReady() && burger2_->burger().isReady();
}

void GameController::initialize()
{
    qDebug() << "Démarrage du jeu Burger Quiz";

    qDebug() << "Etape 1 : recherche des dispositifs burger";
    burger1_->findBurger();
}

void GameController::onBurgerReady()
{
    BurgerController *ctrl = qobject_cast<BurgerController*>(QObject::sender());
    qDebug() << "Le burger de l'équipe " << DebugHelper::teamToString(ctrl->team()) << " est prêt";
    Burger burger = ctrl->burger();
    emit burgerInitialized(burger.team());

    qDebug() << "Clignotement de la LED du burger";
    ctrl->startBlinking();

    if(initializedBurgersCount() < 2) {        
        QTimer::singleShot(5000, this, SLOT(findSecondBurger()));
    } else {
        qDebug() << "Tous les burgers sont initialisés. Le jeu peut commencer";
        emit gameReady();

        gameState_ = INITIALIZED;
    }
}

void GameController::findSecondBurger()
{
    qDebug() << "Recherche du second burger";
    burger2_->findBurger();
}


//Fonction d'arbitrage
static QMutex arbitreMutex_;
void GameController::onButtonPressed()
{
    //On joue tout de suite un son, peu importe l'état du jeu
    QSound::play(":/sounds/buzz.wav");

    if(gameState_ == TEAM_SELECTED) {
        qDebug() << "Vous avez buzzé trop tard...";
        return;
    }

    if(gameState_ != SELECTION) {
        qDebug() << "Vous avez buzzé trop top, attendez que la sélection commence";
        return;
    }

    QMutexLocker lock(&arbitreMutex_); //On fait tout dans une section critique

    BurgerController *ctrl = qobject_cast<BurgerController*>(QObject::sender());
    qDebug() << "Le burger de l'équipe " << DebugHelper::teamToString(ctrl->team()) << " a buzzé !";

    //Activation de l'automate des burgers
    Burger selectionne = ctrl->burger();
    Burger elimine = (ctrl->burger().team() == burger1_->team() ? burger2_->burger() : burger1_->burger());

    qDebug() << "Resultat de l'arbitrage : ";
    qDebug() << "Sélectionné : " << DebugHelper::teamToString(selectionne.team());
    qDebug() << "Eliminé : " << DebugHelper::teamToString(elimine.team());

    gameState_ = TEAM_SELECTED;
    emit burgerSelected(selectionne.team());

    ctrl->selectionne();
}

void GameController::onGameStateChanged()
{
    qDebug() << "L'état du jeu a changé";
}
