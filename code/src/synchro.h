/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */


#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>

#include "ctrain_handler.h"
#include "locomotive.h"
#include "synchrointerface.h"


/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface {
public:
    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    Synchro() {
        // TODO
        diriger_aiguillage(16, DEVIE, 0);
    }

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {
        // TODO
        mutex.acquire();
        if (CSFree) {
            CSFree = false;
            mutex.release();
            CSAccess.acquire();
        } else {
            mutex.release();
            loco.arreter();
            CSAccess.acquire();
            loco.demarrer();
        }
        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive &loco) override {
        // TODO
        mutex.acquire();
        CSAccess.release();
        CSFree = true;
        mutex.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive &loco) override {
        // TODO
        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));

        loco.arreter();
        mutex2.acquire();
        if (nbLocoAtStation < 1) {
            nbLocoAtStation++;
            mutex2.release();
            stationWaitMutex.acquire();
        } else {
            nbLocoAtStation--;
            mutex2.release();
            stationWaitMutex.release();
        }
        PcoThread::usleep(2e6);//TODO: get back to 5e6
        loco.demarrer();
    }

    /* A vous d'ajouter ce qu'il vous faut */

private:
    // Méthodes privées ...
    // Attribut privés ...
    bool CSFree;//whether the critical section is free
    PcoSemaphore CSAccess{1};
    PcoSemaphore mutex{1};

    int nbLocoAtStation = 0;
    PcoSemaphore mutex2{1};
    PcoSemaphore stationWaitMutex{0};
};


#endif// SYNCHRO_H
