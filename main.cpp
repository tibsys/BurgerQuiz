#include <QApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QFontDatabase>
#include "GameController.h"
#include "Constants.h"
#include "FormLog.h"

/**
 * @brief Cette fonction est appelée par le Message Handler de Qt (QtMessageHandler) qui centralise et redirige tous les messages
 * de log créés par les fonctions qDebug(), qWarning(), etc
 *
 * Nous utilisons ce mécanisme, conjointement à la classe FormLog pour afficher les logs dans une fenêtre, de manière simple d'un
 * point de architecture tout en s'appuyant le plus possible sur les mécanismes de Qt.
 *
 * @param msg Le message à afficher
 */
void logManager(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    //Si la fenêtre de log est instanciée, nous lui renvoyons directement tous les messages
    //sans filtrage
    if(FormLog::instance() != nullptr) {
        FormLog::instance()->appendLog(msg);
    }
}

/**
 * @brief Point d'entrée de l'application. Démarre l'application, affiche la fenêtre de log et démarre le moteur QML qui gère la
 * GUI de notre application
 *
 * Cette application n'attend aucun argument, ni ne renvoie de code d'erreur.
 */
int main(int argc, char *argv[])
{
    qInstallMessageHandler(logManager);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);    

    /*
     * Définit l'interface entre le monde Qt C++ et le monde Qt QML.
     * En, d'autres termes, nous exposons ici des objets C++ qui seront utilisés en QML
     * et pourront être sérialisés dans un QVariant, utilisés dans des signaux, etc.
     */
    qmlRegisterType<GameController>("BurgerQuiz", 1, 0, "GameController");
    qmlRegisterUncreatableType<Burger>("BurgerQuiz", 1, 0, "Burger", "Cet objet ne peut être instancié depuis QML");
    qRegisterMetaType<Burger>("Burger");
    qRegisterMetaType<Burger>("Burger&");

    //Instanciation de l'application principale, offrant une run-loop à notre application
    QApplication app(argc, argv);

    //Instanciation et affichage de la fenêtre d'affichage des logs
    FormLog *formLog = new FormLog;
    formLog->show();

    //Ajout de la fonte utilisée dans la GUI
    QFontDatabase::addApplicationFont ( ":/fonts/horseshoeslemonade.ttf" );

    //Instanciation et initialisation du moteur QML
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    //Démarrage de l'application
    return app.exec();
}
