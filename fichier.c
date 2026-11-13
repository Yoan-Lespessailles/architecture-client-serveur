#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>

char *lireFichier(const char *chemin, size_t *taille) {

    *taille = 0;

    /*---- Ouvrir le fichier ----*/
    FILE *fichier = fopen(chemin, "rb");

    if (fichier == NULL) {
        perror("Ouverture du fichier");
        return NULL;
    }

    /*---- Déterminer sa taille ----*/
    if (fseek(fichier, 0, SEEK_END) != 0) {
        perror("Déplacement dans le fichier");
        fclose(fichier);
        return NULL;
    }

    long longueur = ftell(fichier);

    if (longueur < 0) {
        perror("Lecture de la taille");
        fclose(fichier);
        return NULL;
    }

    if (fseek(fichier, 0, SEEK_SET) != 0) {
        perror("Retour au début du fichier");
        fclose(fichier);
        return NULL;
    }

    /*---- Réserver la mémoire : contenu + fin de chaîne ----*/
    char *contenu = malloc((size_t)longueur + 1);

    if (contenu == NULL) {
        perror("Allocation du contenu");
        fclose(fichier);
        return NULL;
    }

    /*---- Lire le contenu ----*/
    size_t lus = fread(contenu, 1, (size_t)longueur, fichier);

    if (lus != (size_t)longueur || ferror(fichier)) {
        fprintf(stderr, "Erreur : lecture incomplète du fichier\n");
        free(contenu);
        fclose(fichier);
        return NULL;
    }

    contenu[lus] = '\0';
    *taille = lus;

    fclose(fichier);

    return contenu;
}