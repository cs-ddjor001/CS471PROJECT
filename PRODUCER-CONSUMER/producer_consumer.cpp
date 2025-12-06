#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 10
#define MAX_ITEMS 1000
#define MIN_SLEEP_MS 5
#define MAX_SLEEP_MS 40

struct Sale{
    int day;        
    int month;      
    int year;       
    int store_id;   
    int register_no;
    float amount;
};

struct ConsumerStats{
    int consumer_id;
    float local_total;
    float month_totals[13];
};


struct CircularBuffer{
    Sale buffer[BUFFER_SIZE];
    int in;
    int out;
    int count;
};

/* ---------- Global shared variables ---------- */
CircularBuffer shared_buffer;
sem_t sem_empty, sem_full;
pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_stats = PTHREAD_MUTEX_INITIALIZER;

int total_produced = 0; // total items produced
int done_producing = 0; // flag: producers finished

float global_total = 0.0f;
float global_month_totals[13];

/**
 * @brief Generates a random sale record for a producer
 */
Sale generate_random_sale(int store_id) {
    Sale s;
    s.day = (rand() % 30) + 1;
    s.month = (rand() % 12) + 1;
    s.year = 16;
    s.store_id = store_id;
    s.register_no = (rand() % 6) + 1;
    s.amount = 0.5f + ((float)rand() / RAND_MAX) * (999.99f - 0.5f);
    return s;
}

/**
 * @brief Safely insert an item into the circular buffer
 */
void buffer_put(Sale item) {
    shared_buffer.buffer[shared_buffer.in] = item;
    shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE;
    shared_buffer.count++;
}

/**
 * @brief Safely remove an item from the circular buffer
 */
Sale buffer_get() {
    Sale item = shared_buffer.buffer[shared_buffer.out];
    shared_buffer.out = (shared_buffer.out + 1) % BUFFER_SIZE;
    shared_buffer.count--;
    return item;
}

/* ---------- Thread functions ---------- */

/**
 * @brief Producer thread function
 */
void* producer(void* arg) {
    int store_id = *((int*)arg);

    while (1) {
        pthread_mutex_lock(&mutex_buffer);
        if (total_produced >= MAX_ITEMS) {
            pthread_mutex_unlock(&mutex_buffer);
            break;
        }
        pthread_mutex_unlock(&mutex_buffer);

        Sale s = generate_random_sale(store_id);

        sem_wait(&sem_empty);

        pthread_mutex_lock(&mutex_buffer);
        buffer_put(s);
        total_produced++;
        pthread_mutex_unlock(&mutex_buffer);

        sem_post(&sem_full);

        int sleep_ms = MIN_SLEEP_MS + rand() % (MAX_SLEEP_MS - MIN_SLEEP_MS + 1);
        usleep(sleep_ms * 1000);
    }

    return NULL;
}

/**
 * @brief Consumer thread function
 */
void* consumer(void* arg) {
    int cid = *((int*)arg);

    ConsumerStats stats;
    stats.consumer_id = cid;
    stats.local_total = 0.0f;
    memset(stats.month_totals, 0, sizeof(stats.month_totals));

    while (1) {
        sem_wait(&sem_full);

        pthread_mutex_lock(&mutex_buffer);
        if (shared_buffer.count > 0) {
            Sale s = buffer_get();
            pthread_mutex_unlock(&mutex_buffer);
            sem_post(&sem_empty);

            stats.local_total += s.amount;
            stats.month_totals[s.month] += s.amount;

        } else if (done_producing) {
            pthread_mutex_unlock(&mutex_buffer);
            sem_post(&sem_full); // wake other consumers
            break;

        } else {
            pthread_mutex_unlock(&mutex_buffer);
            sem_post(&sem_full); // spurious wakeup
        }
    }

    pthread_mutex_lock(&mutex_stats);
    global_total += stats.local_total;
    for (int i = 1; i <= 12; i++) {
        global_month_totals[i] += stats.month_totals[i];
    }
    pthread_mutex_unlock(&mutex_stats);

    printf("Consumer %d local total: %.2f\n", cid, stats.local_total);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int P = atoi(argv[1]);
    int C = atoi(argv[2]);

    srand(time(NULL));

    shared_buffer.in = 0;
    shared_buffer.out = 0;
    shared_buffer.count = 0;

    sem_init(&sem_empty, 0, BUFFER_SIZE);
    sem_init(&sem_full, 0, 0);
    pthread_mutex_init(&mutex_buffer, NULL);
    pthread_mutex_init(&mutex_stats, NULL);

    memset(global_month_totals, 0, sizeof(global_month_totals));
    global_total = 0.0f;

    pthread_t producers[P], consumers[C];
    int producer_ids[P], consumer_ids[C];

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    // Launch producers
    for (int i = 0; i < P; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Launch consumers
    for (int i = 0; i < C; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Join producers
    for (int i = 0; i < P; i++) pthread_join(producers[i], NULL);

    // Signal that production is done
    pthread_mutex_lock(&mutex_buffer);
    done_producing = 1;
    pthread_mutex_unlock(&mutex_buffer);

    // Wake all consumers
    for (int i = 0; i < C; i++) sem_post(&sem_full);

    // Join consumers
    for (int i = 0; i < C; i++) pthread_join(consumers[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec)/1e9;

    printf("\n=== Global Statistics ===\n");
    printf("Aggregate total: %.2f\n", global_total);
    printf("Month totals:\n");
    for (int i = 1; i <= 12; i++) {
        printf("  Month %02d : %.2f\n", i, global_month_totals[i]);
    }
    printf("Total simulation time: %.3f seconds\n", elapsed);

    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    pthread_mutex_destroy(&mutex_buffer);
    pthread_mutex_destroy(&mutex_stats);

    return 0;
}
