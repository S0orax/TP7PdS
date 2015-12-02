#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

struct th_arg_s {
    char *bloc;
    unsigned long taille;
    unsigned long res;
};

void *th_compteur_gc(void *arg);

unsigned long compteur_gc(char *bloc, unsigned long taille) {
    unsigned long i, cptr = 0;

    for (i = 0; i < taille; i++)
        if (bloc[i] == 'G' || bloc[i] == 'C')
            cptr++;

    return cptr;
}

#define ARG ((struct th_arg_s *) arg)
void *th_compteur_gc(void *arg) {
    ARG->res = compteur_gc(ARG->bloc, ARG->taille);
    return arg;
}

int main(int argc, char *argv[]) {
    struct stat st;
    int fd;
    char *tampon;
    int lus, nbThread, i;
    unsigned long cptr = 0;
    off_t taille = 0;
    struct timespec debut, fin;
    struct th_arg_s *a;
    pthread_t *tids;

    assert(argv[1] != NULL);
    assert(argv[2] != NULL);

    nbThread = atoi(argv[2]);

    tids = (pthread_t *) malloc(sizeof(pthread_t) * nbThread);
    assert(tids != NULL);
    a = (struct th_arg_s *) malloc(sizeof(struct th_arg_s) * nbThread);
    assert(a != NULL);

    /* Quelle taille ? */
    assert(stat(argv[1], &st) != -1);
    tampon = malloc(st.st_size);
    assert(tampon != NULL);

    /* Chargement en mémoire */
    fd = open(argv[1], O_RDONLY);
    assert(fd != -1);
    while ((lus = read(fd, tampon + taille, st.st_size - taille)) > 0)
        taille += lus;
    assert(lus != -1);
    assert(taille == st.st_size);
    close(fd);

    /* Calcul proprement dit */
    assert(clock_gettime(CLOCK_MONOTONIC, &debut) != -1);
    /*cptr = compteur_gc(tampon, taille);*/

    for(i = 0; i < nbThread; i++) {
        int currentTaille;

        a[i].bloc = tampon + i * taille / nbThread;
        currentTaille = taille - i * taille / nbThread;
        a[i].taille = currentTaille / (nbThread - i);
        a[i].res = 0;
        printf("%s %ld\n", a[i].bloc, a[i].taille);
        assert(pthread_create(&tids[i], NULL, th_compteur_gc, (void *)(&a[i])) == 0);
    }

    for(i = 0; i < nbThread; i++) {
        assert(pthread_join(tids[i], NULL) == 0);
        cptr += a[i].res;
    }

    free(tids);
    free(a);

    assert(clock_gettime(CLOCK_MONOTONIC, &fin) != -1);

    /* Affichage des résultats */
    printf("Nombres de GC:   %ld\n", cptr);
    printf("Taux de GC:      %lf\n", ((double) cptr) / ((double) taille));

    fin.tv_sec  -= debut.tv_sec;
    fin.tv_nsec -= debut.tv_nsec;
    if (fin.tv_nsec < 0) {
        fin.tv_sec--;
        fin.tv_nsec += 1000000000;
    }
    printf("Durée de calcul: %ld.%09ld\n", fin.tv_sec, fin.tv_nsec);
    printf("(Attention: très peu de chiffres après la virgule sont réellement significatifs !)\n");

    return 0;
}
