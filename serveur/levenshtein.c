#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "levenshtein.h"

static void enMinuscules(char *str){
    for (size_t i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

static void enleverEspacesSuperflus(char *str){
    size_t j = 0;
    int dansEspace = 1;

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (isspace((unsigned char)str[i])) {
            if (!dansEspace) {
                str[j++] = ' ';
                dansEspace = 1;
            }
        } else {
            str[j++] = str[i];
            dansEspace = 0;
        }
    }

    if (j > 0 && str[j - 1] == ' ') {
        j--;
    }
    str[j] = '\0';
}

static char *copierEtFormater(const char *str){
    size_t taille = strlen(str) + 1;  /* Inclut le '\0'. */
    char *copie = malloc(taille);

    if (copie != NULL) {
        memcpy(copie, str, taille);
        enMinuscules(copie);
        enleverEspacesSuperflus(copie);
    }

    return copie;
}

static size_t minimum(size_t a, size_t b){
    return a < b ? a : b;
}

size_t calculDistance(const char *texte, const char *data){
    /* Créer des copies pour mettre les textes en forme sans toucher aux originaux. */
    char *texteFormate = copierEtFormater(texte);
    char *dataFormate = copierEtFormater(data);

    /* On garde seulement deux lignes du tableau des distances. */
    size_t *lignePrecedente = NULL;
    size_t *ligneEnCours = NULL;

    /* Cette valeur reste en place si on doit quitter la fonction sur une erreur. */
    size_t distance = SIZE_MAX;

    if (texteFormate == NULL || dataFormate == NULL) {
        goto nettoyage;
    }

    /* Mesurer les textes après la mise en forme. strlen() compte les octets. */
    size_t longueurTexte = strlen(texteFormate);
    size_t longueurData = strlen(dataFormate);

    /* Vérifier que la taille d'une ligne, en octets, tient dans un size_t. */
    if (longueurData >= SIZE_MAX / sizeof *lignePrecedente) {
        goto nettoyage;
    }

    /* Chaque ligne a longueurData + 1 cases. La case 0 représente le second texte vide. */
    lignePrecedente = malloc((longueurData + 1) * sizeof *lignePrecedente);
    ligneEnCours = malloc((longueurData + 1) * sizeof *ligneEnCours);

    if (lignePrecedente == NULL || ligneEnCours == NULL) {
        goto nettoyage;
    }

    /* Remplir la ligne 0. Pour passer du texte vide aux j premiers octets du second texte, il faut j insertions. */
    for (size_t j = 0; j <= longueurData; j++) {
        lignePrecedente[j] = j;
    }

    /* À chaque tour, on prend un octet de plus dans le premier texte. */
    for (size_t i = 1; i <= longueurTexte; i++) {
        /* Colonne 0. Pour passer de i octets au texte vide, il faut i suppressions. */
        ligneEnCours[0] = i;

        /* Remplir la ligne de gauche à droite. */
        for (size_t j = 1; j <= longueurData; j++) {
            /* Case du dessus (i - 1, j). On reprend le coût calculé sans le dernier octet du premier texte et on ajoute 1 pour supprimer cet octet. */
            size_t suppression = lignePrecedente[j] + 1;

            /* Case de gauche (i, j - 1). On reprend le coût calculé sans le dernier octet du second texte et on ajoute 1 pour insérer cet octet. */
            size_t insertion = ligneEnCours[j - 1] + 1;

            /* Le dernier octet d'un début de longueur i est à l'indice i - 1. La comparaison != donne 0 si les octets sont égaux, 1 sinon. */
            size_t cout = (texteFormate[i - 1] != dataFormate[j - 1]);

            /* Case en diagonale (i - 1, j - 1). */
            size_t remplacement = lignePrecedente[j - 1] + cout;

            /* Enregistrer le plus petit des trois coûts dans la case (i, j). */
            ligneEnCours[j] = minimum(minimum(suppression, insertion), remplacement);
        }

        /* Échanger les pointeurs. La ligne qu'on vient de remplir devient la ligne précédente. L'ancien tableau servira à écrire la suivante. */
        size_t *ancienne = lignePrecedente;
        lignePrecedente = ligneEnCours;
        ligneEnCours = ancienne;
    }
    /* La dernière ligne calculée est dans lignePrecedente. */
    distance = lignePrecedente[longueurData];

nettoyage:
    free(lignePrecedente);
    free(ligneEnCours);
    free(texteFormate);
    free(dataFormate);
    return distance;
}

/*int main(void){
    const char *texte = "Bob marche dans le couloir";
    const char *data = "Bob est en train de marcher dans le couloir";
    size_t distance = calculDistance(texte, data);

    if (distance == SIZE_MAX) {
        fprintf(stderr, "Impossible de calculer la distance.\n");
        return 1;
    }

    size_t logueurTexte = strlen(texte);
    size_t logueurData = strlen(data);
    size_t longueurMax = (logueurTexte > logueurData) ? logueurTexte : logueurData;

    double difference = 0.0;
    if (longueurMax > 0) {
        difference = 100.0 * distance / longueurMax;
    }

    double similarite = 100.0 - difference;

    printf("Difference : %.2f %%\n", difference);
    printf("Similarite : %.2f %%\n", similarite);
    return 0;
}*/