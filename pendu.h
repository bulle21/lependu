#ifndef _LEPENDU
#define _LEPENDU

#define FICHIER_DES_SCORES "score.hi"
#define MAX_WORD_LENGTH 100
#define MAX_WRONG_LETTERS 10
#define DICO "dico.txt"
#define INITIAL_CAPACITY 1000

//#define DEBUG	1

#define BEEP	1

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>

// les globales
extern int max_erreurs;
extern int word_count;
extern int record,score;;
extern char *mot_a_chercher;
extern char **dictionary;

// Enum pour représenter les étapes du dessin du pendu
typedef enum {
    POTENCE,
    TETE,
    CORPS,
    BRAS_GAUCHE,
    BRAS_DROIT,
    JAMBE_GAUCHE,
    JAMBE_DROITE,
    COMPLET
} PenduEtat;

//prototypages
char 	**read_dictionary(const char *,int *);
char 	*select_random_word(char **,int);
void 	free_dictionary(char **,int);
void 	dessiner_pendu(WINDOW *,PenduEtat);
int 	deja_trouve(int,char *,int);
int 	calcul_max_tentatives(const char *);
void 	finish(int);
void 	lectureScoreDansFichier(void);
void 	ecrireScoreDansFichier(void);

#endif
