#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define MAX_TRAINS 100

typedef struct Train {
    int id;
    char direction; 
    int priority;  
    int loading_time;
    int crossing_time;
    double ready_time;
    struct Train *next; 
} Train;

typedef struct TrainQueue {
    Train *head;
    Train *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TrainQueue;

TrainQueue east_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
TrainQueue west_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

pthread_mutex_t track_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t controller_cond = PTHREAD_COND_INITIALIZER;

Train trains[MAX_TRAINS];
int train_count = 0;
int last_direction = 'N';
int east_waiting_high = 0, west_waiting_high = 0;
int consecutive_crossings = 0;
double start_time;
FILE *output_file;
int train_ready = 0;

void sort_queue(TrainQueue *queue) {
    if (queue->head == NULL || queue->head->next == NULL) {
        // No need to sort if the queue is empty or has only one element
        return;
    }

    Train *priority_1_head = NULL, *priority_1_tail = NULL;
    Train *non_priority_head = NULL, *non_priority_tail = NULL;
    Train *current = queue->head;

    // Separate priority 1 and non-priority 1 trains
    while (current != NULL) {
        Train *next_train = current->next;  // Save the next train
        current->next = NULL;  // Detach the current train from the list

        if (current->priority == 1) {
            // Append to priority 1 list
            if (priority_1_tail) {
                priority_1_tail->next = current;
            } else {
                priority_1_head = current;
            }
            priority_1_tail = current;
        } else {
            // Append to non-priority list
            if (non_priority_tail) {
                non_priority_tail->next = current;
            } else {
                non_priority_head = current;
            }
            non_priority_tail = current;
        }

        current = next_train;  // Move to the next train
    }

    // Merge priority 1 list with non-priority list
    if (priority_1_tail) {
        priority_1_tail->next = non_priority_head;
        queue->head = priority_1_head;
    } else {
        // The non-priority list is the full list
        queue->head = non_priority_head;
    }

    // Update the tail of the queue
    queue->tail = non_priority_tail ? non_priority_tail : priority_1_tail;
}


// Queue management functions
void enqueue_train(TrainQueue *queue, Train *train) {
    pthread_mutex_lock(&queue->mutex);
    train->next = NULL;
    if (queue->tail) {
        queue->tail->next = train;
    } else {
        queue->head = train;
    }
    queue->tail = train;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

Train *dequeue_train(TrainQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->head == NULL) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    
    Train *train = queue->head;
    queue->head = queue->head->next;
    
    if (queue->head == NULL) {
        queue->tail = NULL;  // Queue is now empty, reset tail as well
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return train;
}


// Helper functions for timing and logging
double get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

void format_elapsed_time(char *buffer, double elapsed_time) {
    int hours = (int)(elapsed_time / 3600);
    int minutes = (int)((elapsed_time - hours * 3600) / 60);
    double seconds = elapsed_time - hours * 3600 - minutes * 60;
    snprintf(buffer, 20, "%02d:%02d:%04.1f", hours, minutes, seconds);
}

void print_status(const char *status, int id, const char *direction) {
    char time_str[20];
    double elapsed_time = get_current_time() - start_time;
    format_elapsed_time(time_str, elapsed_time);
    fprintf(output_file, "%s Train %2d %s %s\n", time_str, id, status, direction);
    fflush(output_file);
}

// Load trains from input file
void load_trains(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    char direction;
    int loading_time, crossing_time;
    while (fscanf(file, " %c %d %d", &direction, &loading_time, &crossing_time) != EOF) {
        Train *train = &trains[train_count];
        train->id = train_count;
        train->direction = direction;
        train->priority = (direction == 'E' || direction == 'W') ? 1 : 0;
        train->loading_time = loading_time;
        train->crossing_time = crossing_time;
        train_count++;
    }
    fclose(file);
}

// Controller thread function to manage track access
void *controller_thread(void *arg) {
    int trains_crossed = 0;

    while (trains_crossed < train_count) {
        pthread_mutex_lock(&track_mutex);

        // Wait until there’s a train ready to cross
        while (!train_ready || (east_queue.head == NULL && west_queue.head == NULL)) {
            pthread_cond_wait(&controller_cond, &track_mutex);
        }

        Train *selected_train = NULL;

        // Make sure that if trains are loaded at the same time they are both considered at start
        usleep(5000);

        // Sort and select trains as per your priority rules
        sort_queue(&east_queue);
        sort_queue(&west_queue);

        // Select a train using priority and starvation-prevention rules


        if (consecutive_crossings >= 2) {
            // Switch direction preference to prevent starvation
            if (last_direction == 'E' || last_direction == 'e') {
                // Prefer a train from the west if available
                if (west_queue.head) {
                    selected_train = dequeue_train(&west_queue);
                    west_waiting_high -= (selected_train->priority == 1);
                }
            } else if (last_direction == 'W' || last_direction == 'w') {
                // Prefer a train from the east if available
                if (east_queue.head) {
                    selected_train = dequeue_train(&east_queue);
                    east_waiting_high -= (selected_train->priority == 1);
                }
            }

            // If no train from the opposite direction is ready, continue with the current direction
            if (!selected_train) {
                consecutive_crossings = 0; // Reset consecutive crossings count
            }
        }

        if (!selected_train) {
            // Only easy queue has a train ready
            if (east_queue.head && !west_queue.head) {
                selected_train = dequeue_train(&east_queue);
                east_waiting_high -= (selected_train->priority == 1);

            // Only west queue has a train ready
            } else if (west_queue.head && !east_queue.head) {
                selected_train = dequeue_train(&west_queue);
                west_waiting_high -= (selected_train->priority == 1);


            // Both queues have a train ready
            } else if (west_queue.head && east_queue.head) {
                Train *east_train = east_queue.head;
                Train *west_train = west_queue.head;

                // East train has higher priority than west train
                if (east_train->priority > west_train->priority) {
                    selected_train = dequeue_train(&east_queue);
                    east_waiting_high--;

                // West train has higher priority than east train    
                } else if (west_train->priority > east_train->priority) {
                    selected_train = dequeue_train(&west_queue);
                    west_waiting_high--;

                // Both queues have same priority    
                } else {

                    // Last train that crossed was from west queue so let east queue cross
                    // If this is the first train go west
                    if (last_direction == 'E' || last_direction == 'e' || last_direction == 'N') {
                        selected_train = dequeue_train(&west_queue);
                    }
                    else {
                        selected_train = dequeue_train(&east_queue);
                    }
                    
                }
            }
        }

        if (selected_train) {
            // Cross the train and print status
            print_status("is ON the main track going", selected_train->id, selected_train->direction == 'E' || selected_train->direction == 'e' ? "East" : "West");
            usleep(selected_train->crossing_time * 100000);
            print_status("is OFF the main track after going", selected_train->id, selected_train->direction == 'E' || selected_train->direction == 'e' ? "East" : "West");

            // Update last direction and crossing statistics
            consecutive_crossings = (last_direction == selected_train->direction) ? consecutive_crossings + 1 : 1;
            last_direction = selected_train->direction;
            //consecutive_crossings = (last_direction == selected_train->direction) ? consecutive_crossings + 1 : 1;
            trains_crossed++;
        }

        // Update train_ready based on queue states and recheck for train readiness
        train_ready = (east_queue.head != NULL || west_queue.head != NULL);
        pthread_mutex_unlock(&track_mutex);
    }
    return NULL;
}

void *train_thread(void *arg) {
    Train *train = (Train *)arg;
    usleep(train->loading_time * 100000);
    
    // Signal readiness after loading
    print_status("is ready to go", train->id, (train->direction == 'E' || train->direction == 'e') ? "East" : "West");

    if (train->direction == 'E' || train->direction == 'e') {
        enqueue_train(&east_queue, train);
    } else {
        enqueue_train(&west_queue, train);
    }

    // Set train_ready to 1 if a train is added and signal controller
    pthread_mutex_lock(&track_mutex);
    train_ready = 1;
    pthread_cond_signal(&controller_cond);
    pthread_mutex_unlock(&track_mutex);
    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Failed to open output.txt");
        exit(EXIT_FAILURE);
    }
    start_time = get_current_time();
    load_trains(argv[1]);

    // Create the controller thread and start looping
    pthread_t controller;
    pthread_create(&controller, NULL, controller_thread, NULL);

    // Create all the train threads
    pthread_t train_threads[MAX_TRAINS];
    for (int i = 0; i < train_count; i++) {
        pthread_create(&train_threads[i], NULL, train_thread, &trains[i]);
    }
    for (int i = 0; i < train_count; i++) {
        pthread_join(train_threads[i], NULL);
    }
    pthread_join(controller, NULL);

    fclose(output_file);
    return 0;
}
