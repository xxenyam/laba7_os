#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Структура для зберігання даних інтеграції для кожного потоку
typedef struct {
    double a;                
    double b;                
    int n;                   
    double precision;        
    double result;           
    pthread_mutex_t *mutex; 
} IntegrationData;


double function(double x) {
    return 4 - x * x; 
}

// Функція для інтегрування сегмента за методом трапецій
double integrate_segment(double a, double b, int n) {
    double h = (b - a) / n;   
    double sum = 0.0;          
    for (int i = 0; i < n; i++) {
        double x = a + i * h + h / 2; 
        sum += function(x);           
    }
    return sum * h;   
}

// Функція, що буде виконуватися в кожному потоці для інтегрування сегмента
void* thread_integrate(void* arg) {
    IntegrationData* data = (IntegrationData*) arg; 
    double local_result = integrate_segment(data->a, data->b, data->n); 
    
    pthread_mutex_lock(data->mutex);      
    data->result += local_result;        
    pthread_mutex_unlock(data->mutex);    
    
    return NULL;
}

// Основна функція для інтегрування з використанням кількох потоків
double integrate(double a, double b, double precision, int p) {
    pthread_t threads[p];                 
    pthread_mutex_t mutex;                
    pthread_mutex_init(&mutex, NULL);     
    IntegrationData thread_data[p];       
    double result = 0.0;                  
    
    double h = (b - a) / p;               
    int n = 10;                           
    
    // Створюємо потоки для паралельного обчислення інтегралів
    for (int i = 0; i < p; i++) {
        thread_data[i].a = a + i * h;          
        thread_data[i].b = thread_data[i].a + h; 
        thread_data[i].n = n;                   
        thread_data[i].precision = precision;   
        thread_data[i].result = 0.0;            
        thread_data[i].mutex = &mutex;          
        
        // Створюємо потік для виконання інтеграції
        if (pthread_create(&threads[i], NULL, thread_integrate, &thread_data[i]) != 0) {
            perror("Помилка створення потоку");
            exit(1);
        }
    }
    
    // Чекаємо на завершення всіх потоків
    for (int i = 0; i < p; i++) {
        pthread_join(threads[i], NULL); 
        result += thread_data[i].result; 
    }
    
    pthread_mutex_destroy(&mutex); 
    return result; 
}

// Основна функція для отримання параметрів і виклику інтеграції
int main(int argc, char *argv[]) {
    double a = 0;             
    double b = 2;             
    double precision = 0.0001; 
    int p = 4;                

    if (argc > 1) a = atof(argv[1]);
    if (argc > 2) b = atof(argv[2]);
    if (argc > 3) precision = atof(argv[3]);
    if (argc > 4) p = atoi(argv[4]);


    double result = integrate(a, b, precision, p);
    printf("Наближене значення інтегралу: %f\n", result); 
    
    return 0;
}