#define _GNU_SOURCE
#include "shared.h"


// make  file for multiuser
void get_user_task_filename(char *filename) {
    snprintf(filename, 150, "%s_tasks.dat", current_user_name);
}

// --- CORE FUNCTIONS ---

void print_progress_bar() {
    pthread_mutex_lock(&todo_list_mutex);
    
    char bar[200];
    
    if (todo_count == 0) {
        snprintf(bar, sizeof(bar), "Progress: [ No tasks to track ]");
        print_centered(bar, AC_YELLOW);
        pthread_mutex_unlock(&todo_list_mutex);
        return;
    }

    int completed = 0;
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].completed) completed++;
    }

    float percent = (float)completed / todo_count;
    int width = 25; 
    int pos = width * percent;

    
    char bar_content[100] = "";
    for (int i = 0; i < width; ++i) {
        if (i < pos) strcat(bar_content, "=");
        else if (i == pos) strcat(bar_content, ">");
        else strcat(bar_content, " ");
    }
    
    snprintf(bar, sizeof(bar), "Progress: [%s] %d%% Completed", bar_content, (int)(percent * 100));
    
    print_centered(bar, AC_CYAN);
    
    pthread_mutex_unlock(&todo_list_mutex);
}
void add_task() {
    pthread_mutex_lock(&todo_list_mutex);
    if (todo_count >= MAX_TASKS) {
        printf(AC_RED "   Error: Task list full!\n" AC_RESET);
        pthread_mutex_unlock(&todo_list_mutex);
        return;
    }

    TodoItem t;
    t.id = (todo_count == 0) ? 1 : todo_list[todo_count - 1].id + 1;
    pthread_mutex_unlock(&todo_list_mutex);

    printf(AC_BOLD "\n   --- CREATE NEW TASK ---\n" AC_RESET);
    printf("   Title: ");
    fgets(t.title, MAX_TASK_LENGTH, stdin);
    t.title[strcspn(t.title, "\n")] = 0;

    printf("   Description: ");
    fgets(t.description, MAX_DESCRIPTION_LENGTH, stdin);
    t.description[strcspn(t.description, "\n")] = 0;

    printf("   Priority (1=High, 2=Med, 3=Low): ");
    if (scanf("%d", &t.priority) != 1) t.priority = 3;
    clear_stdin_buffer();

    t.due_date = 0; 
    t.completed = false;
    t.has_reminder = false;

    pthread_mutex_lock(&todo_list_mutex);
    todo_list[todo_count++] = t;
    save_tasks_to_file();
    pthread_mutex_unlock(&todo_list_mutex);

    printf(AC_GREEN "   Task added successfully!\n" AC_RESET);
}

void view_tasks() {
    pthread_mutex_lock(&todo_list_mutex);
    clear_screen();
    printf(AC_BOLD AC_MAGENTA "   === %s's TASK BOARD ===\n" AC_RESET, current_user_name);
    
    if (todo_count == 0) {
        printf("\n   " AC_YELLOW "No tasks found." AC_RESET "\n");
    } else {
        printf("\n   %-4s | %-18s | %-6s | %-8s\n", "ID", "Title", "Prio", "Status");
        printf("   ----------------------------------------------\n");

        for (int i = 0; i < todo_count; i++) {
            char *p_color = (todo_list[i].priority == 1) ? AC_RED : (todo_list[i].priority == 2 ? AC_YELLOW : AC_BLUE);
            char *s_color = todo_list[i].completed ? AC_GREEN : AC_RESET;
            char *status = todo_list[i].completed ? "[DONE]" : "[PEND]";

            printf("   %-4d | %-18.18s | %sP-%d" AC_RESET "    | %s%-8s" AC_RESET "\n",
                   todo_list[i].id, todo_list[i].title, p_color, todo_list[i].priority, s_color, status);
        }
    }
    pthread_mutex_unlock(&todo_list_mutex);
    printf("\n   Press Enter to return...");
    getchar();
}

