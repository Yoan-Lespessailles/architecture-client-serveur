#include <stdio.h>
#include <string.h>

#define MAX 100

int calculDistance(const char *texte, const char *data)
{
    size_t tailleTexte = strlen(texte);
    size_t tailleData = strlen(data);

    if (tailleTexte > MAX || tailleData > MAX) {
        return -1;
    }

    int logueurTexte = (int)tailleTexte;
    int logueurData = (int)tailleData;
    int d[MAX + 1][MAX + 1];

    // Première colonne : combien de lettres supprimer pour arriver à "" ?
    for (int i = 0; i <= logueurTexte; i++) {
        d[i][0] = i;
        printf("d[%d][0] = %d\n", i, d[i][0]);
    }

    // Première ligne : combien de lettres ajouter en partant de "" ?
    for (int j = 0; j <= logueurData; j++) {
        d[0][j] = j;
        printf("d[0][%d] = %d\n", j, d[0][j]);
    }

    for (int i = 1; i <= logueurTexte; i++) {
        for (int j = 1; j <= logueurData; j++) {
            int suppression = d[i - 1][j] + 1;
            int insertion = d[i][j - 1] + 1;

            int cout = 0;
            if (texte[i - 1] != data[j - 1]) {
                cout = 1;
            }
            int remplacement = d[i - 1][j - 1] + cout;

            // Garder le plus petit des trois coûts.
            d[i][j] = suppression;
            if (insertion < d[i][j]) {
                d[i][j] = insertion;
            }
            if (remplacement < d[i][j]) {
                d[i][j] = remplacement;
            }
        }
    }

    return d[logueurTexte][logueurData];
}

int main(void){
    const char *texte = "Bob marche dans le couloir";
    const char *data = "Bob est en train de marcher dans le couloir";

    int distance = calculDistance(texte, data);
    if (distance < 0) {
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
}