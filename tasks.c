#include "shared.h"

void add_task();
void view_tasks();
void mark_task_complete();
void delete_task();
void save_tasks_to_file(const char* filename);
void load_tasks_from_file(const char* filename);

// Helper to print a visual progress bar
void print_progress_bar() {
    if (todo_count == 0) return;
    int completed = 0;
    for(int i=0; i<todo_count; i++) if(todo_list[i].completed) completed++;
    
    float percent = (float)completed / todo_count;
    int width = 30;
    int pos = width * percent;

    printf("\n" AC_CYAN "   Progress: [" AC_GREEN);
    for (int i = 0; i < width; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf(AC_CYAN "] %d%% Completed" AC_RESET "\n", (int)(percent * 100));
}

void add_task() {
    if (todo_count >= MAX_TASKS) {
        printf(AC_RED "   Full!\n" AC_RESET);
        return;
    }
    pthread_mutex_lock(&todo_list_mutex);

    TodoItem new_task;
    new_task.id = (todo_count == 0) ? 1 : todo_list[todo_count - 1].id + 1;
    new_task.completed = false;
    new_task.has_reminder = false;

    printf(AC_BOLD "\n   [ ADD NEW TASK ]\n" AC_RESET);
    
    printf("   Title: ");
    fgets(new_task.title, sizeof(new_task.title), stdin);
    new_task.title[strcspn(new_task.title, "\n")] = 0;

    printf("   Description: ");
    fgets(new_task.description, sizeof(new_task.description), stdin);
    new_task.description[strcspn(new_task.description, "\n")] = 0;

    // Priority Selection
    printf("   Priority (1=High, 2=Med, 3=Low): ");
    int p;
    if (scanf("%d", &p) == 1) new_task.priority = p;
    else new_task.priority = 3; // Default Low
    clear_stdin_buffer();

    printf("   Due Date (YYYY-MM-DD HH:MM) or Enter to skip: ");
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
    printf(AC_GREEN "   Task added successfully! Press Enter..." AC_RESET);
    getchar();
    pthread_mutex_unlock(&todo_list_mutex);
}

void view_tasks() {
    pthread_mutex_lock(&todo_list_mutex);
    clear_screen();
    printf(AC_BOLD AC_MAGENTA "\n   === YOUR TODO LIST ===\n" AC_RESET);
    
    print_progress_bar();
    printf("\n");

    if (todo_count == 0) {
        printf(AC_YELLOW "   No tasks found. Go add some!\n" AC_RESET);
    } else {
        // Table Header
        printf(AC_BOLD "   %-4s | %-20s | %-8s | %-10s | %s\n" AC_RESET, "ID", "Title", "Priority", "Status", "Due Date");
        printf("   ----------------------------------------------------------------\n");
        
        for (int i = 0; i < todo_count; i++) {
            char date_buf[20] = "--";
            if (todo_list[i].due_date > 0) {
                struct tm *t = localtime(&todo_list[i].due_date);
                strftime(date_buf, 20, "%m-%d %H:%M", t);
            }

            char *prio_str;
            char *prio_color;
            if (todo_list[i].priority == 1) { prio_str = "HIGH"; prio_color = AC_RED; }
            else if (todo_list[i].priority == 2) { prio_str = "MED"; prio_color = AC_YELLOW; }
            else { prio_str = "LOW"; prio_color = AC_BLUE; }

            char *status = todo_list[i].completed ? "[DONE]" : "[ ]";
            char *status_color = todo_list[i].completed ? AC_GREEN : AC_RESET;

            printf("   %-4d | %-20s | %s%-8s" AC_RESET " | %s%-10s" AC_RESET " | %s\n", 
                   todo_list[i].id, 
                   todo_list[i].title, 
                   prio_color, prio_str, 
                   status_color, status,
                   date_buf);
        }
    }
    printf("\n   Press Enter to return...");
    getchar();
    pthread_mutex_unlock(&todo_list_mutex);
}

void mark_task_complete() {
    int id;
    printf("   Enter ID to complete: ");
    if (scanf("%d", &id) == 1) {
        clear_stdin_buffer();
        pthread_mutex_lock(&todo_list_mutex);
        for (int i = 0; i < todo_count; i++) {
            if (todo_list[i].id == id) {
                todo_list[i].completed = true;
                printf(AC_GREEN "   Task '%s' Marked Completed!\n" AC_RESET, todo_list[i].title);
                break;
            }
        }
        pthread_mutex_unlock(&todo_list_mutex);
    } else clear_stdin_buffer();
    printf("   Press Enter..."); getchar();
}

void delete_task() {
    int id;
    printf(AC_RED "   Enter ID to DELETE: " AC_RESET);
    if (scanf("%d", &id) == 1) {
        clear_stdin_buffer();
        pthread_mutex_lock(&todo_list_mutex);
        int found = 0;
        for (int i = 0; i < todo_count; i++) {
            if (todo_list[i].id == id) {
                for (int j = i; j < todo_count - 1; j++) todo_list[j] = todo_list[j + 1];
                todo_count--;
                found = 1;
                break;
            }
        }
        if(found) printf(AC_RED "   Task Deleted.\n" AC_RESET);
        else printf("   Not Found.\n");
        pthread_mutex_unlock(&todo_list_mutex);
    } else clear_stdin_buffer();
    printf("   Press Enter..."); getchar();
}

void save_tasks_to_file(const char* filename) {
    pthread_mutex_lock(&todo_list_mutex);
    FILE *fp = fopen(filename, "wb");
    if (fp) {
        fwrite(&todo_count, sizeof(int), 1, fp);
        fwrite(todo_list, sizeof(TodoItem), todo_count, fp);
        fclose(fp);
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
    }
    pthread_mutex_unlock(&todo_list_mutex);
}