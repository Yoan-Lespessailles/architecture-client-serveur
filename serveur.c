/******************** SERVER CODE ********************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "database.h"
#include "transfert.h"

static int traiterClient(int newSocket)
{
    uint32_t tailleReseau;

    if (recevoirTout(newSocket, &tailleReseau, sizeof(tailleReseau)) == -1)
    {
        fprintf(stderr, "Impossible de recevoir la taille du document.\n");
        return 1;
    }

    uint32_t taille = ntohl(tailleReseau);

    if (taille > TAILLE_MAX_DOCUMENT)
    {
        fprintf(stderr, "Le document reçu dépasse la taille maximale.\n");
        return 1;
    }

    char *contenu = malloc((size_t)taille + 1);
    if (contenu == NULL)
    {
        perror("malloc");
        return 1;
    }

    if (recevoirTout(newSocket, contenu, taille) == -1)
    {
        fprintf(stderr, "Le document reçu est incomplet.\n");
        free(contenu);
        return 1;
    }

    contenu[taille] = '\0';
    printf("\nDocument reçu (%zu octets) :\n%s\n", (size_t)taille, contenu);

    free(contenu);

    if (envoyerTout(newSocket, "OK", 2) == -1)
    {
        fprintf(stderr, "Impossible de confirmer la réception.\n");
        return 1;
    }

    return 0;
}

int main(void)
{
    /*---- Charger les documents de la base ----*/
    int nbFichiers = 0;
    Document *data = chargerDocuments("database", &nbFichiers);

    printf("Nombre de documents : %d\n", nbFichiers);

    for (int i = 0; i < nbFichiers; i++)
    {
        printf("\n===== %s =====\n", data[i].nom);
        printf("%s\n", data[i].contenu);
    }

    /*---- Créer la socket d'écoute ----*/
    int welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (welcomeSocket == -1)
    {
        perror("socket");
        libererDocuments(data, nbFichiers);
        return 1;
    }

    int resultat = 1;

    /*---- Configurer l'adresse du serveur ----*/
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(welcomeSocket, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) == -1)
    {
        perror("bind");
        goto nettoyage;
    }

    if (listen(welcomeSocket, 5) == -1)
    {
        perror("listen");
        goto nettoyage;
    }

    printf("\nEn écoute sur 127.0.0.1:7891...\n");

    /*---- Attendre la connexion d'un client ----*/
    int newSocket = accept(welcomeSocket, NULL, NULL);
    if (newSocket == -1)
    {
        perror("accept");
        goto nettoyage;
    }

    /*---- Envoyer le message de test au client ----*/
    resultat = traiterClient(newSocket);
    close(newSocket);

nettoyage:
    close(welcomeSocket);
    libererDocuments(data, nbFichiers);
    return resultat;
}