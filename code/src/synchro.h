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
    Synchro() {}

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {
        mutex.acquire();
        //Then look if the critical section is free -> go or wait.
        if (CSFree && loco.priority == 0) {
            loco.afficherMessage(qPrintable(QString("Je suis prioritaire (priority = %1) pour la SC").arg(loco.priority)));

            CSFree = false;
            mutex.release();
        } else {
            loco.afficherMessage(qPrintable(QString("Je ne suis PAS prioritaire (priority = %1) donc j'attends !").arg(loco.priority)));
            mutex.release();
            loco.arreter();
            CSAccess.acquire();
            loco.demarrer();
        }
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
        mutex.acquire();
        CSFree = true;
        mutex.release();
        //Si nous sommes la locomotive prioritaire, nous pouvons relacher l'accès pour l'autre
        if (loco.priority == 0)
            CSAccess.release();

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
        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));

        loco.arreter();
        mutex2.acquire();
        if (nbLocoWaiting < 1) {
            nbLocoWaiting++;
            loco.priority = totalNbLocos - nbLocoWaiting;
            mutex2.release();
            stationWaitMutex.acquire();
        } else {
            nbLocoWaiting++;
            loco.priority = totalNbLocos - nbLocoWaiting;
            mutex2.release();
            stationWaitMutex.release();
        }
        loco.afficherMessage("Début de l'attente en gare");
        loco.arreter();
        PcoThread::usleep(2e6);//TODO: get back to 5e6
        mutex2.acquire();
        nbLocoWaiting--;
        mutex2.release();
        loco.demarrer();
    }

    /* A vous d'ajouter ce qu'il vous faut */

private:
    // Méthodes privées ...
    // Attribut privés ...
    //Permet la gestion de l'entrée et sortie en section critique
    bool CSFree = true;//whether the critical section is free
    PcoSemaphore CSAccess{0};
    PcoSemaphore mutex{1};

    //Permet l'attente de l'autre locomotive en gare
    PcoSemaphore stationWaitMutex{0};

    //Permet la gestion des priorités
    PcoSemaphore mutex2{1};
    int nbLocoWaiting = 0;
    const int totalNbLocos = 2;
};


#endif// SYNCHRO_H
