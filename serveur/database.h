#ifndef DATABASE_H
#define DATABASE_H

/*---- Structure représentant un document ----*/
typedef struct {
    char *nom;
    char *contenu;
} Document;

/*---- Fonctions de gestion des documents ----*/
Document *chargerDocuments(const char *dossier, int *nbFichiers);
void libererDocuments(Document *data, int nbFichiers);

#endif