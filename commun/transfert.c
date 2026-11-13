#include <errno.h>
#include <sys/socket.h>

#include "transfert.h"

int envoyerTout(int socket, const void *donnees, size_t taille)
{
    const unsigned char *octets = donnees;
    size_t total = 0;

    while (total < taille) {
        ssize_t envoyes = send(socket, octets + total, taille - total, MSG_NOSIGNAL);

        if (envoyes < 0 && errno == EINTR) {
            continue;
        }
        if (envoyes <= 0) {
            return -1;
        }

        total += (size_t)envoyes;
    }

    return 0;
}

int recevoirTout(int socket, void *donnees, size_t taille)
{
    unsigned char *octets = donnees;
    size_t total = 0;

    while (total < taille) {
        ssize_t recus = recv(socket, octets + total, taille - total, 0);

        if (recus < 0 && errno == EINTR) {
            continue;
        }
        if (recus <= 0) {
            return -1;
        }

        total += (size_t)recus;
    }

    return 0;
}