#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

long counter = 0; 

typedef struct {
    int iterations;
} ThreadArgs;

void *increment(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    for (int i = 0; i < threadArgs->iterations; i++) {
        counter++; 
    }
    return NULL;
}

void *decrement(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    for (int i = 0; i < threadArgs->iterations; i++) {
        counter--; 
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = 2; // Кількість пар потоків за замовчуванням
    int iterations = 1000000; // Кількість ітерацій для кожного потоку за замовчуванням

    // Обробка параметрів командного рядка
    int option;
    while ((option = getopt(argc, argv, "t:i:")) != -1) {
        switch (option) {
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'i':
                iterations = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Використання: %s [-t num_threads] [-i iterations]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("Початкове значення змінної: %ld\n", counter);
    printf("Кількість потоків: %d | Операцій на потік: %d\n", num_threads, iterations);

    // Виділення пам'яті для потоків
    pthread_t *threads = malloc(num_threads * 2 * sizeof(pthread_t));
    ThreadArgs args = {iterations};

    // Запуск пар потоків
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i * 2], NULL, increment, &args);
        pthread_create(&threads[i * 2 + 1], NULL, decrement, &args);
    }

    // Очікування завершення всіх потоків
    for (int i = 0; i < num_threads * 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // Виведення фінального значення змінної
    printf("Фінальне значення змінної: %ld\n", counter);
    
    free(threads);

    return 0;
}