#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    pid_t pid, wpid;

    for(int i = 0; i < argc - 1; i++){
        pid = fork();

        if(pid == -1){
            perror("erro fork");
            return EXIT_FAILURE;
        }

        if(pid == 0){
            char temp[sizeof(argv[i + 1]+1)] = {}, *dot;
            strcpy(temp, argv[i + 1]);
            dot = strchr(temp, '.');
            if(dot != 0){*dot = 0;}
            strcat(temp, ".epub");
            execvp("pandoc", (char *[]){"pandoc", argv[i + 1], "-o", temp, NULL});
            return EXIT_SUCCESS;
        }
    }

    
    int status = 0;
    while ((wpid = wait(&status)) > 0);
    
    char *zip[argc + 4];
    zip[0] = malloc((strlen("zip") + 1) * sizeof(char));
    strcpy(zip[0], "zip");
    zip[1] = malloc((strlen("ebooks.zip") + 1) * sizeof(char));
    strcpy(zip[1], "ebooks.zip");
    for(int k = 0; k < argc - 1; k++){
        char temp[sizeof(argv[k + 1]+1)] = {}, *dot;
        strcpy(temp, argv[k + 1]);
        dot = strchr(temp, '.');
        if(dot != 0){*dot = 0;}
        strcat(temp, ".epub");
        zip[k + 2] = malloc((strlen(temp) + 1) * sizeof(char));
        strcpy(zip[k+2], temp);
    }
    zip[argc + 2] = "--quiet";
    zip[argc + 3] = NULL;

    execvp(zip[0], zip);

    return EXIT_SUCCESS;
}