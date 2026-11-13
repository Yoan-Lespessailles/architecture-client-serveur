
/******************** CLIENT CODE ********************/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "fichier.h"
#include "transfert.h"

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Lire le document à envoyer ----*/
    size_t taille = 0;
    char *contenu = lireFichier("document_test.txt", &taille);

    if (contenu == NULL)
    {
        return 1;
    }

    if (taille > TAILLE_MAX_DOCUMENT)
    {
        fprintf(stderr, "Le document dépasse la taille maximale.\n");
        free(contenu);
        return 1;
    }

    /*---- Vérifier la lecture avant de développer l'envoi ----*/
    printf("Taille du document : %zu octets\n", taille);
    printf("Contenu :\n%s\n", contenu);

    /*---- Créer la socket. Les trois arguments sont : ----*/
    /* 1) Domaine Internet 2) Socket de type flux 3) Protocole par défaut (TCP ici) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configurer les paramètres de la structure d’adresse du serveur ----*/
    /* Famille d’adresses = Internet */
    serverAddr.sin_family = AF_INET;
    /* Définir le numéro de port en utilisant htons pour respecter l’ordre des octets */
    serverAddr.sin_port = htons(7891);
    /* Définir l’adresse IP sur localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Mettre à 0 tous les bits du champ de remplissage */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connecter la socket au serveur en utilisant la structure d’adresse ----*/
    addr_size = sizeof serverAddr;
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size) == -1)
    {
        perror("connect");
        close(clientSocket);
        free(contenu);
        return 1;
    }

    /*---- Envoyer la taille, puis le contenu ----*/
    uint32_t tailleReseau = htonl((uint32_t)taille);
    char reponse[2];
    int resultat = 1;

    if (envoyerTout(clientSocket, &tailleReseau, sizeof(tailleReseau)) == -1 ||
    envoyerTout(clientSocket, contenu, taille) == -1)
    {
        fprintf(stderr, "Impossible d'envoyer le document complet.\n");
    }
    else if (recevoirTout(clientSocket, reponse, sizeof(reponse)) == -1 || memcmp(reponse, "OK", sizeof(reponse)) != 0)
    {
        fprintf(stderr, "Le serveur n'a pas confirmé la réception.\n");
    }
    else
    {
        printf("Le serveur a reçu le document (%zu octets).\n", taille);
        resultat = 0;
    }

    /*---- Libérer les ressources ----*/
    close(clientSocket);
    free(contenu);

    return resultat;

}