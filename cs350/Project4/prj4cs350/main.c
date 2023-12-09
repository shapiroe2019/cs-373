//Author: Jayson Boubin -- Fall 2022
#include "matmul.h"

#ifdef SMALL
    #define MAX_THREADS 4
#endif

#ifdef MED
    #define MAX_THREADS 10
#endif

#ifdef BIG
    #define MAX_THREADS 500
#endif

struct cell{
    int row;
    int col;
};

int* mat1 = NULL;
int* mat2 = NULL;
int* matRes = NULL;
int* matSol = NULL;

int sum(int row, int col){ //A (MxN) X B (NxK) == C (MxK)
    int result = 0;
    for(int i = 0; i < N; i++) {
        result += (mat1[row*N+i] * mat2[i*K+col]);
    }
    return result;
}

void multiply() {
    //TODO
    for(int i = 0; i<M; i++) {
        for(int j = 0; j<K; j++){
            matRes[i*K+j] = sum(i,j);
        }
    }
    //printMats();
}

void *runner(void *param){
    struct cell *data = param;
    matRes[data->row*K + data->col] = sum(data->row, data->col);
    pthread_exit(0);
}

void multiplyWithThreads(int nThreads){
    //TODO
    if(nThreads > MAX_THREADS) {
        nThreads = MAX_THREADS;
    }
    pthread_t *threads = malloc(sizeof(pthread_t)* nThreads);
    int threadIdx = 0;

    for(int i = 0; i<M; i++) {
        for(int j = 0; j<K; j++) {
            struct cell *data = (struct cell *) malloc(sizeof(struct cell));
            data->row = i;
            data->col = j;
            pthread_create(&threads[threadIdx], NULL, runner, data);
            threadIdx++;
            if(threadIdx == nThreads){
                for(int idx = 0; idx < nThreads; idx++){
                    pthread_join(threads[idx], NULL);
                }
                threadIdx = 0;
            }
        }
    }
    for(int i = 0; i < threadIdx; i++){
        pthread_join(threads[i], NULL);
    }
    //printMats();
}

//--- DO NOT MODIFY BELOW HERE ---
int main(int argc, char* argv[])
{
    
    struct timeval start, end;
    double mult_time;

    if(argc != 2) {
        printf("Please provide 1 argument: #threads (int)\n");
        exit(-1);
    }

    int nthreads = atoi(argv[1]);
    
    if(nthreads <= 0){
        printf("Please enter a correct value for #threads\n");
        exit(-1);
    }
    printf("--- Reading in Matrices ---\n");

    mat1 = initialize(M, N, MAT_A);
    mat2 = initialize(N, K, MAT_B);
    matSol = initialize(M, K, MAT_C);
    matRes = malloc(M * K * sizeof(int)); 

    printf("--- Matrices Successfully Read, Multiplying Now ---\n");

    gettimeofday(&start, NULL);
    if(nthreads > 1){
        printf("--- Multiplying with %d threads ---\n", nthreads);
        multiplyWithThreads(nthreads);
    } else {
        printf("--- Multiplying with a single thread ---\n");
        multiply();
    }
    
    gettimeofday(&end, NULL); 
    mult_time = getTotalTime(start, end);

    if(compare()){
        printf("Correct! Your matrices multiplied properly\n");
    } else {
        printf("Incorrect! Your matrices did not multiply properly. Check your solution and try again.\n");
    }
    
    printf("This operation took %f seconds\n", mult_time);

    //Don't forget to free allocated heap memory!
    
    free(mat1);
    free(mat2);
    free(matRes);
    free(matSol);
    
    return 0;
}
