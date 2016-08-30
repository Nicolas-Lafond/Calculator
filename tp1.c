/* TP1 IFT2035
 * by Nicolas Lafond 
 * version 2
 * postfix calculator with infinite precision
 * numbers are stocked in linked list where each digit is stocked in an element
 * the first digit in the list is the last digit of the number represented
 * addition, substraction and multiplication are supported but not division
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

typedef struct Digit {
    char value;
    struct Digit *next;
} Digit;

typedef struct Num {
    int sign;  // 0 : negative  1 : positive
    struct Digit *last; // pointer to the least significant digit
    struct Num *next; // pointer to the element below on the stack
    int nb_ref; // number of variables who refere to that number
} Num;

typedef struct Stack {
    struct Num *top = NULL;
    int nb_elem = 0;
} Stack


void push(Stack *s, Num *a) {
    a->next = s->top;
    s->top = a;
    s->nb_elem++;
}

typedef enum State {
    INIT,
    DIGIT,
    ERROR,
    EQUAL
} State;

num* pop(Stack *s) {
    if (s->nb_elem == 0)
        return NULL;
    else {
        Num *n = s->top;
        s->top = s->top->next;
        s->nb_elem--;
        return n;
    }
}

void emptyStack(Stack *s) {
    while (s->nb_elem > 0)
        pop(s);
}

/*
 * Fonction qui ajoute un nouveau chiffre non initialisé après celui entré en argument
 * return 0 si l'opération a échoué et 1 sinon
 */
int ajouteCell(cell *a, int val) {
    cell *nouveau = malloc(sizeof(cell));
    if (nouveau == NULL)
        return 0;
    else {
        a->suivant = nouveau;
        nouveau->precedent = a;
        nouveau->suivant = NULL;
        nouveau->chiffre = val;
    }
    
    return 1;
}

/*
 * Efface un nombre
 */
void efface(num *nombre) {
    if (nombre == NULL)
        return;
    else if (nombre->chiffre == NULL) {
        free(nombre);
        return;
    }
    else {
        cell *p, *q;
        p = nombre->chiffre;
        free(nombre);
        
        while (p != NULL){
            q = p->suivant;
            free(p);
            p = q;
        }
    
    }
    
    return;
}

/*
 * Affiche un nombre
 */
void affiche (num *nombre) {
    if (nombre == NULL)
        return;
    cell *c = nombre->chiffre;
    
    while(c->suivant != NULL)
        c = c->suivant;
    
    if (nombre->positif == false)
        printf("-");
        
    while (c != NULL) {
        printf("%d",c->chiffre);
        c = c->precedent;    
    }
    
    printf("\n");
    
}

/*
 * procédure qui évalue si a est plus petit que b
 * retourne true si a < b, false sinon
 * on considère que a et b sont positif
 */
int estPlusPetit(num *a, num *b) {
 
        cell *c1 = a->chiffre, *c2 = b->chiffre;
        if (c1->suivant == NULL && c1->suivant == NULL) //si a et b n'ont qu'un chiffre
            return (c1->chiffre < c2->chiffre);
        else if (c1->suivant == NULL)
            return true;
                
        while (c1->suivant != NULL) {
            
            if (c2->suivant == NULL)
                return false;
            else {
                c1 = c1->suivant;
                c2 = c2->suivant;
            }
        }
        
        if (c2->suivant != NULL)
            return true;
            
        while (c1->precedent != NULL) {
            if (c1->chiffre != c2->chiffre)
                if (c1->chiffre < c2->chiffre)
                    return true; 
                else
                    return false;
            c1 = c1->precedent;
            c2 = c2->precedent;
            
        }
        
        return false; //ils sont égaux
      
}


//retourne true si a est zero
int estZero(num *a) {
    return (a->chiffre->chiffre == 0) && (a->chiffre->suivant == NULL);
}
/*
 * procédure qui additionne deux nombres a et b positifs
 * on additionne les nombre un à un et on ajoute les retenues
 * retourne NULL si la mémoire est insuffisante
 */
