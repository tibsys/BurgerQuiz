#ifndef DEBUGHELPER_HPP
#define DEBUGHELPER_HPP

#include <QString>
#include <QByteArray>
#include "Burger.h"

/**
 * @brief Cette classe contenient des fonctions statiques qui facilitent le débogage de l'application
 */
class DebugHelper {
public:
    /**
     * @brief Formate une chaine d'octets en une représentation hexadécimale et retourne cette chaine
     */
    static QString byteArrayToString(QByteArray data) {
        return data.toHex().toUpper();
    }

    /**
     * @brief Traduit une équipe (\see Burger::Team) en chaine de caractères
     */
    static QString teamToString(Burger::Team team) {
        switch(team) {
        case Burger::INCONNUE: return "Inconnue";
        case Burger::MAYO: return "Mayo";
        case Burger::KETCHUP: return "Ketchup";
        }

        return "Inconnue";
    }
};

#endif // DEBUGHELPER_HPP
