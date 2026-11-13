#ifndef FICHIER_H
#define FICHIER_H

#include <stddef.h>

/* Retourne le contenu alloué, ou NULL en cas d'erreur. Le programme appelant devra libérer ce contenu avec free(). */
char *lireFichier(const char *chemin, size_t *taille);

#endif