#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include <time.h>

int main (int argc, char *argv[]) {
    if(argc != 4) {
        return EXIT_FAILURE;
    }

    char *pend = (char*)malloc(50 * sizeof(char));
    int n = atoi(argv[1]);

    for(int i = 1; i <= n; i++) {
        if(i == n)
            sprintf(pend, "pipe%dto1", i);
        else
            sprintf(pend, "pipe%dto%d", i, i+1);

        if((mkfifo(pend, 0666)) < 0) {
            fprintf(stderr, "%s: erro na criacao do named pipe: %s", argv[0], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    free(pend);

    int p = 1 / atof(argv[2]);
    int token = 0;

    pid_t pids[n];
    char *escrever_pipe = (char*)malloc(50 * sizeof(char));
    char *ler_pipe = (char*)malloc(50 * sizeof(char));

    for(int i = 1; i <= n; i++) {
        if((pids[i-1] = fork()) < 0) {
            fprintf(stderr, "%s: erro na criacao do processo: %s\n", argv[0], strerror(errno));
            exit(EXIT_FAILURE);
        } else if(pids[i-1] == 0) {
            if(i == n) {
                sprintf(escrever_pipe, "pipe%dto1", i);
                sprintf(ler_pipe, "pipe%dto%d", i-1, i);
            } else if(i == 1) {
                sprintf(escrever_pipe, "pipe%dto%d", i, i+1);
                sprintf(ler_pipe, "pipe%dto1", n);
            } else {
                sprintf(escrever_pipe, "pipe%dto%d", i, i+1);
                sprintf(ler_pipe, "pipe%dto%d", i-1, i);
            }

            srandom(time(NULL) - i * 2);

            // armazenar o pipe num array
            int arr[2];

            if(i == 1) {
                if((arr[1] = open(escrever_pipe, O_WRONLY)) < 0) {
                    fprintf(stderr, "%s: erro a abrir o pipe: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }

                token++;

                if(write(arr[1], &token, sizeof(int)) < 0) {
                    fprintf(stderr, "%s: erro de escrita: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                close(arr[1]);
            }

            for(;;) {

                // lê a token do processo anterior
                if((arr[0] = open(ler_pipe, O_RDONLY)) < 0) {
                    fprintf(stderr, "%s: erro a abrir o pipe: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if(read(arr[0], &token, sizeof(int)) < 0) {
                    fprintf(stderr, "%s: erro de leitura: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }

                close(arr[0]);

                //incrementar a token
                token++;
                
                int rand = random() % p;
                if(rand == 1) {
                    printf("[p%d] lock on token (val = %d)\n", i, token);
                    sleep(atoi(argv[3]));
                    printf("[p%d] unlock token\n", i);
                }

                // envia a token para o processo seguinte
                if((arr[1] = open(escrever_pipe, O_WRONLY)) < 0) {
                    fprintf(stderr, "%s: erro a abrir o pipe: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                
                if(write(arr[1], &token, sizeof(int)) < 0) {
                    fprintf(stderr, "%s: erro de escrita: %s\n", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                close(arr[1]);
            }
            exit(EXIT_SUCCESS);
        }
    }

    for(int i = 0; i < n; i++) {
        if(waitpid(pids[i], NULL, 0) < 0) {
            fprintf(stderr, "%s: erro waitpid: %s\n", argv[0], strerror(errno));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}