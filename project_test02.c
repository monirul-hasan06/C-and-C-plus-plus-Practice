#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>   // For Sleep() on Windows
#else
#include <unistd.h>    // For usleep() on Linux/Unix
#endif

#define FILENAME "mood_log.txt"

// Function declarations
void addMood();
void viewHistory();
void weeklyReport();
void moodSuggestion(int mood);
void loadingAnimation();

int main() {
    int choice;

    while (1) {
        printf("\n==== Mental Health Mood Tracker ====\n");
        printf("1. Add Today's Mood\n");
        printf("2. View Mood History\n");
        printf("3. Weekly Mood Report\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addMood();
                break;
            case 2:
                viewHistory();
                break;
            case 3:
                weeklyReport();
                break;
            case 4:
                printf("Exiting... Take care of yourself!\n");
                exit(0);
            default:
                printf("Invalid choice. Try again!\n");
        }
    }
    return 0;
}

// Function to add today's mood
void addMood() {
    FILE *fp;
    char date[20], note[100];
    int mood;

    fp = fopen(FILENAME, "a"); // open file in append mode
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter Date (DD/MM/YYYY): ");
    scanf("%s", date);

    printf("Select Mood:\n");
    printf("1. Happy\n2. Sad\n3. Stressed\n4. Angry\n5. Excited\n6. Neutral\n");
    printf("Choice: ");
    scanf("%d", &mood);
    getchar(); // clear buffer

    printf("Enter a note (optional): ");
    fgets(note, sizeof(note), stdin);
    note[strcspn(note, "\n")] = '\0'; // remove newline

    fprintf(fp, "%s,%d,%s\n", date, mood, note); // save in file
    fclose(fp);

    printf("Mood saved successfully!\n");
    moodSuggestion(mood); // give suggestion based on mood
}

// Function to show suggestions based on mood
void moodSuggestion(int mood) {
    switch (mood) {
        case 1:
            printf("Suggestion: Keep smiling! Spend time with friends or family.\n");
            break;
        case 2:
            printf("Suggestion: Listen to your favorite music or talk to a friend.\n");
            break;
        case 3:
            printf("Suggestion: Take a 5-min deep breathing break, relax your mind.\n");
            break;
        case 4:
            printf("Suggestion: Drink water, stay calm, take some alone time.\n");
            break;
        case 5:
            printf("Suggestion: Use your energy to learn something new or create!\n");
            break;
        case 6:
            printf("Suggestion: Try reading a book or going for a walk.\n");
            break;
        default:
            printf("Suggestion: Stay positive and take care of yourself!\n");
    }
}

// Function to view mood history
void viewHistory() {
    FILE *fp;
    char line[200];

    fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("No history found.\n");
        return;
    }

    printf("\n==== Mood History ====\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// Loading animation before report
void loadingAnimation() {
    int i;
    printf("\nGenerating Weekly Report");
    for (i = 0; i < 5; i++) {
        printf(".");
        fflush(stdout);   // immediately show dot
        #ifdef _WIN32
        Sleep(500);       // 500 ms delay (Windows)
        #else
        usleep(500000);   // 500 ms delay (Linux/Unix)
        #endif
    }
    printf("\n\n");
}

// Function to generate weekly report
void weeklyReport() {
    FILE *fp;
    char date[20], note[100];
    int mood, count[7] = {0};

    fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("No data available for report.\n");
        return;
    }

    // show loading before report
    loadingAnimation();

    while (fscanf(fp, "%[^,],%d,%[^\n]\n", date, &mood, note) != EOF) {
        if (mood >= 1 && mood <= 6) {
            count[mood]++;
        }
    }
    fclose(fp);

    printf("==== Weekly Mood Report ====\n");
    printf("Happy   : %d\n", count[1]);
    printf("Sad     : %d\n", count[2]);
    printf("Stressed: %d\n", count[3]);
    printf("Angry   : %d\n", count[4]);
    printf("Excited : %d\n", count[5]);
    printf("Neutral : %d\n", count[6]);
}
