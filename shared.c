#include "shared.h"

TodoItem todo_list[MAX_TASKS];
int todo_count = 0;
pthread_mutex_t todo_list_mutex = PTHREAD_MUTEX_INITIALIZER;
char current_user_name[100] = "";

void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen() {
    // Detects OS to run correct clear command
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}