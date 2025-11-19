#include "shared.h"

void create_notes_directory() {
    struct stat st = {0};
    if (stat(NOTES_DIR_NAME, &st) == -1) mkdir(NOTES_DIR_NAME, 0700);
}

void add_note() {
    create_notes_directory();
    char title[MAX_NOTE_TITLE], filepath[512];
    printf("Enter note title: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;
    for(int i=0; title[i]; i++) if(title[i]==' ') title[i]='_';

    snprintf(filepath, sizeof(filepath), "%s/%s.txt", NOTES_DIR_NAME, title);
    FILE *fp = fopen(filepath, "w");
    if (!fp) return;

    printf("Enter content (type 'END' to finish):\n");
    char line[MAX_NOTE_CONTENT];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0) break;
        fprintf(fp, "%s", line);
    }
    fclose(fp);
    printf("Note saved.\n");
}

void view_notes() {
    create_notes_directory();
    DIR *d = opendir(NOTES_DIR_NAME);
    struct dirent *dir;
    if (d) {
        printf("\n--- Notes ---\n");
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".txt")) {
                printf("Note: %s\n", dir->d_name);
                char path[256];
                snprintf(path, sizeof(path), "%s/%s", NOTES_DIR_NAME, dir->d_name);
                FILE *fp = fopen(path, "r");
                if (fp) {
                    char c;
                    while ((c = fgetc(fp)) != EOF) putchar(c);
                    fclose(fp);
                }
                printf("\n----------------\n");
            }
        }
        closedir(d);
    }
}

// Edit and Delete functions can be added similarly using shared helpers
void delete_note() {
    char title[MAX_NOTE_TITLE], filepath[256];
    printf("Enter title to delete: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;
    snprintf(filepath, sizeof(filepath), "%s/%s.txt", NOTES_DIR_NAME, title);
    if (remove(filepath) == 0) printf("Deleted.\n");
    else printf("Not found.\n");
}

void edit_note() {
    // Similar logic to add_note but opens in "w" mode to overwrite existing
    printf("Re-use Add Note to overwrite/edit for simplicity in this version.\n");
}