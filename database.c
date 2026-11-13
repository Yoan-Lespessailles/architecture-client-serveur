#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/*---- Charger les documents du dossier dans un tableau dynamique ----*/
Document *chargerDocuments(const char *cheminDossier, int *nbFichiers) {

    Document *data = NULL;
    *nbFichiers = 0;

    DIR *dossier = opendir(cheminDossier);
    struct dirent *entree;

    if (dossier == NULL) {
        perror("Erreur ouverture dossier");
        return NULL;
    }

    /*---- Parcourir les fichiers du dossier ----*/
    while ((entree = readdir(dossier)) != NULL) {

        /* Vérifier l'extension .txt */
        char *extension = strrchr(entree->d_name, '.');

        if (extension == NULL || strcmp(extension, ".txt") != 0)
            continue;

        /* Construire le chemin complet */
        char chemin[512];
        int longueur = snprintf(chemin, sizeof(chemin), "%s/%s", cheminDossier, entree->d_name);

        if (longueur < 0 || (size_t)longueur >= sizeof(chemin))
            continue;

        /* Ouvrir le fichier */
        FILE *fichier = fopen(chemin, "rb");

        if (fichier == NULL)
            continue;

        /* Récupérer la taille du fichier */
        if (fseek(fichier, 0, SEEK_END) != 0) {
            fclose(fichier);
            continue;
        }

        long taille = ftell(fichier);

        if (taille < 0) {
            fclose(fichier);
            continue;
        }

        rewind(fichier);

        /* Allouer la mémoire pour le contenu */
        char *contenu = malloc((size_t)taille + 1);

        if (contenu == NULL) {
            fclose(fichier);
            break;
        }

        /* Lire le fichier */
        size_t lus = fread(contenu, 1, (size_t)taille, fichier);
        contenu[lus] = '\0';

        fclose(fichier);

        /* Allouer la mémoire pour le nom du fichier */
        char *nom = malloc(strlen(entree->d_name) + 1);

        if (nom == NULL) {
            free(contenu);
            break;
        }

        strcpy(nom, entree->d_name);

        /* Agrandir le tableau de structures */
        Document *temp = realloc(data, (*nbFichiers + 1) * sizeof(Document));

        if (temp == NULL) {
            free(nom);
            free(contenu);
            break;
        }

        data = temp;

        /* Ajouter le document dans le tableau */
        data[*nbFichiers].nom = nom;
        data[*nbFichiers].contenu = contenu;

        (*nbFichiers)++;
    }

    closedir(dossier);

    return data;
}

/*---- Libérer la mémoire utilisée par les documents ----*/
void libererDocuments(Document *data, int nbFichiers) {

    for (int i = 0; i < nbFichiers; i++) {
        free(data[i].nom);
        free(data[i].contenu);
    }

    free(data);
}
