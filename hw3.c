#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define SIZE 1024
#define sfname "sha"

double **A, *B, *X;
pthread_barrier_t barrier;
int num_processes; 
int size;

void argCheck(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <mat> <invec> <outvec> <np>\n", argv[0]);
        exit(1);
    }
}

int sizeCheck(int argc, char *argv[]) {
    struct stat finfoA, finfoB;
    int noeleA, noeleB;

    stat(argv[1], &finfoA);
    stat(argv[2], &finfoB);

    noeleA = finfoA.st_size / sizeof(double);
    noeleB = finfoB.st_size / sizeof(double);

    if (noeleA != noeleB * noeleB) {
        fprintf(stderr, "Error: no. elements(%d) of %s is not square of that(%d) of %s.\n", noeleA, argv[1], noeleB, argv[2]);
        exit(1);
    }
    num_processes = strtol(argv[4], NULL, 10);
    if (num_processes <= 0) {
        fprintf(stderr, "Error: invalid number of threads. It should be greater than 0.\n");
        exit(1);
    }

    return noeleB;
}

void getData(int size, int argc, char *argv[]) {
    int afd = open(argv[1], O_RDONLY);
    int bfd = open(argv[2], O_RDONLY);
    for (int i = 0; i < size; i++) {
        read(afd, A[i], sizeof(double) * size);
    }
    read(bfd, B, sizeof(double) * size);
    close(afd);
    close(bfd);
}

void *GaussianElimination(void *arg) {
    int process_id = *(int *)arg;
    printf("process_id=%d\n",*(int *)arg);
    //printf("X Gaussian Before : %d , %d\n",X[0],X[1]);  
    //pi는 쓰레드 index, m은 작업의 크기, p는 thread 개수, r은 m%p;
    //start -> process_id*m/p;
    //end -> process_id*m/p-1
    
    for (int k = 0; k < size - 1; k++) {       
        int start = (process_id * size) / num_processes;
        int end = ((process_id + 1) * size) / num_processes;
        for (int i = start; i < end; i++) {
            if (i > k) {
                double factor = A[i][k] / A[k][k];
                for (int j = k; j < size; j++) {
                    A[i][j] -= factor * A[k][j];   
                }
                B[i] -= factor * B[k];
            }
        }
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(NULL);
}

// void GaussianElimination(int size) {
//     for (int i = 0; i < size - 1; i++) {
//         for (int j = i + 1; j < size; j++) {
//             double factor = A[j][i] / A[i][i];
//             for (int k = i; k < size; k++) {
//                 A[j][k] -= factor * A[i][k];
//             }
//             B[j] -= factor * B[i];
//         }
//     }
// }

void *BackSubstitution(void *arg) {
    int process_id = *(int *)arg;
    for (int i = size - 1; i >= 0; i--) {
        if (process_id == 0) {
            X[i] = B[i] / A[i][i];
        }
        pthread_barrier_wait(&barrier);
        for (int j = process_id; j < i; j += num_processes) {
            B[j] -= X[i] * A[j][i];
            A[j][i] = 0;
        }
        pthread_barrier_wait(&barrier);
    }

    //printf("%d %d\n",X[0],X[1]);
   
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    printf("Start\n");
   
    argCheck(argc, argv);
    size = sizeCheck(argc, argv);
    A = (double **)malloc(size * sizeof(double *));
        for(int i=0;i<size;i++){
            A[i] = (double *)malloc(size * sizeof(double));
    }
    B = (double *)malloc(size * sizeof(double));
    getData(size, argc, argv);
    num_processes = strtol(argv[4], NULL, 10);
    

    // for(int i=0;i<size;i++){
    //     for(int j=0;j<size;j++){
    //         printf("%f ",A[i][j]);
    //     }   
    //     printf("\n");
    // }

    // if(num_processes>size){
    //     num_processes=size;
    // }
    
    X = (double *)malloc(size * sizeof(double));
   
//real final
    pthread_barrier_init(&barrier, NULL, num_processes); 
    pthread_t threads[num_processes];
    int thread_ids[num_processes];


    for (int i = 0; i < num_processes; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, GaussianElimination,  &thread_ids[i]); 
    }

    for (int i = 0; i < num_processes; i++) {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < num_processes; i++) {
        pthread_create(&threads[i], NULL, BackSubstitution, &thread_ids[i]);
    }
    for (int i = 0; i < num_processes; i++) {
        pthread_join(threads[i], NULL);
    }
    int xfd = creat(argv[3], 0644);
    write(xfd, X, sizeof(double) * size);    
    close(xfd);
    pthread_barrier_destroy(&barrier); 

    for (int i = 0; i < size; i++) {
        free(A[i]);
    }
    free(A);
    free(B);
    free(X);

    return 0;
}

//hello
//gcc -g -o hw3 hw3.c -pthread
//gdb hw3
//run A2.dat B2.dat X2.dat 3