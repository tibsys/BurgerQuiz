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
    qInfo() << "Démarrage du jeu Burger Quiz";

    qInfo() << "Etape 1 : recherche des dispositifs burger";
    burger1_->findBurger();
}

void GameController::onBurgerReady()
{
    BurgerController *ctrl = qobject_cast<BurgerController*>(QObject::sender());
    qInfo() << "Le burger de l'équipe " << DebugHelper::teamToString(ctrl->team()) << " est prêt";
    Burger burger = ctrl->burger();
    emit burgerInitialized(burger.team());

    qInfo() << "Clignotement de la LED du burger";
    ctrl->startBlinking();

    if(initializedBurgersCount() < 2) {        
        QTimer::singleShot(5000, this, SLOT(findSecondBurger()));
    } else {
        qInfo() << "Tous les burgers sont initialisés. Le jeu peut commencer";
        emit gameReady();

        gameState_ = INITIALIZED;
    }
}

void GameController::findSecondBurger()
{
    qInfo() << "Recherche du second burger";
    burger2_->findBurger();
}


//Fonction d'arbitrage
static QMutex arbitreMutex_;
void GameController::onButtonPressed()
{
    //On joue tout de suite un son, peu importe l'état du jeu
    QSound::play(":/sounds/buzz.wav");

    if(gameState_ == TEAM_SELECTED) {
        qInfo() << "Vous avez buzzé trop tard...";
        return;
    }

    if(gameState_ != SELECTION) {
        qInfo() << "Vous avez buzzé trop top, attendez que la sélection commence";
        return;
    }

    QMutexLocker lock(&arbitreMutex_); //On fait tout dans une section critique

    BurgerController *ctrl = qobject_cast<BurgerController*>(QObject::sender());
    qInfo() << "Le burger de l'équipe " << DebugHelper::teamToString(ctrl->team()) << " a buzzé !";

    //Activation de l'automate des burgers
    Burger selectionne = ctrl->burger();
    Burger elimine = (ctrl->burger().team() == burger1_->team() ? burger2_->burger() : burger1_->burger());

    qInfo() << "Resultat de l'arbitrage : ";
    qInfo() << "Sélectionné : " << DebugHelper::teamToString(selectionne.team());
    qInfo() << "Eliminé : " << DebugHelper::teamToString(elimine.team());

    gameState_ = TEAM_SELECTED;
    emit burgerSelected(selectionne.team());
    ctrl->startBlinking();

    ctrl->selectionne();
}

void GameController::onGameStateChanged()
{
    qInfo() << "L'état du jeu a changé";
}

void GameController::setReady()
{
    qInfo() << "Remet le jeu à l'état Prêt.";

    qInfo() << "Extinction de la LED de l'équipe MAYO";
    burger1_->ledOff();

    qInfo() << "Extinction de la LED de l'équipe KETCHUP";
    burger2_->ledOff();

    gameState_ = SELECTION;
}
