#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>

#define MAX_WORD_LENGTH 100
#define MAX_WRONG_LETTERS 10
#define DICO "dico.txt"
#define INITIAL_CAPACITY 1000

//#define DEBUG	1

int max_erreurs;

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

char **read_dictionary(const char *filename, int *word_count) 
{
int 	i,fd, capacity;
char 	**words, *line_end, buffer[MAX_WORD_LENGTH];
ssize_t bytes_read;
off_t 	current_pos = 0;
    
if((fd = open(filename, O_RDONLY)) == -1) {
    perror("Erreur lors de l'ouverture du fichier");
    return NULL;
    }
    
// Allocation initiale du tableau de mots
words = (char **) malloc(INITIAL_CAPACITY * sizeof(char *));
capacity = INITIAL_CAPACITY;
*word_count = 0;
    
while((bytes_read = pread(fd, buffer, sizeof(buffer) - 1, current_pos)) > 0) {
    buffer[bytes_read] = '\0';
        
    // Recherche de la fin de ligne (0D0A)
    line_end = strstr(buffer, "\r\n");
    if(line_end) {
        *line_end = '\0';
            
        // Vérifier si nous avons besoin de réallouer
        if(*word_count >= capacity) {
            capacity *= 2;
            words = (char **) realloc(words, capacity * sizeof(char *));
            }
            
        // Allouer et copier le mot
        words[*word_count] = (char *) malloc(strlen(buffer) + 1);
            
        // Convertir en minuscules
        for(i = 0; buffer[i];i ++) 
            words[*word_count][i] = tolower(buffer[i]);
        words[*word_count][strlen(buffer)] = '\0';
            
        (*word_count)++;
            
        // Passer à la prochaine ligne
        current_pos += (line_end - buffer) + 2;
        } 
    else 
        break;
    }
    
close(fd);
return words;
}

char *select_random_word(char **words, int word_count) 
{
if(word_count == 0) 
    return(NULL);
    
// Initialiser le générateur de nombres aléatoires
srand(time(NULL));
    
// Sélectionner un mot aléatoire
int random_index = rand() % word_count;
return(words[random_index]);
}

void free_dictionary(char **words, int word_count) 
{
int i;

for(i = 0; i < word_count; i++) 
    free(words[i]);
free(words);
}

void dessiner_pendu(WINDOW *win, PenduEtat etape) 
{
    // Effacer la fenêtre avant de redessiner
    wclear(win);
    box(win, 0, 0);

    mvwprintw(win, 1, 2, "Pendu :");

    // Dessin de la potence (toujours affichée)
    mvwprintw(win, 2, 10, "   +---+");
    mvwprintw(win, 3, 10, "   |   |");

    // Dessin progressif selon l'état
    if (etape >= TETE) {
       	    	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "       |");
            	mvwprintw(win, 6, 10, "       |");
            	mvwprintw(win, 7, 10, "       |");
    		}
    if (etape >= CORPS) {
            	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "   |   |");
            	mvwprintw(win, 6, 10, "       |");
            	mvwprintw(win, 7, 10, "       |");
    		}
    if (etape >= BRAS_GAUCHE) {
            	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "  /|   |");
            	mvwprintw(win, 6, 10, "       |");
            	mvwprintw(win, 7, 10, "       |");
    		}
    if (etape >= BRAS_DROIT) {
            	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "  /|\\  |");
            	mvwprintw(win, 6, 10, "       |");
            	mvwprintw(win, 7, 10, "       |");
    		}
    if (etape >= JAMBE_GAUCHE) {
            	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "  /|\\  |");
            	mvwprintw(win, 6, 10, "  /    |");
            	mvwprintw(win, 7, 10, "       |");
    		}
    if (etape >= JAMBE_DROITE) {
            	mvwprintw(win, 4, 10, "   O   |");
            	mvwprintw(win, 5, 10, "  /|\\  |");
            	mvwprintw(win, 6, 10, "  / \\  |");
            	mvwprintw(win, 7, 10, "       |");
    		}

    // Ligne de base (toujours affichée)
    mvwprintw(win, 8, 10, "=========");

    wrefresh(win);
}

