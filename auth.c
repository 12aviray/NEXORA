#include "shared.h"


void safe_input(char *buf, int size) {
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

void register_user() {
    User newUser, checkUser;
    FILE *fp = fopen(USER_FILE, "ab+");
    
    clear_screen();
    print_vertical_padding(10); 
    
    print_centered("==============================", AC_CYAN);
    print_centered("     REGISTER NEW ACCOUNT     ", AC_BOLD AC_YELLOW);
    print_centered("==============================", AC_CYAN);
    printf("\n");

    print_centered_prompt(" Enter Username: ", AC_WHITE);
    safe_input(newUser.username, 50);

    rewind(fp);
    while (fread(&checkUser, sizeof(User), 1, fp)) {
        if (strcmp(checkUser.username, newUser.username) == 0) {
            printf("\n");
            print_centered("Error: Username already exists!", AC_RED);
            fclose(fp);
            SLEEP_SEC(2);
            return;
        }
    }

    print_centered_prompt(" Enter Password: ", AC_WHITE);
    safe_input(newUser.password, 50);

    fwrite(&newUser, sizeof(User), 1, fp);
    fclose(fp);
    
    printf("\n");
    print_centered("Registration Successful!", AC_GREEN);
    SLEEP_SEC(1);
}

bool login_system() {
    User inputUser, storedUser;
    char choice_str[10];

    while (1) {
        clear_screen();
        print_vertical_padding(10); 

        print_centered("==============================", AC_CYAN);
        print_centered("   NEXORA: AUTHENTICATION    ", AC_BOLD AC_CYAN);
        print_centered("==============================", AC_CYAN);
        printf("\n");
        print_centered("[1] Login Now", AC_WHITE);
        print_centered("[2] Register ", AC_WHITE);
        print_centered("[0] Exit     ", AC_RED);
        printf("\n");
        
        print_centered_prompt("Selection > ", AC_YELLOW);
        safe_input(choice_str, 10);

        if (choice_str[0] == '0') exit(0);
        if (choice_str[0] == '2') {
            register_user();
            continue;
        }
        if (choice_str[0] != '1') continue;

        // Login UI
        clear_screen();
        print_vertical_padding(8);
        print_centered("--- LOGIN ---", AC_BOLD AC_YELLOW);
        printf("\n");
        
        print_centered_prompt(" Username: ", AC_WHITE);
        safe_input(inputUser.username, 50);
        
        print_centered_prompt(" Password: ", AC_WHITE);
        safe_input(inputUser.password, 50);

        FILE *fp = fopen(USER_FILE, "rb");
        if (!fp) {
            print_centered("No users found. Please register.", AC_RED);
            SLEEP_SEC(2);
            continue;
        }

        bool found = false;
        while (fread(&storedUser, sizeof(User), 1, fp)) {
            if (strcmp(storedUser.username, inputUser.username) == 0 && 
                strcmp(storedUser.password, inputUser.password) == 0) {
                found = true;
                strcpy(current_user_name, storedUser.username);
                break;
            }
        }
        fclose(fp);

        if (found) {
            print_centered("Login Successful! Redirecting...", AC_GREEN);
            SLEEP_SEC(1);
            return true;
        } else {
            print_centered("Invalid Username or Password!", AC_RED);
            SLEEP_SEC(2);
        }
    }
}