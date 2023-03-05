#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void create_straight_tree(int n, int numpere, int original, int* pipe_pere_desc, int*pipe_pere_asc) {
    int i;
    int firstn=n;
    DIR *dir;

    if(n>0){
        int pipe_fils_desc[2];
        int pipe_fils_asc[2];
        pipe(pipe_fils_asc);
        pipe(pipe_fils_desc);
         if (fork() == 0) { //fils
            printf("(%d,%d) NProcessus (pid=%d, parent=%d)\n", numpere,original-numpere-n+1, getpid(), getppid());
            close(pipe_pere_asc[0]);
            close(pipe_pere_asc[1]);
            close(pipe_pere_desc[0]);
            close(pipe_pere_desc[1]);
            // chaq enfant crée récursivemt un enfant
            create_straight_tree(n-1,numpere,original, pipe_fils_desc, pipe_fils_asc);
        }
        //père
        //coordonnées : numpere, original_numpere_n
        close(pipe_pere_asc[0]);
        close(pipe_pere_desc[1]);
        close(pipe_fils_asc[1]);
        close(pipe_fils_desc[0]);

        char dirname[10];
        char filename[10];
        char path[30];
        char buf;
        if((numpere==1)&&(original-numpere-n==0)){
            buf='c';
        }
        sprintf(dirname,"%d-%d",numpere,original-numpere-n);

        if((dir=opendir(dirname))==NULL){
            mkdir(dirname,0644);
            perror("");
            printf("Création du dossier : %s\n",dirname);
            fflush(stdout);
            dir=opendir(dirname);
        }
        struct stat st;
        strcpy(filename, "/visite_1");
        strcpy(path, dirname);
        strcat(path, filename);
        //char *nomfichier=strcat(str,"/visite_1");
        if(stat(path,&st)==0){
            //Si fichier existe
            /*if((original-numpere-n)==0){//Si on est le père original et q fichier existe
                if(numpere==1){ // Si on est le process de base et q fichier existe
                    //De fait, pipe_pere_desc est en fait pipe_desc_child
                    //écriture ds fichier, lancement du timer
                    write(pipe_pere_desc[1],&buf,sizeof(char)); //on passe à son fils de droite
                    read(pipe_pere_asc[0],&buf,sizeof(char)); //on lit qd ça revient à la fin
                    //fin du timer, écriture ds fichier
                    close(pipe_pere_desc[1]);
                    close(pipe_pere_asc[0]);
                    //Fin de la circulation du jeton
                }
            //IMPOSSIBLE CAR ON N'A PAS LE TUBE VERS LE PERE
            //De fait, pipe_pere_desc est en fait pipe_desc_child
            //écriture ds fichier, lancement du timer
            //write(pipe_pere_desc[1],&buf,sizeof(char)); //on passe à son fils de droite
            //fin du timer, écriture ds fichier
            //read(pipe_pere_asc[0],&buf,sizeof(char)); //on lit ce que le fils nous renvoie
            //write(pipe_pere_desc[1],&buf,sizeof(char));
            close(pipe_pere_desc[1]);
            close(pipe_pere_asc[0]);
            //veut dire qu'on doit passer à la branche suivante
            }
            //Le fichier existe mais on n'est pas le père original
            close(pipe_fils_desc[1]);
            close(pipe_pere_desc[0]);
            read(pipe_fils_asc[0],&buf,sizeof(char));
            char*nomf2=strcat(str,"/visite_2");
            int visite2 = open(nomfichier,O_CREAT|O_WRONLY,0644);
            //Ecrire temps final dans fichier (finir timer)
            write(pipe_pere_asc[1],&buf,sizeof(char));
            close(pipe_fils_asc[0]);
            close(pipe_pere_asc[1]);*/
        } else {
            //Si fichier n'existe pas
            //close(pipe_fils_asc[0]);
            //close(pipe_pere_asc[1]);

            read(pipe_pere_desc[0],&buf,sizeof(char));

            int visite1 = open(path,O_CREAT|O_WRONLY,0644);
            write(visite1,&buf,sizeof(char));
            //Lancer timer
            if(write(pipe_fils_desc[1],&buf,sizeof(char))==0){
                printf("Ecrit");
                fflush(stdout);
            }
            close(visite1);
            read(pipe_fils_asc[0],&buf,sizeof(char));
            strcpy(filename, "/visite_2");
            strcpy(path, dirname);
            strcat(path, filename);
            int visite2 = open(path,O_CREAT|O_WRONLY,0644);
            //Ecrire temps final dans fichier (finir timer)
            write(pipe_pere_asc[1],&buf,sizeof(char));
            wait(NULL);
            close(pipe_fils_desc[1]);
            close(pipe_pere_desc[0]);
            close(visite2);
            close(pipe_fils_asc[0]);
            close(pipe_pere_asc[1]);

        }
        wait(NULL);
        exit(0);
    } else {
        //dernier de la chaine
        printf("Je suis le processus %d - %d",numpere,original-numpere-n);
        fflush(stdout);
            char dirname[10];
            char filename[10];
            char path[30];
            char buf;
            sprintf(dirname,"%d-%d",numpere,original-numpere-n);
            if((dir=opendir(dirname))==NULL){
                mkdir(dirname,0644);
                perror("");
                dir=opendir(dirname);
            }
            strcpy(filename, "/visite_1");
            strcpy(path, dirname);
            strcat(path, filename);
            read(pipe_pere_desc[0],&buf,sizeof(char));
            int visite1 = open(path,O_CREAT|O_WRONLY,0644);
            printf("char : %c\n",buf);
            write(visite1,&buf,sizeof(char));
            write(pipe_pere_asc[1],&buf,sizeof(char));
            close(visite1);
            wait(NULL);
            close(pipe_pere_asc[1]);
            close(pipe_pere_desc[0]);
            close(pipe_pere_asc[0]);
            close(pipe_pere_desc[1]);
    }
    // parent attend la fin des enfants
    for (i = 0; i <n; i++) {
        wait(NULL);
    }
}

void create_regular_tree(int n,int original){
    int pipe_asc_child_2[2];
    int pipe_desc_child_2[2];
    pipe(pipe_asc_child_2);
    pipe(pipe_desc_child_2);
    if(n>1){
        if(fork()==0){
            printf("(%d,%d) Processus (pid=%d, parent=%d)\n",original-n+2,0,getpid(), getppid());
            close(pipe_asc_child_2[0]);
            close(pipe_asc_child_2[1]);
            close(pipe_desc_child_2[0]);
            close(pipe_desc_child_2[1]);
            create_regular_tree(n-1,original);
        } else {
            create_straight_tree(n-1,(original-n+1),original,pipe_desc_child_2, pipe_asc_child_2);
        }
        wait(NULL);
    } else {
        //Pour processus n-0
        create_straight_tree(0,original,original,NULL,NULL);
    }
    //printf("la\n");
}

int main(int argc, char* argv[]){

   struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    printf("Current stack size limit: %lu bytes\n", limit.rlim_cur);
    
    limit.rlim_cur = 1024 * 1024 * 16; // set the stack size limit to 16 MB
    if (setrlimit(RLIMIT_STACK, &limit) != 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }
    
    getrlimit(RLIMIT_STACK, &limit);
    printf("New stack size limit: %lu bytes\n", limit.rlim_cur);

    int parentPid=getpid();
    int n=atoi(argv[1]);
    create_regular_tree(n,n);
    //printf("ici\n");
    if (getpid() == parentPid) {
            printf("dedans\n");
            char mychar[500];
        sprintf(mychar, "pstree -p %d", parentPid);
        system(mychar);    } 
    return 0;
}