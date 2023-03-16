#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
typedef struct {
    uint32_t *data_ptr;
    pthread_mutex_t *mutex1;
    pthread_mutex_t *mutex2;
} thread_data_t;

thread_data_t thread_data;

void *thread_1(void *arg) {
    printf("thread1 begins, %u\n", *thread_data.data_ptr);
    while (*thread_data.data_ptr > 1)
    {
        pthread_mutex_lock(thread_data.mutex1);
        (*thread_data.data_ptr)--;
        printf("thread1 bounce %u\n", *thread_data.data_ptr);
        pthread_mutex_unlock(thread_data.mutex1);
        sleep(1);
    }
    return NULL;
}

void *thread_2(void *arg) {
    printf("thread2 begins, %u\n", *thread_data.data_ptr);
    while (*thread_data.data_ptr > 1)
    {
        pthread_mutex_lock(thread_data.mutex1);
        (*thread_data.data_ptr)--;
        printf("thread2 bounce %u\n", *thread_data.data_ptr);
        pthread_mutex_unlock(thread_data.mutex1);
        sleep(1);
    }
    return NULL;
}

int main()
{
    memset(&thread_data, 0, sizeof(thread_data_t));
    int data = 10;
    thread_data.data_ptr = &data;

    thread_data.mutex1 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(thread_data.mutex1, NULL);

    pthread_t thread_1_id;
    pthread_t thread_2_id;
    pthread_create(&thread_1_id, NULL, *thread_1, NULL);
    pthread_create(&thread_2_id, NULL, *thread_2, NULL);
    pthread_join(thread_1_id, NULL);
    pthread_join(thread_2_id, NULL);
    return 0;
}
