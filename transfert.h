#ifndef TRANSFERT_H
#define TRANSFERT_H

#include <stddef.h>

#define TAILLE_MAX_DOCUMENT (64u * 1024u)

/* Retourne 0 si tous les octets ont été transférés, -1 sinon. */
int envoyerTout(int socket, const void *donnees, size_t taille);
int recevoirTout(int socket, void *donnees, size_t taille);

#endif