/*
 * NEXORA - Ultimate Task Manager
 * ==============================
 * Features:
 * 1. CRUD Operations (Add, View, Complete, Delete)
 * 2. System Level Persistent Alarm (Complex: Works via OS Kernel even if app is closed)
 * 3. Fuzzy Search (Complex: Levenshtein Distance Algorithm)
 * 4. Notes System (File I/O)
 * 5. Cross-Platform (Linux & Windows)
 */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

// --- OS Specific Includes ---
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <unistd.h>
    #include <sys/types.h>
    #define MKDIR(dir) mkdir(dir, 0700)
#endif

// --- Colors ---
#define AC_RESET   "\x1b[0m"
#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_CYAN    "\x1b[36m"
#define AC_BOLD    "\x1b[1m"

// --- Constants ---
#define MAX_TASKS 100
#define TODO_FILE "tasks.dat"
#define NOTES_DIR "notes"

// --- Structures ---
typedef struct {
    int id;
    char title[100];
    bool completed;
    int priority; // 1=High, 2=Med, 3=Low
} TodoItem;

// --- Globals ---
TodoItem todo_list[MAX_TASKS];
int todo_count = 0;

// --- Helpers ---
void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

// --- ALGORITHM 1: Levenshtein Distance (For Fuzzy Search) ---
int levenshtein_distance(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1;
            matrix[i][j] = min3(
                matrix[i - 1][j] + 1,
                matrix[i][j - 1] + 1,
                matrix[i - 1][j - 1] + cost
            );
        }
    }
    return matrix[len1][len2];
}

// --- ALGORITHM 2: OS Kernel Alarm (Persistent) ---
void schedule_persistent_alarm(char *task_title, int hour, int min) {
    char command[2048];
    char clean_title[100];

    // Clean title for shell command safety
    strcpy(clean_title, task_title);
    for(int i=0; clean_title[i]; i++) {
        if(!isalnum(clean_title[i])) clean_title[i] = ' '; 
    }

    #ifdef _WIN32
        char time_str[10];
        snprintf(time_str, sizeof(time_str), "%02d:%02d", hour, min);
        snprintf(command, sizeof(command),
            "schtasks /Create /F /SC ONCE /TN \"Nexora_%s\" /ST %s /TR \"powershell -WindowStyle Hidden -Command \\\"Add-Type -AssemblyName PresentationFramework; [System.Windows.MessageBox]::Show('DUE: %s', 'Nexora Alarm'); [System.Console]::Beep(1000,1000);\\\"\"",
            clean_title, time_str, clean_title);
    #else
        // Linux Command Chain
        snprintf(command, sizeof(command),
            "echo \"export DISPLAY=:0 && export DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u)/bus && notify-send -u critical 'NEXORA ALERT' 'Task Due: %s' && spd-say 'Attention, task %s is due'\" | at %02d:%02d",
            clean_title, clean_title, hour, min);
    #endif

    printf("\n   " AC_CYAN "[SYSTEM] Scheduling alarm with OS Kernel..." AC_RESET "\n");
    int result = system(command);

    if (result == 0) {
        printf(AC_GREEN "   [SUCCESS] Alarm set for %02d:%02d!\n" AC_RESET, hour, min);
        printf(AC_YELLOW "   You can now close the code. The system will handle it.\n" AC_RESET);
    } else {
        printf(AC_RED "   [FAILED] System refused schedule.\n" AC_RESET);
        #ifndef _WIN32
        printf("   Linux Fix: Run 'sudo systemctl enable --now atd'\n");
        #endif
    }
}

// --- File Operations ---
void save_tasks() {
    FILE *fp = fopen(TODO_FILE, "wb");
    if (!fp) return;
    fwrite(&todo_count, sizeof(int), 1, fp);
    fwrite(todo_list, sizeof(TodoItem), todo_count, fp);
    fclose(fp);
}

void load_tasks() {
    FILE *fp = fopen(TODO_FILE, "rb");
    if (!fp) return;
    fread(&todo_count, sizeof(int), 1, fp);
    fread(todo_list, sizeof(TodoItem), todo_count, fp);
    fclose(fp);
}

// --- Core Features ---
void add_task() {
    if (todo_count >= MAX_TASKS) { printf("List Full!\n"); return; }
    TodoItem t;
    t.id = (todo_count == 0) ? 1 : todo_list[todo_count - 1].id + 1;
    t.completed = false;

    printf("   Enter Title: ");
    fgets(t.title, 100, stdin);
    t.title[strcspn(t.title, "\n")] = 0;

    printf("   Priority (1=High, 2=Med, 3=Low): ");
    if(scanf("%d", &t.priority) != 1) t.priority = 3;
    clear_stdin();

    todo_list[todo_count++] = t;
    save_tasks();
    printf(AC_GREEN "   Task Added!\n" AC_RESET);
}

void view_tasks() {
    printf("\n   " AC_BOLD "--- YOUR TASK LIST ---" AC_RESET "\n");
    printf("   %-4s | %-20s | %-8s | %s\n", "ID", "Title", "Priority", "Status");
    printf("   ------------------------------------------------\n");
    
    for (int i = 0; i < todo_count; i++) {
        char *prio = (todo_list[i].priority == 1) ? AC_RED "HIGH" : ((todo_list[i].priority == 2) ? AC_YELLOW "MED " : AC_CYAN "LOW ");
        char *status = todo_list[i].completed ? AC_GREEN "[DONE]" AC_RESET : "[PENDING]";
        
        printf("   %-4d | %-20s | %s" AC_RESET " | %s\n", 
            todo_list[i].id, todo_list[i].title, prio, status);
    }
}

