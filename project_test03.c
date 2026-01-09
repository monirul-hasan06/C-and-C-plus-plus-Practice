#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>   // Sleep(ms)
#else
#include <unistd.h>    // usleep(us)
#endif

#define FILENAME "mood_log.csv"
#define MAX_ENTRIES 2000
#define NOTE_SIZE  300

// Mood codes
// 1=Happy, 2=Sad, 3=Stressed, 4=Angry, 5=Excited, 6=Neutral

typedef struct {
    char date[20];
    int mood;
    char note[NOTE_SIZE];
    int sleepHours;   // hours slept last night
    int waterGlasses; // glasses of water today
} Entry;

// Declarations
void addMood();
void viewHistory();
void weeklyReport();                 // Advanced analytics on last 7 entries
void moodSuggestion(int mood);
void loadingAnimation(const char* msg, int dots, int ms);
void printBar(const char* label, int count);
int  readAllEntries(Entry arr[], int maxn);
int  containsIgnoreCase(const char* text, const char* key);
void toLowerStr(char* s);
const char* motivationalQuote(int mood);
void moodPrediction(Entry arr[], int n);
int  clampInt(int v, int lo, int hi);

// ---------------------- MAIN MENU ----------------------
int main() {
    int choice;

    while (1) {
        printf("\n==== Mental Health Mood Tracker (Advanced) ====\n");
        printf("1. Add Today's Mood\n");
        printf("2. View Mood History\n");
        printf("3. Weekly Mood Report (Advanced)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar()!='\n'); // clear invalid input
            printf("Invalid input. Try again.\n");
            continue;
        }

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
                printf("Exiting... Take care of yourself.\n");
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

// ---------------------- ADD MOOD ----------------------
void addMood() {
    FILE *fp = fopen(FILENAME, "a");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }

    char date[20];
    int mood;
    char note[NOTE_SIZE];
    int sleepHours, waterGlasses;

    printf("Enter Date (DD/MM/YYYY): ");
    scanf("%19s", date);

    printf("Select Mood:\n");
    printf("1. Happy\n2. Sad\n3. Stressed\n4. Angry\n5. Excited\n6. Neutral\n");
    printf("Choice: ");
    scanf("%d", &mood);
    while (getchar()!='\n'); // clear buffer

    printf("Enter a note (optional, single line): ");
    fgets(note, sizeof(note), stdin);
    note[strcspn(note, "\n")] = '\0';

    printf("Hours of sleep last night (e.g., 6): ");
    scanf("%d", &sleepHours);
    printf("Water intake today (glasses, e.g., 8): ");
    scanf("%d", &waterGlasses);

    // CSV format: date,mood,note,sleep,water
    fprintf(fp, "%s,%d,%s,%d,%d\n", date, mood, note, sleepHours, waterGlasses);
    fclose(fp);

    printf("Mood saved successfully.\n");
    moodSuggestion(mood);

    // Show motivational quote for negative moods
    if (mood == 2 || mood == 3 || mood == 4) {
        const char* q = motivationalQuote(mood);
        if (q && q[0]) {
            printf("Motivational: %s\n", q);
        }
    }
}

// ---------------------- SUGGESTION ----------------------
void moodSuggestion(int mood) {
    switch (mood) {
        case 1: printf("Suggestion: Keep smiling. Spend time with friends or family.\n"); break;
        case 2: printf("Suggestion: Listen to your favorite music or talk to a close friend.\n"); break;
        case 3: printf("Suggestion: Take a 5-minute deep-breathing break; short walk can help.\n"); break;
        case 4: printf("Suggestion: Drink water, pause, write your thoughts for 5 minutes.\n"); break;
        case 5: printf("Suggestion: Use the energy to learn something new or create a small project.\n"); break;
        case 6: printf("Suggestion: Try reading a few pages or take a light walk outside.\n"); break;
        default: printf("Suggestion: Stay positive and be kind to yourself.\n");
    }
}

// ---------------------- VIEW HISTORY ----------------------
void viewHistory() {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        printf("No history found.\n");
        return;
    }
    printf("\n==== Full Mood History (CSV) ====\n");
    printf("date,mood,note,sleep,water\n");
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// ---------------------- LOADING ----------------------
void loadingAnimation(const char* msg, int dots, int ms) {
    printf("\n%s", msg);
    fflush(stdout);
    for (int i = 0; i < dots; i++) {
        printf(".");
        fflush(stdout);
        #ifdef _WIN32
            Sleep(ms);
        #else
            usleep(ms * 1000);
        #endif
    }
    printf("\n\n");
}

// ---------------------- UTIL ----------------------
void printBar(const char* label, int count) {
    printf("%-9s: ", label);
    for (int i = 0; i < count; i++) printf("*");
    printf(" (%d)\n", count);
}

int readAllEntries(Entry arr[], int maxn) {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) return 0;
    int n = 0;
    while (n < maxn) {
        Entry e;
        char noteBuf[NOTE_SIZE];
        // Read CSV line robustly:
        // date,mood,note,sleep,water
        char line[1200];
        if (!fgets(line, sizeof(line), fp)) break;

        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\0') continue;

        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';

        // Parse with strtok (careful: note may contain commas; we kept note single-line but may contain commas)
        // Strategy: split first 2 fields via commas, then find last 2 integers by scanning from end.
        // Simpler approach: use sscanf with %[^,],%d,%[^,],%d,%d assuming note has no comma-heavy use. We advised single-line note; okay.
        if (sscanf(line, "%19[^,],%d,%299[^,],%d,%d", e.date, &e.mood, noteBuf, &e.sleepHours, &e.waterGlasses) == 5) {
            strncpy(e.note, noteBuf, NOTE_SIZE-1);
            e.note[NOTE_SIZE-1] = '\0';
            arr[n++] = e;
        } else {
            // Fallback: try a more permissive parse using last commas
            char *p = strrchr(line, ',');
            if (!p) continue;
            int water = atoi(p+1);
            *p = '\0';
            char *p2 = strrchr(line, ',');
            if (!p2) continue;
            int sleep = atoi(p2+1);
            *p2 = '\0';
            // Now we have: date,mood,note in 'line'
            Entry e2;
            if (sscanf(line, "%19[^,],%d,%299[^\n]", e2.date, &e2.mood, noteBuf) >= 2) {
                strncpy(e2.note, noteBuf, NOTE_SIZE-1);
                e2.note[NOTE_SIZE-1] = '\0';
                e2.sleepHours = sleep;
                e2.waterGlasses = water;
                arr[n++] = e2;
            }
        }
    }
    fclose(fp);
    return n;
}

