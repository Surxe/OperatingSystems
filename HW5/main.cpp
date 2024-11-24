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
#include <cstdlib>
#include <ctime>

#define NUM_THREADS 4
#define NUM_SEMAPHORES 3
#define WORK_LOAD 10

sem_t semaphores[NUM_SEMAPHORES];  // Array of semaphores

struct ThreadData {
    int id;
    int work;
};

// Delay thread randomly
void random_sleep() {
    int sleep_time = rand() % 11;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

// Acquire multiple semaphores
bool acquire_semaphores(std::vector<int>& needed_semaphores) {
    // Lock all semas, release if any fail
    for (int sem_id : needed_semaphores) {
        if (sem_trywait(&semaphores[sem_id]) != 0) {  // Check if sema is avail
            // Release acquired semas
            for (int released_sem : needed_semaphores) {
                if (released_sem == sem_id) break;
                sem_post(&semaphores[released_sem]);
            }
            return false;
        }
    }
    return true;
}

// The function that each thread runs
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    srand(time(nullptr) + data->id);  // Seed the RNG again

    while (data->work > 0) {
        int num_semaphores_needed = rand() % NUM_SEMAPHORES + 1;  // [1,2,3]
        std::vector<int> needed_semaphores;

        // Choose unique semas
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

            std::cout << "Doing work on thread " << data->id << ", " << data->work - 1 << " work left.\n";
            data->work--;

            // Release semas
            for (int sem_id : needed_semaphores) {
                std::cout << "Thread " << data->id << " is releasing semaphore " << sem_id << std::endl;
                sem_post(&semaphores[sem_id]);
            }

            random_sleep();
        } else {
            std::cout << "Thread " << data->id << " could not obtain required semaphores. Retrying...\n";
            random_sleep();
        }
    }
    pthread_exit(nullptr);
}

int main() {
    // Seed the RNG
    srand(time(nullptr));

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // Initialize semas
    for (int i = 0; i < NUM_SEMAPHORES; ++i) {
        sem_init(&semaphores[i], 0, 1);  // Binary sema
    }

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

    // Destroy semas
    for (int i = 0; i < NUM_SEMAPHORES; ++i) {
        sem_destroy(&semaphores[i]);
    }

    std::cout << "All threads completed successfully!\n";
    return 0;
}
