#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include<unistd.h>
 int criticalWriters=0;
int NO_READERS;
int NO_WRITERS;
int NO_READERS_READING = 0;     // How many readers need shared resources
int NO_WRITERS_WRITING = 0;     // How many writers need shared resources

pthread_mutex_t resourceMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tryResourceMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writerMutex = PTHREAD_MUTEX_INITIALIZER;

void *readerJob(void *arg) {
    int *id = (int*)arg;
    while (1) {
        pthread_mutex_lock(&tryResourceMutex); // Indicate reader is trying to enter
        pthread_mutex_lock(&readerMutex);
        NO_READERS_READING++; // Indicate that you are needing the shared resource (one more reader)
        if (NO_READERS_READING == 1) {
            pthread_mutex_lock(&resourceMutex);
        }
        pthread_mutex_unlock(&readerMutex);
        pthread_mutex_unlock(&tryResourceMutex);
         printf("READER ID %d WALKED IN \n",*id);
        printf("ReaderQ: %d , WriterQ: %d [in: R:%d W:%d]\n",NO_READERS-NO_READERS_READING,NO_WRITERS-NO_WRITERS_WRITING,NO_READERS_READING,NO_WRITERS_WRITING);
        sleep(2);
        pthread_mutex_lock(&readerMutex);
        NO_READERS_READING--;
        if (NO_READERS_READING == 0) { // Check if you are the last reader
            pthread_mutex_unlock(&resourceMutex);
        }
        pthread_mutex_unlock(&readerMutex);
    }
    return 0;
}

void *writerJob(void *arg) {
    int *id = (int*)arg;
    while (1) {
        pthread_mutex_lock(&writerMutex);
        NO_WRITERS_WRITING++;
        if (NO_WRITERS_WRITING == 1) {
            pthread_mutex_lock(&tryResourceMutex); // If there are no other writers lock the readers out
        }
        pthread_mutex_unlock(&writerMutex);

        pthread_mutex_lock(&resourceMutex);
        criticalWriters++;
         printf("WRITER ID %d WALKED IN \n",*id);
        printf("ReaderQ: %d , WriterQ: %d [in: R:%d W:%d]\n",NO_READERS-NO_READERS_READING,NO_WRITERS-criticalWriters,NO_READERS_READING,criticalWriters);
        sleep(2);
        pthread_mutex_unlock(&resourceMutex);
        criticalWriters--;


        pthread_mutex_lock(&writerMutex);
        NO_WRITERS_WRITING--;
        if (NO_WRITERS_WRITING == 0) {
            pthread_mutex_unlock(&tryResourceMutex); // If there are no writers left unlock the readers
        }
        pthread_mutex_unlock(&writerMutex);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    NO_READERS = atoi(argv[1]);
    NO_WRITERS = atoi(argv[2]);

    // Initialize arrays of threads IDs
    pthread_t *readersThreadsIds = malloc(NO_READERS * sizeof(pthread_t));
    pthread_t *writersThreadsIds = malloc(NO_READERS * sizeof(pthread_t));

    // Initialize shared memory (array) with random numbers

    // Create readers threads
    for (int i = 0; i < NO_READERS; ++i) {
        int* id = (int*)(malloc(sizeof(int)));
        *id = i;
        pthread_create(&readersThreadsIds[i], NULL, readerJob,(void*)id);
    }
    // Create writers threads
    for (int i = 0; i < NO_WRITERS; ++i) {
        int* id = (int*)(malloc(sizeof(int)));
        *id = i;
        pthread_create(&writersThreadsIds[i], NULL, writerJob, (void*)id);
    
    }

    // Wait for readers to finish
    for (int i = 0; i < NO_READERS; ++i) {
        pthread_join(readersThreadsIds[i], NULL);
        }
    // Wait for writers to finish
    for (int i = 0; i < NO_WRITERS; ++i) {
        pthread_join(writersThreadsIds[i], NULL);
    }

    free(readersThreadsIds);
    free(writersThreadsIds);
    pthread_mutex_destroy(&resourceMutex);
    pthread_mutex_destroy(&tryResourceMutex);
    pthread_mutex_destroy(&readerMutex);
    pthread_mutex_destroy(&writerMutex);
    return 0;
}