int containsIgnoreCase(const char* text, const char* key) {
    if (!text || !key) return 0;
    size_t n = strlen(text), m = strlen(key);
    if (m == 0 || n < m) return 0;

    // Create lowercase copies
    char *t = (char*)malloc(n+1);
    char *k = (char*)malloc(m+1);
    if (!t || !k) { if(t) free(t); if(k) free(k); return 0; }
    strcpy(t, text); strcpy(k, key);
    toLowerStr(t); toLowerStr(k);

    int found = strstr(t, k) != NULL;
    free(t); free(k);
    return found;
}

void toLowerStr(char* s) {
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

const char* motivationalQuote(int mood) {
    static const char* sadQuotes[] = {
        "Every day may not be good, but there’s something good in every day.",
        "This too shall pass. Better days are coming.",
        "It’s okay to feel sad. Be gentle with yourself."
    };
    static const char* stressQuotes[] = {
        "Take a deep breath. You are stronger than you think.",
        "One step at a time. Focus on what you can control.",
        "Rest is productive. Pause and reset."
    };
    static const char* angryQuotes[] = {
        "Respond, don’t react. A short pause changes everything.",
        "Let go of what you cannot control.",
        "Calm mind, clear path forward."
    };

    if (mood == 2) return sadQuotes[rand()%3];
    if (mood == 3) return stressQuotes[rand()%3];
    if (mood == 4) return angryQuotes[rand()%3];
    return "";
}

int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// ---------------------- PREDICTION (last 3 days) ----------------------
void moodPrediction(Entry arr[], int n) {
    if (n < 3) return;
    int m1 = arr[n-1].mood, m2 = arr[n-2].mood, m3 = arr[n-3].mood;

    // Simple rule: if last 3 are same negative mood -> warn and suggest
    if (m1 == m2 && m2 == m3) {
        if (m1 == 2) {
            printf("Prediction: You have been Sad for 3 days. Consider talking to a friend or taking a relaxing walk.\n");
        } else if (m1 == 3) {
            printf("Prediction: Continuous Stress for 3 days. Try scheduling breaks and short breathing exercises.\n");
        } else if (m1 == 4) {
            printf("Prediction: Feeling Angry repeatedly. Journaling or a brief timeout can help.\n");
        } else if (m1 == 1) {
            printf("Prediction: Consistently Happy. Keep up your positive habits.\n");
        } else if (m1 == 5) {
            printf("Prediction: High excitement lately. Channel it into a project or learning goal.\n");
        } else if (m1 == 6) {
            printf("Prediction: Neutral for several days. Try a new activity for variety.\n");
        }
    }
}

// ---------------------- WEEKLY REPORT (Advanced) ----------------------
void weeklyReport() {
    Entry arr[MAX_ENTRIES];
    int total = readAllEntries(arr, MAX_ENTRIES);
    if (total == 0) {
        printf("No data available for report.\n");
        return;
    }

    loadingAnimation("Generating Weekly Report", 5, 450);

    // Take last 7 entries for "weekly"
    int start = total > 7 ? total - 7 : 0;
    int n = total - start;

    int count[7] = {0}; // 0 unused; 1..6 used
    int sumSleep = 0, sumWater = 0;
    int lowSleepDays = 0, lowSleepSadStress = 0;

    // Trigger keywords
    const char* keys[] = {
        "exam","deadline","assignment","family","health","money","friend","relationship","work","study","job"
    };
    int keyCount[sizeof(keys)/sizeof(keys[0])];
    memset(keyCount, 0, sizeof(keyCount));

    for (int i = start; i < total; i++) {
        Entry *e = &arr[i];
        if (e->mood >= 1 && e->mood <= 6) count[e->mood]++;

        sumSleep += e->sleepHours;
        sumWater += e->waterGlasses;

        if (e->sleepHours < 6) {
            lowSleepDays++;
            if (e->mood == 2 || e->mood == 3) lowSleepSadStress++;
        }

        for (size_t k = 0; k < sizeof(keys)/sizeof(keys[0]); k++) {
            if (containsIgnoreCase(e->note, keys[k])) keyCount[k]++;
        }
    }

    double avgSleep = n ? (double)sumSleep / n : 0.0;
    double avgWater = n ? (double)sumWater / n : 0.0;

    printf("==== Weekly Mood Report (last %d entries) ====\n", n);
    printf("Happy   : %d\n", count[1]);
    printf("Sad     : %d\n", count[2]);
    printf("Stressed: %d\n", count[3]);
    printf("Angry   : %d\n", count[4]);
    printf("Excited : %d\n", count[5]);
    printf("Neutral : %d\n", count[6]);

    printf("\nASCII Chart:\n");
    printBar("Happy",   count[1]);
    printBar("Sad",     count[2]);
    printBar("Stress",  count[3]);
    printBar("Angry",   count[4]);
    printBar("Excited", count[5]);
    printBar("Neutral", count[6]);

    // Trend summary
    int maxMood = 1, maxVal = count[1];
    for (int m = 2; m <= 6; m++) {
        if (count[m] > maxVal) { maxVal = count[m]; maxMood = m; }
    }
    const char* moodName[] = {"","Happy","Sad","Stressed","Angry","Excited","Neutral"};
    printf("\nTrend: Mostly %s this week.\n", moodName[maxMood]);

    // Prediction (based on last 3 entries overall)
    if (total >= 3) {
        moodPrediction(arr, total);
    }

    // Wellbeing score (0..10)
    int pos = 2*(count[1] + count[5]) + count[6];
    int neg = 2*(count[2] + count[3] + count[4]);
    int raw = pos - neg;               // range roughly [-14, +14] for 7 entries
    int score = (int)((raw + 14) * 10 / 28.0 + 0.5); // map to 0..10
    score = clampInt(score, 0, 10);
    printf("Overall Wellbeing Score: %d/10\n", score);

    // Habits summary
    printf("\nHabits (last %d entries):\n", n);
    printf("Average sleep: %.2f hours\n", avgSleep);
    printf("Average water: %.2f glasses\n", avgWater);

    if (avgSleep < 6.0)  printf("Insight: You are sleeping less than 6 hours on average. Try an earlier bedtime.\n");
    if (avgWater < 6.0)  printf("Insight: Water intake seems low. Aim for more regular hydration.\n");

    if (lowSleepDays > 0) {
        double corr = (double)lowSleepSadStress / lowSleepDays * 100.0;
        printf("Correlation: On low-sleep days (<6h), %.0f%% of the time your mood was Sad/Stressed.\n", corr);
    }

    // Trigger detection summary
    printf("\nTriggers detected in notes (keyword counts):\n");
    int any = 0;
    for (size_t k = 0; k < sizeof(keys)/sizeof(keys[0]); k++) {
        if (keyCount[k] > 0) {
            printf(" - %s: %d\n", keys[k], keyCount[k]);
            any = 1;
        }
    }
    if (!any) printf(" - None spotted this week.\n");

    // Actionable guidance
    printf("\nActionable Guidance:\n");
    if (count[2] + count[3] >= 3) {
        printf("- Consider a short daily routine: 5-min breathing + 10-min walk + brief journaling.\n");
    }
    if (count[4] >= 2) {
        printf("- Practice a pause: count to 10 and write before responding during conflicts.\n");
    }
    if (count[1] + count[5] >= 4) {
        printf("- Great momentum. Lock in habits that support this: regular sleep, movement, and social time.\n");
    }
    if (avgSleep < 6.0) {
        printf("- Set a consistent lights-off time and reduce screens 30 minutes before bed.\n");
    }
    if (avgWater < 6.0) {
        printf("- Keep a bottle nearby and sip throughout the day.\n");
    }

    // Gentle closing suggestion based on dominant mood
    if (maxMood == 2) printf("\nNote: If sadness persists, connecting with a trusted person can help.\n");
    if (maxMood == 3) printf("Note: Plan micro-breaks between tasks to reduce stress build-up.\n");
    if (maxMood == 4) printf("Note: A quick reset (walk, water, writing) can lower anger spikes.\n");
}

// ---------------------- END ----------------------
