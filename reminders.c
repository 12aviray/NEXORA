#include "shared.h"

typedef struct {
    int task_index;
    time_t reminder_time;
} ReminderArgs;

void *reminder_thread_func(void *arg) {
    ReminderArgs *args = (ReminderArgs *)arg;
    int task_index = args->task_index;
    time_t reminder_time = args->reminder_time;

    while (1) {
        time_t current_time = time(NULL);
        double seconds_to_wait = difftime(reminder_time, current_time);

        if (seconds_to_wait <= 0) {
            pthread_mutex_lock(&todo_list_mutex);
            if (task_index < todo_count && todo_list[task_index].has_reminder) {
                printf("\n!!! REMINDER !!! Task: '%s' is due!\n", todo_list[task_index].title);
            }
            pthread_mutex_unlock(&todo_list_mutex);
            break;
        } else {
            sleep(seconds_to_wait > 10 ? 10 : (int)seconds_to_wait + 1);
        }
    }
    free(args);
    return NULL;
}

void setup_reminder_for_task(int task_id, time_t reminder_time) {
    pthread_mutex_lock(&todo_list_mutex);
    int idx = -1;
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == task_id) { idx = i; break; }
    }

    if (idx != -1 && reminder_time > time(NULL)) {
        todo_list[idx].has_reminder = true;
        todo_list[idx].reminder_time = reminder_time;
        
        ReminderArgs *args = malloc(sizeof(ReminderArgs));
        args->task_index = idx;
        args->reminder_time = reminder_time;

        pthread_t tid;
        pthread_create(&tid, NULL, reminder_thread_func, args);
        pthread_detach(tid);
        printf("Reminder set.\n");
    } else {
        printf("Invalid task or past time.\n");
    }
    pthread_mutex_unlock(&todo_list_mutex);
}

void list_reminders() {
    pthread_mutex_lock(&todo_list_mutex);
    printf("\n--- Active Reminders ---\n");
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].has_reminder) {
            char buf[64];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&todo_list[i].reminder_time));
            printf("Task: %s | At: %s\n", todo_list[i].title, buf);
        }
    }
    pthread_mutex_unlock(&todo_list_mutex);
}