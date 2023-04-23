#include<stdio.h>
#include<string.h>

void split_string(char str[], char class[], char div[], char name[], char roll[], char attendance[]) {
    char *token;
    int i = 0;
    token = strtok(str, "_");
    while (token != NULL) {
        switch (i) {
            case 0:
                strcpy(class, token);
                break;
            case 1:
                strcpy(div, token);
                break;
            case 2:
                strcpy(name, token);
                break;
            case 3:
                strcpy(roll, token);
                break;
            case 4:
                strcpy(attendance, token);
                break;
        }
        i++;
        token = strtok(NULL, "_");
    }
}

int main() {
    char str[] = "10_A_John_1234";
    char class[5], div[1], name[20], roll[5], attendance[10];
    split_string(str, class, div, name, roll, attendance);
    printf("Class: %s\n", class);
    printf("Div: %s\n", div);
    printf("Name: %s\n", name);
    printf("Roll: %s\n", roll);
    printf("Attendance: %s\n", attendance);
    return 0;
}