num* addP(num *a, num *b) {
   
    int retenue = 0, s = 0;
    num *somme = malloc(sizeof(num));
    if (somme == NULL)
        return NULL;
    somme->positif = a->positif; 
    somme->chiffre = malloc(sizeof(cell));
    if (somme->chiffre == NULL)
        return NULL;
    
    //c1 est le chiffre du premier nombre, c2 le chiffre du deuxième nombre et cs le chiffre du nombre somme
    cell *c1 = a->chiffre, *c2 = b->chiffre, *cs = somme->chiffre;
    
    while(!(c1 == NULL && c2 == NULL)) { // tant que il reste des chiffres à additionner
        
        if (c1 == NULL && c2 != NULL) {
            s = (c2->chiffre) + retenue;
            c2 = c2->suivant;
        
        }
        else if(c1 != NULL && c2 == NULL) {
            s = (c1->chiffre) + retenue;
            c1 = c1->suivant;
        
        }
        else {
            s = (c1->chiffre) + (c2->chiffre) + retenue;
            c1 = c1->suivant;
            c2 = c2->suivant;
        
        }    
        
        if (s > 9) retenue = 1;
        else retenue = 0;
        
        cs->chiffre = s%10;
        ajouteCell(cs,0);
        cs = cs->suivant;
    }
    
    if (retenue)
        cs->chiffre = 1;
    else {
        cs->precedent->suivant = NULL;
        cs->precedent = NULL;
        free(cs);
    }  
    return somme;
}


void inverseSigne(num *n) {
    if (n->positif == true)
        n->positif = false;
    else
        n->positif = true;
}

/*
 * procédure qui soustrait deux nombres a et b positifs et tel que a>b
 */
num* subP(num *a, num *b) {

    int d = 0, retenue = 0;
    num *difference = malloc(sizeof(num));
    if (difference == NULL)
        return NULL;
    difference->positif = true;
    difference->chiffre = malloc(sizeof(cell));
    if (difference->chiffre == NULL)
        return NULL;
        
    cell *c1 = a->chiffre, *c2 = b->chiffre, *cd = difference->chiffre;
    
    while(c1 != NULL) { // tant que il reste des chiffres à soustraire
        if ( c2 == NULL){
            cd->chiffre = (c1->chiffre) -retenue;
            retenue = 0;
        }
        else {
            if (c1->chiffre >= c2->chiffre + retenue) {
                cd->chiffre = (c1->chiffre) - ((c2->chiffre) + retenue);
                retenue = 0;
            }
            else {
                cd->chiffre = 10 + (c1->chiffre) - (c2->chiffre + retenue);
                retenue = 1;
            }
            c2 = c2->suivant;
    
        }
        
        ajouteCell(cd,0);
        cd = cd->suivant;
        c1 = c1->suivant;
        
    }
    cd->precedent->suivant = NULL;
    cd->precedent = NULL;
    free(cd);
    
    
    return difference;
}



//additionne deux nombres quelconques
num* add(num *a, num *b) {
    
    if (estZero(a))  //si a est zéro
        return b;
    else if (estZero(b)) //si b est zéro
        return a;
    else if (a->positif == true && b->positif == false) //si a est positif et b négatif
        if (estPlusPetit(a,b)) {
            num *r = subP(b,a);
            r->positif = false;
            return r;
        }
        else
            return subP(a,b);
            
    else if (a->positif == false && b->positif == true) { //si a est négatif et b positif
     if (estPlusPetit(b,a)) {
            num *r = subP(a,b);
            r->positif = false;
            return r;
        }
        else
            return subP(b,a);
     }
    else if (a->positif == true) { //si ils sont positifs
        return addP(a,b);
    }
    else { //si a et b sont négatifs  
        num *r = addP(a,b);
        r->positif = false;
        return r;
    }   
}

