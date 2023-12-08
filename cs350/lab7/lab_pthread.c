#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "-lpthread"
#include <unistd.h>
#include <string.h>

long currentCounter = 0;
int total = 0; // to record total number of messages from the two threads
pthread_t t1, t2;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * thread_work1(void * arg) {
    char *userArgs = arg;
    long self_counter = 0; // to record how many times printed by the current thread
    while(currentCounter < total - 1){
      //usleep(50);
      pthread_mutex_lock(&lock);
      printf("%d: In thread 1 (TID:%u): ", (int) currentCounter + 1, (unsigned int) t1);
      for(int i = 0; i < strlen(userArgs); i++){
        printf("%c ", userArgs[i]);
      }
      printf("\n");
      currentCounter++;
      self_counter++;
      pthread_mutex_unlock(&lock);
    }
    return (void *)self_counter;
}

void * thread_work2(void * arg) {
    char *userArgs = arg;
    long self_counter = 0; // to record how many times printed by the current thread
    while(currentCounter < total - 1){
      //usleep(100);
      pthread_mutex_lock(&lock);
      printf("%d: In thread 2 (TID:%u): ", (int) currentCounter + 1, (unsigned int) t2);
      for(int i = 0; i < strlen(userArgs); i++){
        printf("%c ", userArgs[strlen(userArgs) - 1 - i]);
      }
      printf("\n");
      currentCounter++;
      self_counter++;
      pthread_mutex_unlock(&lock);
    }
    return (void *)self_counter;
}

int main(int argc, char * argv[]) {
    long t1_c = 0, t2_c = 0; // to record numbers of times printed by T1 and T2

    if (argc < 3) {
        printf("There must be 2 arguements following the binary!\n");
        return 0;
    }

    total = atoi(argv[1]);

    char * arr = malloc(sizeof(char)*(argc - 2));
    for(int i = 0; i < argc - 2; i++){
      arr[i] = argv[i+2][0];
    }
    
    pthread_create(&t1, NULL, thread_work1, arr);
    pthread_create(&t2, NULL, thread_work2, arr);
    pthread_join(t1, (void *)&t1_c);
    pthread_join(t2, (void *)&t2_c);
    
    printf("In main thread: T1 printed %ld times. T2 printed %ld times.\n", t1_c, t2_c);
    
    return 0;
}


