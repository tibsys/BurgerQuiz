#ifndef FORMLOG_H
#define FORMLOG_H

#include <QWidget>
#include <QString>

namespace Ui {
class FormLog;
}

/**
 * @brief Cette classe affiche et contrôle une fenêtre d'affichage pour les événements de débogage.
 * Toutes les chaines envoyées à la classe QDebug sont captées par ce contrôleur pour être affichées dans
 * un widget du type QTextEdit.
 *
 * Les entrées de la log ne sont pas directement envoyées au QTextEdit mais stockées dans un buffer local (QStringList)
 * et à intervalles réguliers, cette liste est dépilée pour afficher les nouvelles entrées.
 *
 * Cette classe est en apparence un Singleton, pourtant elle n'en est pas un. Elle possède une variable membre statique et une fonction
 * publique statique qui retourne une instance de la classe, mais son constructeur est public et il n'y a pas de code d'instanciation
 * dans cette classe.
 * Elle ne fait que rendre accessible son instance par la fonction instance(), la variable instance_ ayant été initialisée dans le constructeur.
 *
 * Cette tactique est nécessaire pour deux raisons :
 * 1 - Il n'est pas possible d'instancier une fenêtre sans avoir instancié au préalable une QApplication, cela exclu dont le Singleton. De plus c'est
 * une mauvaise pratique d'instancier des classes de GUI en Singleton.
 * 2 - Nous avons besoin de l'accès à l'instance de cette fenêtre afin d'y rediriger les logs depuis le fichier main.cpp dans la fonction logManager().
 */
class FormLog : public QWidget
{
    Q_OBJECT

public:
    explicit FormLog(QWidget *parent = nullptr);
    ~FormLog();
    static FormLog* instance() { return instance_; }

public slots:
    /**
     * @brief ajoute de la log au buffer de la fenêtre
     */
    void appendLog(const QString&);

private slots:
    /**
     * @brief fonction appelée à intervalles raguliers pour dépiler le buffer et afficher les nouvelles entrées de la log.
     */
    void refreshLogs();

private:
    Ui::FormLog *ui;
    static FormLog* instance_;

    /**
     * @brief Buffer local recevant les entrées de la log qui doivent être affichées.
     */
    QStringList logs_;
};

#endif // FORMLOG_H
