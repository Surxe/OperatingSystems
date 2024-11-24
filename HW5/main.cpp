#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

sem_t semArr[3];

struct ThreadData {
    int id;
    int work;
};

int main() {
    pthread_t threads[3];
    struct ThreadData thread_data[3]; // Corrected to match struct name

    // Initialize semaphores
    for (int i = 0; i < 3; ++i) {
        sem_init(&semArr[i], 0, 1);  // Binary semaphore
    }

    // Additional thread creation logic would go here...

    return 0;
}
