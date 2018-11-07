import QtQuick 2.9
import QtQuick.Window 2.2
import BurgerQuiz 1.0

Window {
    visible: true
    width: 640
    height: 480
    color: "#000000"
    title: qsTr("Burger Quiz")

    ViewGame {
        id: vueJeu
        anchors.fill: parent
        visible: true
    }

    Component.onCompleted: {
        console.log("L'application est démarrée, lancement du générique")
        //vueJeu.mediaplayer.play()
    }
}