int deja_trouve(int ch,char *mot_affiche,int lg)
{
int i,trouve;

trouve = 0;
for(i = 0;i < lg;i ++)
	if(mot_affiche[i] == ch && mot_affiche[i] != '_')	{
		trouve = 1;
		break;
		}
return(trouve);
}

// Fonction pour calcul dynamiquement les tentatives
int calcul_max_tentatives(const char *mot) 
{
int longueur = strlen(mot);

if(longueur <= 4) 
	return(4);        // Mots très courts
else if(longueur <= 6) 
	return(5);        // Mots courts
else if(longueur <= 8) 
	return(6);        // Mots moyens
else if(longueur <= 10) 
	return(7);        // Mots longs
return(8);                // Mots très longs
}

int main(int n,char **t) 
{
int 		i,trouve,ch,word_count;
char 	**dictionary,
	mot_affiche[MAX_WORD_LENGTH],
	*mot_a_chercher,dico[32],
	lettre_deja_saisie = 0,
	lettres_incorrectes[MAX_WRONG_LETTERS] = {0}; // Tableau pour stocker les lettres incorrectes
int 	nb_lettres_incorrectes = 0,erreurs = 0,lettres_trouvees = 0;
    
if(n == 2) strcpy(dico,t[1]); else strcpy(dico,DICO); 

if((dictionary = read_dictionary(dico,&word_count)) == NULL) {
    fprintf(stderr, "impossible de lire le fichier %s\n", DICO);
    return(-1);
    }

// Initialisation ncurses avec plus de contrôles
initscr();            // Initialise l'écran
start_color();        // Active les couleurs si disponibles
cbreak();             // Désactive le buffering de ligne
noecho();             // Désactive l'affichage des touches
keypad(stdscr, TRUE); // Active les touches spéciales
curs_set(0);          // Cache le curseur
    
use_default_colors(); // utilise les couleurs de fond par defaut
    
// Initialisation des couleurs
init_pair(1,COLOR_BLUE,-1);
init_pair(2,COLOR_BLUE,-1);

// Définir le fond de l'écran principal
bkgd(COLOR_PAIR(2));
clear();
refresh();

// Choisir un mot aléatoirement
mot_a_chercher = strdup(select_random_word(dictionary,word_count));
    
// Fenêtre principale
WINDOW *win_jeu = newwin(20,50,2,2);
wbkgd(win_jeu,COLOR_PAIR(1));
box(win_jeu,0,0);
    
// Fenêtre pour le pendu
WINDOW *win_pendu = newwin(10,30,2,55);
wbkgd(win_pendu, COLOR_PAIR(2));
box(win_pendu,0,0);
    
// Fenêtre pour les lettres incorrectes
WINDOW *win_lettres_incorrectes = newwin(10,30,12,55);
wbkgd(win_lettres_incorrectes,COLOR_PAIR(2));
box(win_lettres_incorrectes,0,0);
mvwprintw(win_lettres_incorrectes,1,2,"Lettres incorrectes :");
    
// Fenêtre de fin 
WINDOW *win_fin = newwin(10,83,24,2);
wbkgd(win_fin, COLOR_PAIR(2));
box(win_fin,0,0);
    
// Initialisation du jeu
for(i = 0; i < strlen(mot_a_chercher);i++) 
    mot_affiche[i] = '_';
mot_affiche[strlen(mot_a_chercher)] = '\0';
max_erreurs = calcul_max_tentatives(mot_a_chercher);
    
// Titre du jeu
mvwprintw(win_jeu,2,15,"JEU DU PENDU");
    
while(erreurs < max_erreurs && lettres_trouvees < strlen(mot_a_chercher)) {
    // Effacer et réinitialiser les fenêtres
    wclear(win_jeu);
    box(win_jeu, 0, 0);
    mvwprintw(win_jeu,2,15,"JEU DU PENDU");
        
    // Afficher le mot
    mvwprintw(win_jeu,5,2,"Mot : ");
    for(i = 0; i < strlen(mot_a_chercher); i++) 
        mvwprintw(win_jeu,5,8 + i*2, "%c", mot_affiche[i]);
        
    // Afficher les erreurs
    mvwprintw(win_jeu,7,2, "Erreurs : %d / %d", erreurs,max_erreurs);
        
    // Dessiner le pendu selon les erreurs
    dessiner_pendu(win_pendu, (PenduEtat)((erreurs * JAMBE_DROITE) / max_erreurs));
        
    // Afficher les lettres incorrectes
    wclear(win_lettres_incorrectes);
    box(win_lettres_incorrectes,0,0);
    mvwprintw(win_lettres_incorrectes,1,2, "Lettres incorrectes :");
    for(i = 0; i < nb_lettres_incorrectes;i ++) 
        mvwprintw(win_lettres_incorrectes,2 + i/10,2 + (i%10)*2, "%c", lettres_incorrectes[i]);
        
    // Demander une lettre
    mvwprintw(win_jeu, 10, 2, "Entrez une lettre : ");
#ifdef DEBUG
    //verifications ...OH
    mvwprintw(win_jeu, 12, 2, "%s ",mot_a_chercher);
#endif
        
    // Rafraîchir toutes les fenêtres
    wrefresh(win_jeu);
    wrefresh(win_pendu);
    wrefresh(win_lettres_incorrectes);
        
    // Capturer la lettre
    ch = getch();
        
    // Vérifier si c'est une lettre valide
    if (ch < 'a' || ch > 'z') 
	    continue;
    
    //verifier que une bonne lettre deja trouve ne passe pas en mauvaise reponse en incremnetant lettres_incorrectes
    if(deja_trouve(ch,mot_affiche,strlen(mot_a_chercher)))
	continue;    

    trouve = 0;
    for(i = 0; i < strlen(mot_a_chercher);i ++) 
        if(mot_a_chercher[i] == ch && mot_affiche[i] == '_') {
            mot_affiche[i] = ch;
            trouve = 1;
            lettres_trouvees ++;
            }
        
    if(!trouve) {
        // Vérifier si la lettre n'a pas déjà été saisie incorrectement
        lettre_deja_saisie = 0;
        for(i = 0; i < nb_lettres_incorrectes; i++) 
            if(lettres_incorrectes[i] == ch) {
                lettre_deja_saisie = 1;
                break;
            	}
        if(!lettre_deja_saisie) {
            if(nb_lettres_incorrectes < MAX_WRONG_LETTERS) {
                lettres_incorrectes[nb_lettres_incorrectes] = ch;
                nb_lettres_incorrectes++;
                }
            erreurs ++;
            }
        }
    }
    
// Écran de fin

//OH
// Afficher le mot
    mvwprintw(win_jeu,5,2,"Mot : ");
    for(i = 0; i < strlen(mot_a_chercher); i++)
        mvwprintw(win_jeu,5,8 + i*2, "%c", mot_affiche[i]);

    // Afficher les erreurs
    mvwprintw(win_jeu,7,2, "Erreurs : %d / %d", erreurs, calcul_max_tentatives(mot_a_chercher));


    // Afficher les lettres incorrectes
    wclear(win_lettres_incorrectes);
    box(win_lettres_incorrectes,0,0);
    mvwprintw(win_lettres_incorrectes,1,2, "Lettres incorrectes :");
    for(i = 0; i < nb_lettres_incorrectes;i ++)
        mvwprintw(win_lettres_incorrectes,2 + i/10,2 + (i%10)*2, "%c", lettres_incorrectes[i]);

if(lettres_trouvees >= strlen(mot_a_chercher)) {
    mvwprintw(win_fin, 2, 2, "Félicitations ! Vous avez gagné !");
    mvwprintw(win_fin, 3, 2, "Le mot était : %s", mot_a_chercher);
    } 
else {
    mvwprintw(win_fin, 2, 2, "Dommage ! Vous avez perdu.");
    mvwprintw(win_fin, 3, 2, "Le mot était : %s", mot_a_chercher);
    // Dessiner le pendu selon les erreurs
    dessiner_pendu(win_pendu, COMPLET);
    }
    
// Rafraîchir toutes les fenêtres
wrefresh(win_jeu);
wrefresh(win_pendu);
wrefresh(win_lettres_incorrectes);
wrefresh(win_fin);
    
wgetch(win_fin);
    
// Libération mémoire et fermeture ncurses
free(mot_a_chercher);
free_dictionary(dictionary, word_count);
endwin();
    
return(0);
}