num* sub(num *a, num *b) {
    
    if (estZero(a)) { //si a est zéro
        inverseSigne(b);
        return b;
    }
    else if (estZero(b)) //si b est zéro
        return a;
    else if (a->positif == true && b->positif == false) //si a est positif et b négatif
        return add(a,b);
    else if (a->positif == false && b->positif == true) { //si a est négatif et b positif
        num *r = add(a,b);
        r->positif = false;
        return r;   
    }
    else if (a->positif == true) { //si ils sont positifs
        if (estPlusPetit(a,b)) { //si a<b
            num *r = subP(b,a);
            r->positif = false;
            return r;
        }
        else
            return subP(a,b);
    }
    else { //si a et b sont négatifs alors on calcule |b|-|a|
        if (estPlusPetit(b,a)) { //si |b|<|a|
            num *r = subP(a,b);
            r->positif = false;
            return r;
        }
        else  {
            num *r = subP(b,a);
            r->positif = true;
            return r;
        }
    }    
}

/*
 * Fonction qui multiplie un nombre a par un chiffre ch
 */
num* mulChiffre(num *a, int ch) {
    int retenue = 0, r;
    num *p = malloc(sizeof(num));
    p->positif = true;
    p->chiffre = malloc(sizeof(cell));
    cell *c = a->chiffre, *cp = p->chiffre;
    
    r = ch*(c->chiffre) + retenue;
    cp->chiffre = r%10;
    retenue = r/10;
    
    while(c->suivant != NULL) {
        c = c->suivant;
        r = ch*(c->chiffre) + retenue;
        ajouteCell(cp,r%10);
        retenue = r/10;
        cp = cp->suivant;
    }
    
    if (retenue)
        ajouteCell(cp,retenue);
        
    return p;
}

/*
 * Fonction qui ajoute les chiffres de b à a, ex:a=12345 b=6789 alors
 * a=123456789 et b n'est pas modifier
 */
void ajouteChiffres(num *a, num *b) {
    cell *c = b->chiffre;
    cell *r = malloc(sizeof(cell));
    cell *debut =r;
    
    while(c != NULL) {
        r->chiffre = c->chiffre;
        ajouteCell(r,0);
        c = c->suivant;
        r = r->suivant;
    }
    
    r->suivant = a->chiffre;
    a->chiffre->precedent = r;
    a->chiffre = debut;
    return;
}

/*
 * procédure qui multiplie deux nombres a et b
 */
num *mul(num *a, num *b) { 

    num *produit = malloc(sizeof(num));
    produit->positif = true;
    produit->chiffre = malloc(sizeof(cell));
    num *zero = malloc(sizeof(num));
    zero->chiffre = malloc(sizeof(cell));
    zero->chiffre->chiffre = 0;
    
   
              
    //on initialise un nombre
    num  *p, *q ;
    produit = mulChiffre(a,b->chiffre->chiffre);
    
    cell *c = b->chiffre->suivant;
    
    while (c != NULL) {
        p = mulChiffre(a,c->chiffre);
        ajouteChiffres(p,zero); //on ajoute les zéro à la fin du nombre
        q = addP(produit,p);
        efface(p);
        produit = q;
        c = c->suivant;
        
        // on ajoute un 0 à zero
        cell *n = malloc(sizeof(cell));
        n->chiffre = 0;
        n->suivant = zero->chiffre;
        zero->chiffre = n;
    }
    
   if (a->positif == b->positif) // si a est positif et b négatif
        produit->positif = true;
   else
        produit->positif = false;
   
    efface(zero);
    return produit;
}



num* stockeNombre(char* s, int debut, int fin) {
    //prend en argument une chaine de caractère constitué uniquement de chiffre sans espace et retourne un pointeur vers un num
    //qui represente ce chiffre
    int i = fin - 1;
    num *nombre = malloc(sizeof(num));
    if (s[debut] == '-') {
        nombre->positif = false;
        debut += 1;
    }
    else
        nombre->positif = true; 
    
    cell *p = malloc(sizeof(cell));
    nombre->chiffre = p;
    p->chiffre = s[fin] - 48;
    while (i >= debut) {
        ajouteCell(p, s[i] - 48);
        p = p->suivant;
        i--;
    }
    
    return nombre;
    
}

