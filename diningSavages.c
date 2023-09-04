#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

 #define BROJ_DIVLJAKA 4
 #define BROJ_PORCIJA 3
 #define VELICINA_PORCIJE 3

sem_t prazanLonac;
sem_t punLonac;

void *divljak (void*);
void *kuvar (void*);

static pthread_mutex_t porcije_mutex;

static int porcije = 0;

int main() {

    printf("Resenje problema Dining Savages\n");

    //kreiranje varijabli i threadova
    int i, id[BROJ_DIVLJAKA+1];
    pthread_t tid[BROJ_DIVLJAKA+1];

    // inicijalizacija mutexa
    pthread_mutex_init(&porcije_mutex, NULL);

    //inicijalizacija semafora
    sem_init(&prazanLonac, 0, 0);
    sem_init(&punLonac,  0, 0);
    
    //kreiraju se niti za svakog divljaka i za jednog kuvara
    for (i=0; i<BROJ_DIVLJAKA; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, divljak, (void *)&id[i]);
    }
    pthread_create (&tid[i], NULL, kuvar, (void *)&id[i]);
    
    //pthread_join omogucava da aplikacija ceka da se thread zavrsi
    for (i=0; i<BROJ_DIVLJAKA; i++)
        pthread_join(tid[i], NULL);
}


void *kuvar (void *id) {
    int kuvar_id = *(int *)id;
    double brojSpremanja = (1.0*BROJ_DIVLJAKA*BROJ_PORCIJA)/VELICINA_PORCIJE;//broj ukupnih porcija/broj serviranja
    brojSpremanja = ceil(brojSpremanja);//ukoliko je broj spremanja npr 4.3, znaci da je potrebno 5 spremanja da bi svi jeli
    
    int i = 1;
    while ( i <= brojSpremanja ) {
        sem_wait (&prazanLonac);
        
        printf ("\n.......KUVA SE.....\n");
        
        sleep(4);
        porcije += VELICINA_PORCIJE;
  
        printf ("\n>>Kuvar je spremio %d porcija po %d. put\n", VELICINA_PORCIJE, i);
        
        sem_post (&punLonac);
        i++;
    }
    return NULL;
}

void *divljak (void *id) {
    int divljak_id = *(int *)id;
    int brojPorcija = BROJ_PORCIJA;
    
    while ( brojPorcija ) {
        pthread_mutex_lock (&porcije_mutex);
        if (porcije == 0){
            sem_post(&prazanLonac);
            
             printf ("Divljak %i je probudio kuvara i ceka da hrana bude gotova..\n", divljak_id+1);
            
            sem_wait(&punLonac);
        }
        porcije--;
        sleep(0.5);//vreme potrebno da se porcija uzme
        pthread_mutex_unlock (&porcije_mutex);

        brojPorcija--;


        printf ("Divljak %i jede porciju %d\n", divljak_id+1, BROJ_PORCIJA-brojPorcija);


        sleep(2);//vreme potrebno da se porcija pojede

        printf ("Divljak %i je pojeo  %d. porciju\n", divljak_id+1, BROJ_PORCIJA-brojPorcija);
    }
    

    return NULL;
}
