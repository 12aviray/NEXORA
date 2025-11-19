#include "shared.h"

// Prototypes
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

// Console Login
void login_screen() {
    clear_screen();
    printf("\n\n");
    printf(AC_BOLD AC_CYAN "   #######################################\n");
    printf("   #           NEXORA LOGIN              #\n");
    printf("   #######################################\n" AC_RESET);
    printf("\n");
    
    printf("   Enter Name: ");
    fgets(current_user_name, 99, stdin);
    current_user_name[strcspn(current_user_name, "\n")] = 0;

    if (strlen(current_user_name) == 0) strcpy(current_user_name, "User");
    
    printf(AC_GREEN "\n   Welcome, %s! Loading your workspace...\n" AC_RESET, current_user_name);
    sleep(1);
}

void show_menu() {
    clear_screen();
    printf(AC_BOLD AC_CYAN "   NEXORA | Task Manager | User: %s\n" AC_RESET, current_user_name);
    printf("   ========================================\n");
    
    // Simple Stats
    int pending = 0;
    for(int i=0; i<todo_count; i++) if(!todo_list[i].completed) pending++;
    printf("   Pending Tasks: " AC_RED "%d" AC_RESET "\n", pending);
    printf("   ========================================\n\n");

    printf("   [1] Add New Task       [2] View All Tasks\n");
    printf("   [3] Complete Task      [4] Delete Task\n");
    printf("   [5] Set Reminder       [6] Active Reminders\n");
    printf("   [7] Create Note        [8] View Notes\n");
    printf("   [9] Edit Note          [10] Delete Note\n");
    printf("   [0] Save & Exit\n");
    printf("\n   " AC_YELLOW "Select Option > " AC_RESET);
}

int main() {
    load_tasks_from_file(TODO_FILE);
    login_screen();
    
    int choice;
    do {
        show_menu();
        
        if (scanf("%d", &choice) == 1) {
            clear_stdin_buffer();
            switch (choice) {
                case 1: add_task(); break;
                case 2: view_tasks(); break;
                case 3: mark_task_complete(); break;
                case 4: delete_task(); break;
                case 5: {
                    printf(AC_CYAN "\n   --- Set Reminder ---\n" AC_RESET);
                    int id; char date[50];
                    printf("   Task ID: "); scanf("%d", &id); clear_stdin_buffer();
                    printf("   Time (YYYY-MM-DD HH:MM): "); fgets(date, 50, stdin);
                    struct tm tm={0};
                    if(strptime(date, "%Y-%m-%d %H:%M", &tm)) setup_reminder_for_task(id, mktime(&tm));
                    else printf(AC_RED "   Invalid Format!\n" AC_RESET);
                    printf("   Press Enter..."); getchar();
                    break;
                }
                case 6: list_reminders(); printf("Press Enter..."); getchar(); break;
                case 7: add_note(); printf("Press Enter..."); getchar(); break;
                case 8: view_notes(); printf("Press Enter..."); getchar(); break;
                case 9: edit_note(); break;
                case 10: delete_note(); printf("Press Enter..."); getchar(); break;
                case 0: save_tasks_to_file(TODO_FILE); printf(AC_GREEN "   Goodbye!\n" AC_RESET); break;
                default: printf(AC_RED "   Invalid Choice.\n" AC_RESET); sleep(1);
            }
        } else {
            clear_stdin_buffer();
        }
    } while (choice != 0);
    
    pthread_mutex_destroy(&todo_list_mutex);
    return 0;
}