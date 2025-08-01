#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
// These global variables represent fruit quantities modified by threads.
// apples: updated by detached threads (-10)
// bananas: updated by joinable threads (-20)
int apples = 300;
int bananas = 500;

// Mutex to ensure thread-safe access to fruit variables
pthread_mutex_t fruitBasketLock;

// Thread Function
/**
 * Each thread represents a fruit worker.
 * Depending on its type (detached or joinable), it either eats apples or bananas.
 */
void* fruitWorker(void* fruitTypePtr) {
    int fruitMode = *((int*)fruitTypePtr); // 0 = joinable (banana worker), 1 = detached (apple worker)
    free(fruitTypePtr); // Clean up dynamic memory

    pthread_t workerID = pthread_self(); // Get thread ID

    // Secure the fruit basket before taking any fruit
    pthread_mutex_lock(&fruitBasketLock);

    if (fruitMode == 1) {
        // Detached thread: reduces apples by 10
        apples -= 10;
      //  printf("Worker %lu took apples. Remaining apples: %d\n", workerID, apples);
    } else {
        // Joinable thread: reduces bananas by 20
        bananas -= 20;
     //   printf("[Joinable] Worker %lu took bananas. Remaining bananas: %d\n", workerID, bananas);
    }

    // Unlock the basket for others
    pthread_mutex_unlock(&fruitBasketLock);

    return NULL;
}

// Main Function
int main() {
    pthread_t fruitThreads[12];         // Array to store all fruit worker threads
    pthread_attr_t basketPolicy;        // Thread attributes (joinable/detached)

    // Prepare the mutex (fruit basket lock)
    pthread_mutex_init(&fruitBasketLock, NULL);

    // Create 12 workers: alternating between joinable (banana) and detached (apple)
    for (int i = 0; i < 12; i++) {
        int* fruitType = malloc(sizeof(int)); // Dynamic memory to avoid shared value issues
        if (fruitType == NULL) {
            fprintf("Error: Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Assign thread type: even = joinable 🍌, odd = detached 🍎
        *fruitType = (i % 2 == 0) ? 0 : 1;

        // Initialize and set thread attributes
        pthread_attr_init(&basketPolicy);
        if (*fruitType == 1) {
            pthread_attr_setdetachstate(&basketPolicy, PTHREAD_CREATE_DETACHED);
        } else {
            pthread_attr_setdetachstate(&basketPolicy, PTHREAD_CREATE_JOINABLE);
        }

        // Create the fruit worker thread
        if (pthread_create(&fruitThreads[i], &basketPolicy, fruitWorker, fruitType) != 0) {
            perror("Thread creation failed"); //throw an error if it doesnt work
            free(fruitType);
        }

        // Destroy the thread attribute object to clean up
        pthread_attr_destroy(&basketPolicy);
    }

    // Join only the joinable threads (even indices = banana workers)
    for (int i = 0; i < 12; i += 2) {
        pthread_join(fruitThreads[i], NULL);
    }

    // Destroy the mutex (fruit basket is closed)
    pthread_mutex_destroy(&fruitBasketLock);

    // Final fruit inventory after all workers are done
    printf("Final num1 (detached): %d\n", apples);
    printf("Final num2 (joinable): %d\n", bananas);

    return 0;
}

