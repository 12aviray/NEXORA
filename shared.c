#include "shared.h"

// Define Global Variables
TodoItem todo_list[MAX_TASKS];
int todo_count = 0;
pthread_mutex_t todo_list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Helper Function
void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}