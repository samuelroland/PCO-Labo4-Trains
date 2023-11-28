/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run() {
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    while (true) {
        // Attente en gare
        attendre_contact(station);
        sharedSection->stopAtStation(loco);

        // Entrée dans la SC
        attendre_contact(delimitorsCS.first);
        loco.afficherMessage("J'ai atteint la section commune");
        sharedSection->access(loco);
        loco.afficherMessage("En route dans la section commune");

        //Faire tous les changements d'aiguillages nécessaires pour passer en SC
        for (auto change: aiguillagesChanges) {
            diriger_aiguillage(change.first, change.second, 0);
        }

        // Sortie de la SC
        attendre_contact(delimitorsCS.second);
        loco.afficherMessage("Je quitte la section commune");
        sharedSection->leave(loco);
    }
}

void LocomotiveBehavior::printStartMessage() {
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage() {
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
