#include "shared.h"

// Prototypes from other files
void add_task();
void view_tasks();
void mark_task_complete();
void delete_task();
void save_tasks_to_file(const char* f);
void load_tasks_from_file(const char* f);
void setup_reminder_for_task(int id, time_t t);
void list_reminders();
void add_note();
void view_notes();
void delete_note();
void edit_note();

int main() {
    load_tasks_from_file(TODO_FILE);
    int choice;
    do {
        printf("\n1. Add Task | 2. View | 3. Complete | 4. Delete\n");
        printf("5. Reminder | 6. List Reminders\n");
        printf("7. Add Note | 8. View Notes | 9. Edit | 10. Delete Note\n");
        printf("0. Exit\nSelection: ");
        
        if (scanf("%d", &choice) == 1) {
            clear_stdin_buffer();
            switch (choice) {
                case 1: add_task(); break;
                case 2: view_tasks(); break;
                case 3: mark_task_complete(); break;
                case 4: delete_task(); break;
                case 5: {
                    int id; char date[50];
                    printf("Task ID: "); scanf("%d", &id); clear_stdin_buffer();
                    printf("Time (YYYY-MM-DD HH:MM): "); fgets(date, 50, stdin);
                    struct tm tm={0};
                    if(strptime(date, "%Y-%m-%d %H:%M", &tm)) setup_reminder_for_task(id, mktime(&tm));
                    break;
                }
                case 6: list_reminders(); break;
                case 7: add_note(); break;
                case 8: view_notes(); break;
                case 9: edit_note(); break;
                case 10: delete_note(); break;
                case 0: save_tasks_to_file(TODO_FILE); break;
            }
        } else clear_stdin_buffer();
    } while (choice != 0);
    
    pthread_mutex_destroy(&todo_list_mutex);
    return 0;
}