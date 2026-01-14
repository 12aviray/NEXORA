#include "shared.h"
#include <sys/ioctl.h> 
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// Making big border
void print_big_border() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;

    printf(AC_CYAN);
    for (int i = 0; i < width; i++) printf("=");
    printf(AC_RESET "\n");
}

int main() {
    // ১. system start
    clear_screen();
    printf("\n\n");
    print_big_border();
    print_centered("NEXORA: ULTIMATE MANAGER", AC_BOLD AC_CYAN);
    print_big_border();
    printf("\n");

    // login system
    if (!login_system()) {
        print_centered("Exiting NEXORA...", AC_RED);
        return 0;
    }

    // Data load
    load_tasks_from_file();
    create_notes_directory();

    int choice;
    while (1) {
        clear_screen();
        
        print_big_border();
        char header[256]; 
        snprintf(header, sizeof(header), "NEXORA DASHBOARD | User: %s", current_user_name);
        print_centered(header, AC_BOLD AC_CYAN);
        print_big_border();
        
        // progress Bar
        printf("\n");
        print_progress_bar(); 
        printf("\n");

        print_centered("--- TASK OPERATIONS ---", AC_BOLD AC_MAGENTA);
        print_centered("[1] View Task Board      [5] Set Reminder", AC_WHITE);
        print_centered("[2] Add New Task         [6] Smart Search", AC_WHITE);
        print_centered("[3] Mark Task Done       [7] Delete Task ", AC_WHITE);
        printf("\n");

        print_centered("--- NOTES SYSTEM ---", AC_BOLD AC_MAGENTA);
        print_centered("[8] Add Quick Note       [10] Edit Existing Note", AC_WHITE);
        print_centered("[9] View All Notes       [11] Delete Note       ", AC_WHITE);
        print_centered("[12] Smart Search Notes  [13] Set Alarm (Time)  ", AC_WHITE);
        printf("\n");

        print_centered("--- SYSTEM SETTINGS ---", AC_BOLD AC_MAGENTA);
        print_centered("[0] SAVE & LOGOUT", AC_RED);
        printf("\n");

        // input box centering
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int input_padding = (w.ws_col / 2) - 10;
        if(input_padding < 0) input_padding = 0;
        for(int k = 0; k < input_padding; k++) printf(" ");
        
        printf(AC_BOLD AC_GREEN "Selection > " AC_RESET);
        
        if (scanf("%d", &choice) != 1) {
            clear_stdin_buffer();
            continue;
        }
        clear_stdin_buffer();

        switch (choice) {
            case 1: view_tasks(); break;
            case 2: add_task(); break;
            case 3: mark_task_complete(); break;
            case 5: {
                view_tasks_mini();
                int r_id, mins;
                printf("\n   Enter Task ID for Reminder: "); 
                if(scanf("%d", &r_id) == 1) {
                    printf("   Minutes from now: "); 
                    if(scanf("%d", &mins) == 1) {
                        clear_stdin_buffer();
                        setup_thread_reminder(r_id, mins);
                    }
                }
                clear_stdin_buffer();
                SLEEP_SEC(1);
                break;
            }
            case 6: fuzzy_search_tasks(); break;
            case 7: delete_task(); break;
            case 8: add_note(); break;
            case 9: view_notes(); break;
            case 10: edit_note(); break;
            case 11: delete_note(); break;
            case 12: search_notes(); break;
            case 13: {
                int a_id;
                view_tasks_mini();
                printf("\n   Enter Task ID for Alarm: ");
                if(scanf("%d", &a_id) == 1) {
                    clear_stdin_buffer();
                    setup_alarm(a_id); 
                } else {
                    clear_stdin_buffer();
                }
                SLEEP_SEC(1);
                break;
            }
            case 0:
                save_tasks_to_file();
                printf("\n");
                print_centered("Saving data and logging out...", AC_GREEN);
                SLEEP_SEC(1);
                clear_screen();
                return 0;
            default:
                print_centered("Invalid Choice!", AC_RED);
                SLEEP_SEC(1);
        }
    }
    return 0;
}