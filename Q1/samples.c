#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("%s\n", "Input incorreto");
        return EXIT_FAILURE;
    }
    FILE *fp;
    char buffer[100];
    char *pend;
    fp = fopen(argv[1], "r");
    fseek(fp, 0L, SEEK_END);
    long int size = ftell(fp);
    rewind(fp);
    srand(0);
    long n = strtol(argv[2], &pend, 10), m = strtol(argv[3], &pend, 10), random;
    while(n > 0){
        random = rand() % (size - m + 1);
        fseek(fp, random, SEEK_SET);
        fread(buffer, m, 1, fp);
        printf(">%s<\n", buffer);
        rewind(fp);
        n--;
    }
    fclose(fp);
    return 0;
}