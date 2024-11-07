#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define DEFAULT_ARRAY_SIZE 10
#define MAX_READERS_WRITERS 8 
#define MAX_RUN_TIME 5 

int *array;
int array_size = DEFAULT_ARRAY_SIZE;
pthread_rwlock_t rwlock;
int run_time = 1; 

void* reader_writer(void* arg) {
    int id = *((int*)arg);
    while (run_time) {
        int index = rand() % array_size;

      
        if (id % 2 == 0) {
            // Читач
            pthread_rwlock_rdlock(&rwlock);
            printf("Потік-читач %d зчитав значення %d з індексу %d\n", id, array[index], index);
            pthread_rwlock_unlock(&rwlock);
        } else {
            // Письменник
            int value = rand() % 1000;
            pthread_rwlock_wrlock(&rwlock);
            array[index] = value;
            printf("Потік-письменник %d записав значення %d в індекс %d\n", id, value, index);
            pthread_rwlock_unlock(&rwlock);
        }

        usleep(rand() % 1000000); 
    }
    return NULL;
}

void* monitor(void* arg) {
    int elapsed_time = 0;
    while (elapsed_time < MAX_RUN_TIME) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Стан масиву: ");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
        pthread_rwlock_unlock(&rwlock);

        sleep(5); 
        elapsed_time += 5;
    }
    run_time = 0; 
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        array_size = atoi(argv[1]);
    }

    array = (int*)malloc(array_size * sizeof(int));
    if (array == NULL) {
        perror("Не вдалося виділити пам'ять для масиву");
        return 1;
    }

    for (int i = 0; i < array_size; i++) {
        array[i] = rand() % 1000;
    }

    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        perror("Не вдалося ініціалізувати блокування");
        free(array);
        return 1;
    }

    pthread_t threads[MAX_READERS_WRITERS], monitor_thread;
    int thread_ids[MAX_READERS_WRITERS];

    // Створення потоків читачів і письменників
    for (int i = 0; i < MAX_READERS_WRITERS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, reader_writer, &thread_ids[i]) != 0) {
            perror("Не вдалося створити потік");
            free(array);
            pthread_rwlock_destroy(&rwlock);
            return 1;
        }
    }

    // Створення моніторного потоку
    if (pthread_create(&monitor_thread, NULL, monitor, NULL) != 0) {
        perror("Не вдалося створити моніторний потік");
        free(array);
        pthread_rwlock_destroy(&rwlock);
        return 1;
    }

    // Чекаємо завершення всіх потоків
    for (int i = 0; i < MAX_READERS_WRITERS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(monitor_thread, NULL);

    pthread_rwlock_destroy(&rwlock);
    free(array);
    return 0;
}