 #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void create_straight_tree(int n, int numpere, int original) {
    int i;
    int firstn=n;
    for (i = 0; i<n; i++) {
         if (fork() == 0) {
            printf("(%d,%d) Child process (pid=%d, parent=%d)\n", numpere,original-numpere-n+1, getpid(), getppid());
            // chaq enfant crée récursivemt un enfant
            create_straight_tree(n-1,numpere,original);
        }
        wait(NULL);
        exit(0);
    }
    // parent attend la fin des enfants
    for (i = 0; i <n; i++) {
        wait(NULL);
    }
}

void create_regular_tree(int n,int original){
    int i, child[2];
    if(n>1){
        if(fork()!=0){
        create_straight_tree(n-1,(original-n+1),original);
        } else {
            printf("(%d,%d) NChild process (pid=%d, parent=%d)\n",original-n+2,0,getpid(), getppid());
            create_regular_tree(n-1,original);
        }
        wait(NULL);
        return;
    }
}

int main(int argc, char* argv[]){
    int parentPid=getpid();
    int n=atoi(argv[1]);
    create_regular_tree(n,n);
    printf("ici\n");
         if (getpid() == parentPid) {
        char mychar[500];
        sprintf(mychar, "pstree -p %d", parentPid);
        system(mychar);    } 
    return 0;
}