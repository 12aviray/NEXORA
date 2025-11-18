#include "shared.h"

// Function Prototypes
void add_task();
void view_tasks();
void mark_task_complete();
void delete_task();
void save_tasks_to_file(const char* filename);
void load_tasks_from_file(const char* filename);

void add_task() {
    if (todo_count >= MAX_TASKS) {
        printf("To-Do list is full.\n");
        return;
    }
    pthread_mutex_lock(&todo_list_mutex);

    TodoItem new_task;
    new_task.id = todo_count + 1;
    new_task.completed = false;
    new_task.has_reminder = false;

    printf("Enter task title: ");
    fgets(new_task.title, sizeof(new_task.title), stdin);
    new_task.title[strcspn(new_task.title, "\n")] = 0;

    printf("Enter description: ");
    fgets(new_task.description, sizeof(new_task.description), stdin);
    new_task.description[strcspn(new_task.description, "\n")] = 0;

    printf("Enter due date (YYYY-MM-DD HH:MM) or blank: ");
    char date_str[50];
    fgets(date_str, sizeof(date_str), stdin);
    date_str[strcspn(date_str, "\n")] = 0;

    if (strlen(date_str) > 0) {
        struct tm tm_info = {0};
        if (strptime(date_str, "%Y-%m-%d %H:%M", &tm_info) != NULL) {
            new_task.due_date = mktime(&tm_info);
        } else new_task.due_date = 0;
    } else new_task.due_date = 0;

    todo_list[todo_count++] = new_task;
    printf("Task added!\n");
    pthread_mutex_unlock(&todo_list_mutex);
}

void view_tasks() {
    pthread_mutex_lock(&todo_list_mutex);
    if (todo_count == 0) printf("No tasks.\n");
    else {
        printf("\n--- To-Do List ---\n");
        for (int i = 0; i < todo_count; i++) {
            printf("ID: %d | Title: %s | Status: %s\n", 
                   todo_list[i].id, todo_list[i].title, 
                   todo_list[i].completed ? "Completed" : "Pending");
        }
    }
    pthread_mutex_unlock(&todo_list_mutex);
}

void mark_task_complete() {
    int id;
    printf("Enter ID to complete: ");
    if (scanf("%d", &id) == 1) {
        clear_stdin_buffer();
        pthread_mutex_lock(&todo_list_mutex);
        bool found = false;
        for (int i = 0; i < todo_count; i++) {
            if (todo_list[i].id == id) {
                todo_list[i].completed = true;
                printf("Task marked complete.\n");
                found = true;
                break;
            }
        }
        if (!found) printf("ID not found.\n");
        pthread_mutex_unlock(&todo_list_mutex);
    } else clear_stdin_buffer();
}

void delete_task() {
    int id;
    printf("Enter ID to delete: ");
    if (scanf("%d", &id) == 1) {
        clear_stdin_buffer();
        pthread_mutex_lock(&todo_list_mutex);
        bool found = false;
        for (int i = 0; i < todo_count; i++) {
            if (todo_list[i].id == id) {
                for (int j = i; j < todo_count - 1; j++) todo_list[j] = todo_list[j + 1];
                todo_count--;
                printf("Task deleted.\n");
                found = true;
                break;
            }
        }
        if (!found) printf("ID not found.\n");
        pthread_mutex_unlock(&todo_list_mutex);
    } else clear_stdin_buffer();
}

void save_tasks_to_file(const char* filename) {
    pthread_mutex_lock(&todo_list_mutex);
    FILE *fp = fopen(filename, "wb");
    if (fp) {
        fwrite(&todo_count, sizeof(int), 1, fp);
        fwrite(todo_list, sizeof(TodoItem), todo_count, fp);
        fclose(fp);
        printf("Tasks saved.\n");
    }
    pthread_mutex_unlock(&todo_list_mutex);
}

void load_tasks_from_file(const char* filename) {
    pthread_mutex_lock(&todo_list_mutex);
    FILE *fp = fopen(filename, "rb");
    if (fp) {
        fread(&todo_count, sizeof(int), 1, fp);
        fread(todo_list, sizeof(TodoItem), todo_count, fp);
        fclose(fp);
        printf("Tasks loaded.\n");
    } else todo_count = 0;
    pthread_mutex_unlock(&todo_list_mutex);
}