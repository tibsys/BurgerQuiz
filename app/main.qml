import QtQuick 2.9
import QtQuick.Window 2.2
import BurgerQuiz 1.0

Window {
    visible: true
    width: 1400
    height: 900
    color: "#000000"
    title: qsTr("Burger Quiz")

    ViewGame {
        id: vueJeu
        anchors.fill: parent
        visible: true
    }

    Component.onCompleted: {
        console.warn("L'application est démarrée, lancement du générique.")
        console.warn("Pour quitter l'application fermez toutes les fenêtes ou appuyez sur la touche Q")
    }
}