void mark_complete() {
    view_tasks();
    int id;
    printf("\n   Enter ID to Complete: ");
    scanf("%d", &id);
    clear_stdin();
    
    for(int i=0; i<todo_count; i++) {
        if(todo_list[i].id == id) {
            todo_list[i].completed = true;
            save_tasks();
            printf(AC_GREEN "   Task Marked as Done!\n" AC_RESET);
            return;
        }
    }
    printf(AC_RED "   ID not found.\n" AC_RESET);
}

void delete_task() {
    view_tasks();
    int id;
    printf("\n   Enter ID to Delete: ");
    scanf("%d", &id);
    clear_stdin();

    int found = 0;
    for(int i=0; i<todo_count; i++) {
        if(todo_list[i].id == id) {
            for(int j=i; j<todo_count-1; j++) todo_list[j] = todo_list[j+1];
            todo_count--;
            found = 1;
            break;
        }
    }
    if(found) {
        save_tasks();
        printf(AC_RED "   Task Deleted.\n" AC_RESET);
    } else {
        printf("   Not found.\n");
    }
}

// --- Advanced Features (Search & Notes) ---
void fuzzy_search() {
    char query[100];
    printf("\n   [SMART SEARCH] Enter approximate name: ");
    fgets(query, 100, stdin);
    query[strcspn(query, "\n")] = 0;

    printf("   --- Matches ---\n");
    bool found = false;
    for(int i=0; i<todo_count; i++) {
        int dist = levenshtein_distance(query, todo_list[i].title);
        // Match if distance is small OR if it's a substring
        if(dist <= 3 || strstr(todo_list[i].title, query)) {
            printf("   [Match Score: %d%%] %s\n", 100-(dist*10), todo_list[i].title);
            found = true;
        }
    }
    if(!found) printf("   No matches found.\n");
}

void add_note() {
    struct stat st = {0};
    if (stat(NOTES_DIR, &st) == -1) MKDIR(NOTES_DIR);

    char title[100], path[150];
    printf("   Note Title: ");
    fgets(title, 100, stdin);
    title[strcspn(title, "\n")] = 0;
    for(int i=0; title[i]; i++) if(title[i]==' ') title[i]='_';

    snprintf(path, sizeof(path), "%s/%s.txt", NOTES_DIR, title);
    FILE *fp = fopen(path, "w");
    if(fp) {
        printf("   Write Content (Type 'END' on new line to save):\n");
        char line[256];
        while(fgets(line, sizeof(line), stdin)) {
            if(strncmp(line, "END", 3) == 0) break;
            fprintf(fp, "%s", line);
        }
        fclose(fp);
        printf(AC_GREEN "   Note Saved!\n" AC_RESET);
    }
}

void view_notes() {
    struct dirent *de;
    DIR *dr = opendir(NOTES_DIR);
    if (dr == NULL) {
        printf("   No notes folder yet.\n");
        return;
    }
    printf("\n   --- SAVED NOTES ---\n");
    while ((de = readdir(dr)) != NULL) {
        if(strstr(de->d_name, ".txt")) printf("   - %s\n", de->d_name);
    }
    closedir(dr);
    printf("   (Check 'notes' folder to read contents)\n");
}

// --- Main Menu ---
int main() {
    load_tasks();
    clear_screen();
    
    printf(AC_BOLD AC_CYAN "\n   #######################################\n");
    printf("   #       NEXORA: ADVANCED MANAGER      #\n");
    printf("   #######################################\n" AC_RESET);

    int choice;
    while (1) {
        printf("\n   [1] Add Task          [5] Set Alarm (Persistent)\n");
        printf("   [2] View Tasks        [6] Smart Search (Fuzzy)\n");
        printf("   [3] Complete Task     [7] Add Note\n");
        printf("   [4] Delete Task       [8] View Notes List\n");
        printf("   [0] Exit\n");
        printf(AC_YELLOW "   Select > " AC_RESET);
        
        if (scanf("%d", &choice) != 1) { clear_stdin(); continue; }
        clear_stdin();

        switch (choice) {
            case 1: add_task(); break;
            case 2: view_tasks(); break;
            case 3: mark_complete(); break;
            case 4: delete_task(); break;
            case 5: { // Alarm Logic
                view_tasks();
                int id, h, m;
                printf("\n   Enter Task ID: ");
                if(scanf("%d", &id) != 1) { clear_stdin(); break; }
                
                printf("   Enter Time (24h format -> HH MM): ");
                // STRICT INPUT CHECKING FOR HH MM
                if(scanf("%d %d", &h, &m) != 2) {
                    printf(AC_RED "   Invalid Time! Use space (e.g., 15 30)\n" AC_RESET);
                    clear_stdin();
                    break;
                }
                clear_stdin();
                
                // Validate Time
                if(h < 0 || h > 23 || m < 0 || m > 59) {
                    printf(AC_RED "   Invalid Time! (0-23 hours, 0-59 mins)\n" AC_RESET);
                    break;
                }

                char title[100] = "";
                int found = 0;
                for(int i=0; i<todo_count; i++) {
                    if(todo_list[i].id == id) {
                        strcpy(title, todo_list[i].title);
                        found = 1; break;
                    }
                }
                
                if(found) schedule_persistent_alarm(title, h, m);
                else printf(AC_RED "   ID not found.\n" AC_RESET);
                break;
            }
            case 6: fuzzy_search(); break;
            case 7: add_note(); break;
            case 8: view_notes(); break;
            case 0: save_tasks(); printf("   Goodbye!\n"); return 0;
            default: printf("   Invalid Option.\n");
        }
    }
    return 0;
}