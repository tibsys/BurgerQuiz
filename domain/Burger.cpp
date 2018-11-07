#include <QDebug>
#include "Burger.h"

Burger::Burger()
    : d(new BurgerData)
{ }

Burger::~Burger() {}

Burger::Burger(Team team, QString portName)
    : d(new BurgerData)
{
    d->team = team;
    d->portName = portName;
}

void Burger::setTeam(Team team)
{
    d->team = team;
}

Burger::Team Burger::team() const
{    
    return static_cast<Burger::Team>(d->team);
}

QString Burger::portName() const
{
    return d->portName;
}

void Burger::setPortName(QString portName) {
    d->portName = portName;
}

Burger::LedState Burger::ledState() const
{
    return static_cast<LedState>(d->ledState);
}

void Burger::setLedState(LedState state)
{
    d->ledState = state;
}

Burger::BurgerState Burger::burgerState() const
{
    return static_cast<BurgerState>(d->state);
}

void Burger::setBurgerState(BurgerState state)
{
    d->state = state;
}

bool Burger::valid() const
{
    return d->team != INCONNUE;
}

bool Burger::isReady() const
{
    return (valid() && (burgerState() == STATE_ENABLED));
}

void Burger::enable()
{
    d->state = STATE_ENABLED;
}

void Burger::disable()
{
    d->state = STATE_DISABLED;
}

bool Burger::isEnabled() const
{
    return d->state == STATE_ENABLED;
}
