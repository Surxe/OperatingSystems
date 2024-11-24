#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

#define NUM_THREADS 4
#define NUM_SEMAPHORES 3
#define WORK_LOAD 10

sem_t semaphores[NUM_SEMAPHORES];  // Array of semaphores

// Thread data structure
struct ThreadData {
    int id;
    int work;
};

// Function to simulate random delay (0-10 ms)
void random_sleep() {
    int sleep_time = rand() % 11;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

// Function to acquire multiple semaphores
bool acquire_semaphores(std::vector<int>& needed_semaphores) {
    // Try to lock all semaphores, release if any fail
    for (int sem_id : needed_semaphores) {
        if (sem_trywait(&semaphores[sem_id]) != 0) {  // Check if semaphore is available
            // Release any semaphores already acquired
            for (int released_sem : needed_semaphores) {
                if (released_sem == sem_id) break;
                sem_post(&semaphores[released_sem]);
            }
            return false;
        }
    }
    return true;
}

// Function for thread work
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    srand(time(nullptr) + data->id);  // Seed the random generator for each thread

    while (data->work > 0) {
        int num_semaphores_needed = rand() % NUM_SEMAPHORES + 1;  // Random number between 1-3
        std::vector<int> needed_semaphores;

        // Choose unique semaphores
        while (needed_semaphores.size() < num_semaphores_needed) {
            int sem_id = rand() % NUM_SEMAPHORES;
            if (std::find(needed_semaphores.begin(), needed_semaphores.end(), sem_id) == needed_semaphores.end()) {
                needed_semaphores.push_back(sem_id);
            }
        }

        if (acquire_semaphores(needed_semaphores)) {
            for (int sem_id : needed_semaphores) {
                std::cout << "Thread " << data->id << " has obtained semaphore " << sem_id << std::endl;
            }

            std::cout << "Doing work on thread " << data->id << ", " << data->work - 1 << " work left." << std::endl;
            data->work--;

            // Release semaphores
            for (int sem_id : needed_semaphores) {
                std::cout << "Thread " << data->id << " is releasing semaphore " << sem_id << std::endl;
                sem_post(&semaphores[sem_id]);
            }

            random_sleep();
        } else {
            std::cout << "Thread " << data->id << " could not obtain required semaphores. Retrying..." << std::endl;
            random_sleep();
        }
    }
    pthread_exit(nullptr);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // Initialize semaphores
    for (int i = 0; i < NUM_SEMAPHORES; ++i) {
        sem_init(&semaphores[i], 0, 1);  // Binary semaphore
    }

    // Seed the random number generator
    srand(time(nullptr));

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].id = i;
        thread_data[i].work = WORK_LOAD;
        pthread_create(&threads[i], nullptr, thread_function, &thread_data[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Destroy semaphores
    for (int i = 0; i < NUM_SEMAPHORES; ++i) {
        sem_destroy(&semaphores[i]);
    }

    std::cout << "All threads completed successfully!" << std::endl;
    return 0;
}
