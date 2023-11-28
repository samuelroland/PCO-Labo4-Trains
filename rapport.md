# Labo 4 - Gestion de ressources - PCO

Auteur·ices: Vitória Oliveira et Samuel Roland

## Description des fonctionnalités du logiciel

Ce laboratoire a pour objectif de simuler la gestion de ressources partagées par plusieurs locomotives. Ces ressources comprennent un tronçon commun qui ne peut être emprunté par qu'une locomotive à la fois et une gare où elles doivent s'attendre à chaque tour puis repartir après 5 secondes (temps pour que les passagers changent de train).

La priorité d'entrée dans le troncon commun est accordée à la locomotive qui arrive en dernier à la gare.

## Choix d'implémentation
Voici le parcours choisi, nous avons également spécifié les contacts d'accès et d'exit, qui sont les contacts utilisés pour détecter qu'on veut y entrer ou qu'on est est sorti, afin d'appeler `access()` (pour demander l'accès) et `leave()` (pour indiquer la sortie) sur `sharedSection`.
![parcours-choisi.png](imgs/parcours-choisi.png)

<!-- Comment avez-vous abordé le problème, quels choix avez-vous fait, quelle 
décomposition avez-vous choisie, quelles variables ont dû être protégées, ... -->

*Note: de manière général nous avons pensé à rendre certaines parties évolutives, c'est à dire sans avoir besoin de modifier ou moins de modifications si on souhaitait avoir plus de 2 locomotives. Dans ce cas, il faudrait évidemment avoir plus que 2 voies ou plusieurs sections critiques et potentiellement d'autres règles concernant la gare. Il n'est donc pas possible de tout factoriser pour que cela soit facile d'intégrer la suite mais nous avons essayé de penser à N locomotives plutôt que juste 2.*

#### Gestion de l'attente en gare
L'attente entre les locomotives est gérée par le mutex `stationWaitMutex` initialisé à 0 (car on veut bloquer dès la première locomotive) dans la méthode `sharedSection::stopAtStation()`. Un attribut `nbLocoWaiting` compte le nombre de locomotives en attente à la station.

Si la locomotive qui arrive n'est pas la dernière, elle incrémente `nbLocoWaiting` et acquiert le mutex `stationWaitMutex` pour bloquer dans l'attente des locomotives suivantes.
Si la locomotive est la dernière à arriver en gare, on incrémente également `nbLocoWaiting` (pour que le calcul de priorité soit correct), puis on relâche `stationWaitMutex` pour chaque autre locomotive, grâce à un compteur `TOTAL_NB_LOCOS` constant défini à 2 pour notre cas.

Une fois ce mutex libéré, les locomotives attendent chacune 5 secondes puis rédémarrent.

Tous les accès en lecture ou écriture à `nbLocoWaiting` sont protégés par un mutex dédié. TODO fix

### Gestion de l'accès au tronçon commun
L'accès au tronçon commun est accordé à la seul locomotive qui a la priorité (`loco.priority == 0`) et si l'accès est libre. Les autres locomotives doivent attendre, cela est géré à l'aide TODO

**La gestion de la priorité** est réalisée à l'aide de l'attribut `Locomotive::priority`. Chaque locomotive a une priorité attribuée, déterminée par l'ordre dans lequel elles entrent en attente à la gare. La priorité 0 est la plus prioritaire (dernière locomotive arrivée). L'avant dernière locomotive (la première en gare dans notre cas comme on en a 2) aura une priorité de 1.

Lorsqu'une locomotive arrive à la station, elle s'arrête et met à jour sa priorité en fonction du "nombre total de locomotives - nombre de locomotives déjà en attente", garantissant une priorité de valeur plus petite aux locomotives arrivées en dernier.
Cette instruction est protégée par le mutex `mutex2` afin d'éviter des incohérences dans la lecture de `nbLocoWaiting`. 

(TODO ici ou dans section partagée?)
Lorsqu'une locomotive tente d'accéder à la section critique, elle vérifie si elle a la plus haute priorité (priorité égale à 0).
Si c'est le cas, elle accède directement à la section critique.
Sinon, elle s'arrête, libère l'accès à la section critique et attend que la locomotive prioritaire quitte la section.




## Tests effectués


<!-- Description de chaque test, et information sur le fait qu'il ait passé ou non -->

Random notes
Important qu'une locomotive gère l'aiguillage pour elle même et pas l'autre.