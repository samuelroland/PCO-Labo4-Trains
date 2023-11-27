# Labo 4 - Gestion de ressources - PCO

Auteur·ices: Vitória Oliveira et Samuel Roland

## Description des fonctionnalités du logiciel

## Choix d'implémentation
Voici le parcours choisi, nous avons également spécifié les contacts d'accès et d'exit, qui sont les contacts utilisés pour détecter qu'on veut y entrer ou qu'on est est sorti, afin d'appeler `access()` et `leave()` sur `sharedSection`.
![parcours-choisi.png](imgs/parcours-choisi.png)

<!-- Comment avez-vous abordé le problème, quels choix avez-vous fait, quelle 
décomposition avez-vous choisie, quelles variables ont dû être protégées, ... -->


## Tests effectués


<!-- Description de chaque test, et information sur le fait qu'il ait passé ou non -->

Random notes
Important qu'une locomotive gère l'aiguillage pour elle même et pas l'autre.