int estNombre(char* s, int debut, int fin) {
    int valide = true;
    int i;
    for (i = debut; i < fin; i++)
        if ( s[i] < '0' ||  s[i]> '9') 
            valide = false;
 
    return valide;
}



void traitement_chaine(char *s, int taille) {
    //traite une chaine entré par l'utilisateur
    char c = s[0];
    int i = 0, j; //i est l'indice ou on est dans la chaine
    num *v1, *v2;
    pile *p = creePile();
    
    while (i < taille) {
        j = prochainEspace(s,i,taille);
    
        if (s[i] == '+') {
            
            v1 = pop(p);
            v2 = pop(p);
            push(p,add(v1,v2));
           
        }
        else if (s[i] == '-') {
            v1 = pop(p);
            v2 = pop(p);
            push(p,sub(v1,v2));
        }
        else if (s[i] == '*') {
            v1 = pop(p);
            v2 = pop(p);
            push(p,mul(v1,v2));
        }
         
        else if (estNombre(s, i, j)) {
            v1 = stockeNombre(s,i,j-1);
            push(p,v1);
        }
        else {
            printf("Erreur entré non valide\n");
            return;
        }
        
        i = prochainCaractere(s,j,taille);
    
        if (j == -1) {
            affiche(pop(p));
            return;
        } 
         
    }
    
    affiche(pop(p));
    return;
}



int prochainEspace(char* s ,int i, int taille) {
    //retourne l'indice du debut du prochain espace d'une chaine de caractère à partir de la ième lettre
    //retourne -1 si on atteint la fin de la chaine
    while( i < taille)
        if (s[i] == ' ' || s[i] == '\n')
            return i;
        else
            i++;
    
    return -1;
}

int prochainCaractere(char* s ,int i, int taille) {
    //retourne l'indice du debut du prochain caractère d'une chaine de caractère à partir de la ième lettre
    //retourne -1 si on atteint la fin de la chaine
    while( i < taille)
        if (s[i] != ' ' && s[i] != '\n')
            return i;
        else
            i++;
    
    return -1;
}

int isDigit(int c) {
    return (c >= '0') && (c <= '9');
}

int isLowLetter(int c) {
    return (c >= 'a') && (c <= 'z');
}

int main() {

    int c;
    State state;
    Stack stack;
    Num *n, variables[26];
    Digit *d;
    
    s = INIT;
    printf(">");

    while ( (c = getchar()) != EOF) {
        if (c == '\n' && state == ERROR) {
            printf("error incorrect input\n>");
            emptyStack(&stack);
            state = INIT;
            continue;
        }
        else if (c == '\n') {
            printValue(stack.top);
            emptyStack(&stack);
            state = INIT;
            continue;
        }

        switch (state) {
            case INIT:
                if (c == ' ' || c == '\t')
                    break;
                else if (isDigit(c)) {
                    d = createDigit(c);
                    n = createNumber(d);
                }
                else if (c == '+')
                    add(&stack);
                else if (c == '-')
                    substract(&stack);
                else if (c == '*')
                    multiply(&stack);
                else if (isLowLetter(c))
                    push(&stack, value(c));
                else 
                    state = ERROR;       
                break;
    
            case DIGIT:
                if (c == ' ' || c == '\t') {
                    push(&stack, n);
                    state = INIT;
                }
                else if (isDigit(c)) {
                    d = createDigit(c);
                    addDigit(n, d);
                }
                else 
                    state = ERROR;
                break;
   
            case EQUAL:
                if (isLowLetter(c) && (stack.nb_elem == 0))
                    state = ERROR;
                else if (isLowLetter(c) && (stack.nb_elem > 0))
                    affectation(stack.top, c);
                else 
                    state = ERROR;
                break;    
         
            case ERROR:           
                break;
        }
    }

    return 0;	
 
}

