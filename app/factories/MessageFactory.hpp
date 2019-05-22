#ifndef MESSAGEFACTORY_HPP
#define MESSAGEFACTORY_HPP

#include <QByteArray>
#include <QDebug>
#include "Burger.h"
#include "DebugHelper.hpp"

class MessageFactory
{
public:
    /**
     * @brief Header has 1 byte : (hex) 01
     * @return
     */
    static QByteArray messageHeader()
    {
        return QByteArray(1, 1);
    }

    /**
     * @brief Appends bytes needed to terminate the message
     * @param msg
     * @return
     */
    static QByteArray messageTermination() {
        return QByteArray(1, 4);
    }

    enum Message {
        MESSAGE_UNKNOWN = 0,
        PING = 0x16,
        PONG = 0x06,
        LED = 0xED,
        BUTTON = 0xB0
    };

    static QByteArray getCommandId(Message msg) {
        //TODO: Ici il faudrait une gestion d'erreur
        return QByteArray(1, static_cast<char>(msg));
    }

    /**
     * @brief Ping message is a 4 bytes message : (hex) 01 16 00 04
     * @return
     */
    static QByteArray makePingMessage()
    {
        QByteArray  msg;
        msg.append(messageHeader()); //Ajout de l'en-tête des messages
        msg.append(getCommandId(PING));
        msg.append(static_cast<char>(0x0)); //Taille = 0 (pas de payload)
        msg.append(messageTermination()); //Ajout de la terminaison des messages

        return msg;
    }

    /**
     * @brief Pong message is a 4 bytes message : (hex) 01 06 00 04
     * @return
     */
    static QByteArray makePongMessage()
    {
        QByteArray  msg;
        msg.append(messageHeader()); //Ajout de l'en-tête des messages
        msg.append(getCommandId(PONG));
        msg.append(static_cast<char>(0x0)); //Taille = 0 (pas de payload)
        msg.append(messageTermination()); //Ajout de la terminaison des messages

        return msg;
    }

    /**
     * @brief Led message
     * @return
     */
    static QByteArray makeLedMessage(Burger::LedState state)
    {
        QByteArray  msg;
        msg.append(messageHeader()); //Ajout de l'en-tête des messages
        msg.append(getCommandId(LED));
        msg.append(static_cast<char>(0x1)); //Taille = 1
        msg.append(static_cast<char>(state));
        msg.append(messageTermination()); //Ajout de la terminaison des messages

        return msg;
    }

    /**
     * @brief Button message
     * @return
     */
    static QByteArray makeButtonPressedMessage()
    {
        QByteArray  msg;
        msg.append(messageHeader()); //Ajout de l'en-tête des messages
        msg.append(getCommandId(BUTTON));
        msg.append(static_cast<char>(0x0)); //Taille = 0
        msg.append(messageTermination()); //Ajout de la terminaison des messages

        return msg;
    }

    static bool isMessageValid(QByteArray& data)
    {
        if(data.size() < 4) {
            qWarning() << "Message " << DebugHelper::byteArrayToString(data) << " is not valid: length < 4";
            return false;
        }

        if(!data.startsWith(messageHeader())) {
            qWarning() << "Message " << DebugHelper::byteArrayToString(data) << " is not valid: invalid header";
            return false;
        }

        if(!data.endsWith(messageTermination())) {
            qWarning() << "Message " << DebugHelper::byteArrayToString(data) << " is not valid: invalid termination";
            return false;
        }

        if( (data.at(1) != PING) && (data.at(1) != PONG) && (static_cast<quint8>(data.at(1)) != LED) && (static_cast<quint8>(data.at(1)) != BUTTON)) {
            qWarning() << "Message " << DebugHelper::byteArrayToString(data) << " is not valid: " << QString::number(data.at(1), 16) << " is not a valid command";
            return false;
        }

        //TODO: finir la validation

        return true;
    }

    static Message messageTypeFromData(QByteArray& data)
    {
        if(data.size() > 1) {
            switch(static_cast<quint8>(data[1])) {
                case PING: return PING;
            case PONG: return PONG;
            case LED: return LED;
            case BUTTON: return BUTTON;
            }
        } else {
            return MESSAGE_UNKNOWN;
        }

        return MESSAGE_UNKNOWN;
    }
};

#endif // MESSAGEFACTORY_HPP