void mark_task_complete() {
    int id;
    printf("\n   Enter Task ID to complete: ");
    if (scanf("%d", &id) != 1) { clear_stdin_buffer(); return; }
    clear_stdin_buffer();

    pthread_mutex_lock(&todo_list_mutex);
    bool found = false;
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            todo_list[i].completed = true;
            found = true;
            save_tasks_to_file();
            printf(AC_GREEN "   Task %d completed!\n" AC_RESET, id);
            break;
        }
    }
    pthread_mutex_unlock(&todo_list_mutex);
    if (!found) printf(AC_RED "   Task ID not found.\n" AC_RESET);
    SLEEP_SEC(1);
}

void delete_task() {
    int id;
    printf("\n   Enter Task ID to DELETE: ");
    if (scanf("%d", &id) != 1) { clear_stdin_buffer(); return; }
    clear_stdin_buffer();

    pthread_mutex_lock(&todo_list_mutex);
    int found_idx = -1;
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        for (int i = found_idx; i < todo_count - 1; i++) {
            todo_list[i] = todo_list[i + 1];
        }
        todo_count--;
        save_tasks_to_file();
        printf(AC_RED "   Task deleted.\n" AC_RESET);
    } else {
        printf("   Task not found.\n");
    }
    pthread_mutex_unlock(&todo_list_mutex);
    SLEEP_SEC(1);
}

// Case-insensitive Substring Search 
void fuzzy_search_tasks() {
    char query[100];
    printf("\n   " AC_CYAN "Enter task name (approximate): " AC_RESET);
    fgets(query, 100, stdin);
    query[strcspn(query, "\n")] = 0;

    char low_query[100];
    for(int i = 0; query[i]; i++) low_query[i] = tolower((unsigned char)query[i]);
    low_query[strlen(query)] = '\0';

    pthread_mutex_lock(&todo_list_mutex);
    printf("\n   --- SEARCH RESULTS ---\n");
    bool found = false;
    
    for (int i = 0; i < todo_count; i++) {
        
        char low_title[MAX_TASK_LENGTH];
        for(int j = 0; todo_list[i].title[j]; j++) 
            low_title[j] = tolower((unsigned char)todo_list[i].title[j]);
        low_title[strlen(todo_list[i].title)] = '\0';

        int dist = levenshtein(query, todo_list[i].title);
        bool is_substring = (strstr(low_title, low_query) != NULL);

        if (dist <= 2 || is_substring) {
            printf("   [Match] ID: %d | Title: %s | Status: %s\n", 
                   todo_list[i].id, todo_list[i].title, 
                   todo_list[i].completed ? "Done" : "Pending");
            found = true;
        }
    }
    pthread_mutex_unlock(&todo_list_mutex);
    
    if (!found) printf("   No similar tasks found.\n");
    printf("\n   Press Enter...");
    getchar();
}

// --- FILE I/O ---

void save_tasks_to_file() {
    char filename[150];
    get_user_task_filename(filename);
    FILE *fp = fopen(filename, "wb");
    if (fp) {
        fwrite(&todo_count, sizeof(int), 1, fp);
        if (todo_count > 0) {
            fwrite(todo_list, sizeof(TodoItem), todo_count, fp);
        }
        fclose(fp);
    }
}

void load_tasks_from_file() {
    char filename[150];
    get_user_task_filename(filename);
    FILE *fp = fopen(filename, "rb");
    
    pthread_mutex_lock(&todo_list_mutex);
    if (fp) {
        if (fread(&todo_count, sizeof(int), 1, fp) != 1) todo_count = 0;
        if (todo_count > 0) {
            fread(todo_list, sizeof(TodoItem), todo_count, fp);
        }
        fclose(fp);
    } else {
        todo_count = 0; 
    }
    pthread_mutex_unlock(&todo_list_mutex);
}