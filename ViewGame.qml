import QtQuick 2.0
import BurgerQuiz 1.0
import QtMultimedia 5.9

Item {
    property alias masqueKetchup: masqueKetchup
    property alias masqueMayo: masqueMayo
    property alias lblInitalisation: lblInitialisation

    id: vueJeu
    focus: true

    GameController {
        id: gameController

        onBurgerInitialized: {
            if(team === Burger.MAYO) {
                console.warn("Affichage du burger pour l'équipe Mayo")
                //vueJeu.masqueMayo.visible = false
                state = "StateMayoActivated"
            } else if (team === Burger.KETCHUP) {
                console.warn("Affichage du burger pour l'équipe Ketchup")
                //vueJeu.masqueKetchup.visible = false
                state = "StateKetchupActivated"
            }
        }

        onGameReady: {
            state = "StateGameStarted"
            gameController.gameState = GameController.INITIALIZED

            console.warn("")
            console.warn("Le jeu est prêt !")
            console.warn("> Appuyez sur la touche N pour lancer une nouvelle question.")
            console.warn("> Appuyez sur la touche Q pour quitter le jeu.")
        }

        onBurgerSelected: {
            state = (team === Burger.KETCHUP ? "StateKetchupSelected" : "StateMayoSelected")
        }

    }

    Item {
        id: subGenerique
        visible: false
        anchors.fill: parent

        MediaPlayer {
            id: mediaplayer
            source: "/videos/generique.mp4"
            autoLoad: true
            autoPlay: true

            onError: {
                console.warn("Mediaplayer error: " +errorString);
            }

            onStopped: {
                console.warn("Fin du générique. Démarrage du jeu.")
                state = "StateInitializing"
                gameController.initialize()
            }
        }

        VideoOutput {
            anchors.fill: parent
            source: mediaplayer
        }

    }

    Image {
        id: background
        visible: false
        anchors.fill: parent
        source: "/images/ketchupmayo.png"
    }

    Text {
        id: lblInitialisation
        height: 41
        color: "#3478f2"
        text: "En attente des burgers..."
        visible: false
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        font.pointSize: 60
        verticalAlignment: Text.AlignVCenter
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideNone
        font.family: "Horseshoes and Lemonade"
    }

    Item {
        id: masques
        //visible: false
        anchors.bottomMargin: 100
        anchors.fill: parent

        Rectangle {
            id: masqueKetchup
            color: "#000000"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: parent.width/2
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            z: 99
        }

        Rectangle {
            id: masqueMayo
            color: "#000000"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: parent.width/2
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            z: 99
        }
    }

    Keys.onPressed: {
        console.warn("Etat du jeu : " +gameController.gameState)

        if (event.key === Qt.Key_N) {
            if(gameController.gameState !== GameController.INITIALIZING) {
                console.warn("Nouvelle partie")
                state = "StateSelection"
                //gameController.gameState = GameController.SELECTION
                gameController.setReady()
                event.accepted = true
            }
        } else if(event.key === Qt.Key_Q) {
            console.warn("Fermeture de l'application")
            Qt.quit()
        }
    }

    Component.onCompleted: {
        state = "StateOpeningCredits"
    }

    states: [
        State {
            name: "StateOpeningCredits"

            PropertyChanges {
                target: lblInitialisation
                visible: false
            }

            PropertyChanges {
                target: masqueMayo
                visible: false
            }

            PropertyChanges {
                target: masqueKetchup
                visible: false
            }

            PropertyChanges {
                target: subGenerique
                visible: true
            }

            PropertyChanges {
                target: background
                visible: false
            }
        },
        State {
            name: "StateInitializing"

            PropertyChanges {
                target: lblInitalisation
                visible: true
            }

            PropertyChanges {
                target: masqueKetchup
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: true
            }

            PropertyChanges {
                target: background
                visible: true
            }
        },
        State {
            name: "StateMayoActivated"

            PropertyChanges {
                target: lblInitalisation
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }
        },
        State {
            name: "StateKetchupActivated"

            PropertyChanges {
                target: lblInitalisation
                visible: true
            }

            PropertyChanges {
                target: masqueKetchup
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }
        },
        State {
            name: "StateGameStarted"

            PropertyChanges {
                target: lblInitialisation
                text: "Le jeu commence !"
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: false
            }

            PropertyChanges {
                target: masqueKetchup
                visible: false
            }

            PropertyChanges {
                target: subGenerique
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }
        },
        State {
            name: "StateSelection"
            PropertyChanges {
                target: lblInitialisation
                text: "A vos burgers..."
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: false
            }

            PropertyChanges {
                target: masqueKetchup
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }

            PropertyChanges {
                target: subGenerique
                visible: false
            }
        },
        State {
            name: "StateKetchupSelected"
            PropertyChanges {
                target: lblInitialisation
                color: "#bd2709"
                text: "Ketchup a la parole"
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: true
            }

            PropertyChanges {
                target: masqueKetchup
                visible: false
            }

            PropertyChanges {
                target: subGenerique
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }
        },
        State {
            name: "StateMayoSelected"
            PropertyChanges {
                target: lblInitialisation
                color: "#fed736"
                text: "Mayo a la parole"
                visible: true
            }

            PropertyChanges {
                target: masqueMayo
                visible: false
            }

            PropertyChanges {
                target: masqueKetchup
                visible: true
            }

            PropertyChanges {
                target: subGenerique
                visible: false
            }

            PropertyChanges {
                target: background
                visible: true
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2;anchors_width:463}
}
 ##^##*/
