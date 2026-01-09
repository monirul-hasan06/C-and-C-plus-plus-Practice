/* ANNEX PAY - A Classic Banking System in C
 - made by "Team Hash Include", BUBT*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <direct.h>
#define mkdir(path) _mkdir(path)
#else
#include <termios.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

/* File paths */
#define DATA_DIR "./AnnexPay"
#define ACC_FILE "./AnnexPay/accounts.dat"
#define REQ_FILE "./AnnexPay/fund_requests.dat"
#define LOAN_FILE "./AnnexPay/loan_requests.dat"
#define DAILY_LIMIT_FILE "./AnnexPay/daily_limits.dat"
#define SECURITY_QA_FILE "./AnnexPay/security_qa.dat"
#define CHECKBOOK_REQ_FILE "./AnnexPay/checkbook_requests.dat"
#define MESSAGES_FILE "./AnnexPay/messages.dat"
#define REFERRALS_FILE "./AnnexPay/referrals.dat"

#define START_ACC_NO 1001
#define START_REQ_ID 1
#define TX_FILENAME_BUF 64
#define DATE_BUF 64
#define LINE_BUF 256
#define MINI_STATEMENT_COUNT 5
#define MAX_NAME 50
#define PIN_LENGTH 4
#define MAX_LOAN_AMOUNT 100000.0f
#define MAX_TENURE_MONTHS 60
#define MAX_ACCOUNTS 1000
#define MAX_DAILY_WITHDRAW 20000.0f
#define REFERRAL_BONUS 100.0f

/* ANSI color codes */
#define COLOR_GOLD   "\033[1;33m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_RED    "\033[1;31m"
#define COLOR_WHITE  "\033[1;37m"
#define COLOR_RESET  "\033[0m"

/* Game color codes */
#define COLOR_GAME_DEFAULT 7
#define COLOR_GAME_WIN 10
#define COLOR_GAME_LOSE 12
#define COLOR_GAME_TIE 14
#define COLOR_GAME_APP_NAME 11
#define COLOR_GAME_SCORE 15
#define COLOR_GAME_ANIMATION 6

int darkMode = 0;

/* Account structure */
typedef struct {
    int accNo;
    char name[MAX_NAME];
    int pin;
    float balance;
    int wrongAttempts;
    int locked;
    int referredBy; // Referrer account number
} Account;

/* Fund request */
typedef struct {
    int id;
    int fromAcc;  // Requester (who wants funds)
    int toAcc;    // Payer (who will send funds)
    float amount;
    char note[100];
    int status;   // 0: pending, 1: approved, 2: denied
} FundRequest;

/* Loan request */
#pragma pack(push, 1)
typedef struct {
    int id;
    int accNo;
    float amount;
    int tenureMonths;
    int status;   // 0: pending, 1: approved, 2: denied
} LoanRequest;
#pragma pack(pop)

/* Daily withdrawal tracking */
typedef struct {
    int accNo;
    float todayWithdrawn;
    char lastDate[12];  // YYYY-MM-DD
} DailyLimit;

/* Security Questions */
typedef struct {
    int accNo;
    char question[100];
    char answer[50];
} SecurityQA;

/* Checkbook Request */
typedef struct {
    int id;
    int accNo;
    int numberOfLeaves;
    char requestDate[12];
    int status; // 0=pending, 1=approved, 2=denied
} CheckbookRequest;

/* Message System */
typedef struct {
    int id;
    int fromAcc; // 0 for admin, account number for users
    int toAcc;   // 0 for admin, account number for users
    char message[500];
    char timestamp[20];
    int readStatus; // 0=unread, 1=read
} Message;

/* Referral Program */
typedef struct {
    int referrerAcc;
    int referredAcc;
    char referralDate[12];
    int bonusGiven; // 0=no, 1=yes
} Referral;

// Function prototypes
void resetDailyLimitIfNewDay(int accNo);
float getTodayWithdrawn(int accNo);
void addWithdrawalToDaily(int accNo, float amt);
void aboutAnnexPay();
void playRockPaperScissors(); // Game function prototype

/* Create data directory */
void ensureDataDirectory() {
    if (mkdir(DATA_DIR) != 0 && errno != EEXIST) {
        fprintf(stderr, "%sFailed to create data directory: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
    }
}

/* Get current date/time string */
void nowstr(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", tm);
}

/* Get current date string (YYYY-MM-DD) */
void getCurrentDate(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, n, "%Y-%m-%d", tm);
}

/* Platform-independent sleep (ms) */
void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/* Clear screen */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Clear input buffer */
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Wait for user to Press Enter twice to continue */
void waitForEnter() {
    printf("\n%sPress Enter twice to return to menu...%s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    clearInputBuffer();
    getchar();
}

/* Loading animation */
void showLoading(const char *msg, int duration_ms) {
    printf("%s%s", COLOR_WHITE, msg);
    fflush(stdout);
    int dots = 3;
    int interval = duration_ms / dots;
    for (int i = 0; i < dots; i++) {
        printf(".");
        fflush(stdout);
        sleep_ms(interval);
    }
    printf("%s\n", COLOR_RESET);
}

/* Validate name: letters + single spaces */
int validateName(char *name) {
    int len = strlen(name);
    if (len == 0) return 0;

    int start = 0, end = len - 1;
    while (start < len && isspace(name[start])) start++;
    while (end >= start && isspace(name[end])) end--;
    if (start > end) return 0;

    int j = 0;
    int lastSpace = 0;
    for (int i = start; i <= end; i++) {
        if (isalpha(name[i])) {
            name[j++] = name[i];
            lastSpace = 0;
        } else if (name[i] == ' ' && !lastSpace && j > 0 && (i < end && isalpha(name[i+1]))) {
            name[j++] = ' ';
            lastSpace = 1;
        }
    }
    name[j] = '\0';

    return j > 0;
}

/* Read PIN with masking and custom prompt */
int readPin(const char *prompt, int *pin) {
    char pinStr[PIN_LENGTH + 1] = {0};
    int i = 0;
    char ch;

    printf("%s%s%s", COLOR_WHITE, prompt, COLOR_RESET);
    fflush(stdout);

#ifdef _WIN32
    while (i < PIN_LENGTH) {
        ch = _getch();
        if (ch == '\r') break;
        if (isdigit(ch)) {
            pinStr[i++] = ch;
            printf("*");
        } else if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (i < PIN_LENGTH) {
        ch = getchar();
        if (ch == '\n') break;
        if (isdigit(ch)) {
            pinStr[i++] = ch;
            printf("*");
        } else if (ch == 127 && i > 0) {
            i--;
            printf("\b \b");
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    clearInputBuffer();
    printf("\n");
    pinStr[i] = '\0';

    if (strlen(pinStr) != PIN_LENGTH) {
        printf("%sInvalid PIN. Must be %d digits.%s\n", COLOR_RED, PIN_LENGTH, COLOR_RESET);
        sleep_ms(1000);
        clearScreen();
        return 0;
    }
    *pin = atoi(pinStr);
    memset(pinStr, 0, sizeof(pinStr));
    return 1;
}

/* Read password with masking */
int readPassword(char *password, int maxLength) {
    int i = 0;
    char ch;

#ifdef _WIN32
    while (i < maxLength - 1) {
        ch = _getch();
        if (ch == '\r') break;
        if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != '\b') {
            password[i++] = ch;
            printf("#");
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (i < maxLength - 1) {
        ch = getchar();
        if (ch == '\n') break;
        if (ch == 127 && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != 127) {
            password[i++] = ch;
            printf("#");
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    password[i] = '\0';
    printf("\n");
    return i;
}

/* Get next account number */
int nextAccountNumber() {
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp) return START_ACC_NO;
    Account a;
    int last = START_ACC_NO - 1;
    fseek(fp, -((long)sizeof(Account)), SEEK_END);
    if (fread(&a, sizeof(a), 1, fp)) {
        last = a.accNo;
    }
    fclose(fp);
    return last + 1;
}

/* Get next fund request ID */
int nextFundReqId() {
    FILE *fp = fopen(REQ_FILE, "rb");
    if (!fp) return START_REQ_ID;
    FundRequest r;
    int last = START_REQ_ID - 1;
    fseek(fp, -((long)sizeof(FundRequest)), SEEK_END);
    if (fread(&r, sizeof(r), 1, fp)) {
        last = r.id;
    }
    fclose(fp);
    return last + 1;
}

/* Get next loan request ID */
int nextLoanReqId() {
    FILE *fp = fopen(LOAN_FILE, "rb");
    if (!fp) return START_REQ_ID;
    LoanRequest l;
    int last = START_REQ_ID - 1;
    fseek(fp, -((long)sizeof(LoanRequest)), SEEK_END);
    if (fread(&l, sizeof(l), 1, fp)) {
        last = l.id;
    }
    fclose(fp);
    return last + 1;
}

/* Get next checkbook request ID */
int nextCheckbookReqId() {
    FILE *fp = fopen(CHECKBOOK_REQ_FILE, "rb");
    if (!fp) return START_REQ_ID;
    CheckbookRequest cr;
    int last = START_REQ_ID - 1;
    fseek(fp, -((long)sizeof(CheckbookRequest)), SEEK_END);
    if (fread(&cr, sizeof(cr), 1, fp)) {
        last = cr.id;
    }
    fclose(fp);
    return last + 1;
}

/* Get next message ID */
int nextMessageId() {
    FILE *fp = fopen(MESSAGES_FILE, "rb");
    if (!fp) return START_REQ_ID;
    Message msg;
    int last = START_REQ_ID - 1;
    fseek(fp, -((long)sizeof(Message)), SEEK_END);
    if (fread(&msg, sizeof(msg), 1, fp)) {
        last = msg.id;
    }
    fclose(fp);
    return last + 1;
}

/* Save new account */
int saveNewAccount(Account *a) {
    FILE *fp = fopen(ACC_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open accounts file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(a, sizeof(*a), 1, fp) == 1;
    if (!result) {
        fprintf(stderr, "%sFailed to write account: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
    }
    fflush(fp);
    fclose(fp);
    return result;
}

/* Update existing account */
int updateAccount(Account *a) {
    FILE *fp = fopen(ACC_FILE, "rb+");
    if (!fp) {
        fprintf(stderr, "%sFailed to open accounts file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    Account tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp)) {
        if (tmp.accNo == a->accNo) {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            int result = fwrite(a, sizeof(*a), 1, fp) == 1;
            if (!result) {
                fprintf(stderr, "%sFailed to update account: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
            }
            fflush(fp);
            fclose(fp);
            return result;
        }
    }
    fclose(fp);
    return 0;
}

/* Load account by number */
int loadAccountByNo(int accNo, Account *out) {
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp) {
        fprintf(stderr, "%sFailed to open accounts file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    Account a;
    while (fread(&a, sizeof(a), 1, fp)) {
        if (a.accNo == accNo) {
            *out = a;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Delete account */
int deleteAccount(int accNo) {
    Account accounts[MAX_ACCOUNTS];
    int count = 0;
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp) return 0;
    while (fread(&accounts[count], sizeof(Account), 1, fp) && count < MAX_ACCOUNTS) {
        count++;
    }
    fclose(fp);

    int found = -1;
    for (int i = 0; i < count; i++) {
        if (accounts[i].accNo == accNo) {
            found = i;
            break;
        }
    }
    if (found == -1) return 0;

    // Shift array
    for (int i = found; i < count - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    count--;

    // Rewrite file
    fp = fopen(ACC_FILE, "wb");
    if (!fp) return 0;
    for (int i = 0; i < count; i++) {
        fwrite(&accounts[i], sizeof(Account), 1, fp);
    }
    fclose(fp);

    // Delete transaction log
    char txname[TX_FILENAME_BUF];
    snprintf(txname, sizeof(txname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    remove(txname);

    return 1;
}

/* Save new fund request */
int saveNewFundRequest(FundRequest *r) {
    FILE *fp = fopen(REQ_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open fund requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(r, sizeof(*r), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Update fund request */
int updateFundRequest(FundRequest *r) {
    FILE *fp = fopen(REQ_FILE, "rb+");
    if (!fp) {
        fprintf(stderr, "%sFailed to open fund requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    FundRequest tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp)) {
        if (tmp.id == r->id) {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            int result = fwrite(r, sizeof(*r), 1, fp) == 1;
            fflush(fp);
            fclose(fp);
            return result;
        }
    }
    fclose(fp);
    return 0;
}

/* Save new loan request */
int saveNewLoanRequest(LoanRequest *l) {
    FILE *fp = fopen(LOAN_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open loan requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(l, sizeof(*l), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Update loan request */
int updateLoanRequest(LoanRequest *l) {
    FILE *fp = fopen(LOAN_FILE, "rb+");
    if (!fp) {
        fprintf(stderr, "%sFailed to open loan requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    LoanRequest tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp)) {
        if (tmp.id == l->id) {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            int result = fwrite(l, sizeof(*l), 1, fp) == 1;
            fflush(fp);
            fclose(fp);
            return result;
        }
    }
    fclose(fp);
    return 0;
}

/* Save security questions */
int saveSecurityQA(SecurityQA *qa) {
    FILE *fp = fopen(SECURITY_QA_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open security QA file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(qa, sizeof(*qa), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Load security questions */
int loadSecurityQA(int accNo, SecurityQA *out) {
    FILE *fp = fopen(SECURITY_QA_FILE, "rb");
    if (!fp) return 0;
    SecurityQA qa;
    while (fread(&qa, sizeof(qa), 1, fp)) {
        if (qa.accNo == accNo) {
            *out = qa;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Save checkbook request */
int saveCheckbookRequest(CheckbookRequest *cr) {
    FILE *fp = fopen(CHECKBOOK_REQ_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open checkbook requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(cr, sizeof(*cr), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Update checkbook request */
int updateCheckbookRequest(CheckbookRequest *cr) {
    FILE *fp = fopen(CHECKBOOK_REQ_FILE, "rb+");
    if (!fp) {
        fprintf(stderr, "%sFailed to open checkbook requests file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    CheckbookRequest tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp)) {
        if (tmp.id == cr->id) {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            int result = fwrite(cr, sizeof(*cr), 1, fp) == 1;
            fflush(fp);
            fclose(fp);
            return result;
        }
    }
    fclose(fp);
    return 0;
}

/* Save message */
int saveMessage(Message *msg) {
    FILE *fp = fopen(MESSAGES_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open messages file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(msg, sizeof(*msg), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Update message */
int updateMessage(Message *msg) {
    FILE *fp = fopen(MESSAGES_FILE, "rb+");
    if (!fp) {
        fprintf(stderr, "%sFailed to open messages file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    Message tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp)) {
        if (tmp.id == msg->id) {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            int result = fwrite(msg, sizeof(*msg), 1, fp) == 1;
            fflush(fp);
            fclose(fp);
            return result;
        }
    }
    fclose(fp);
    return 0;
}

/* Save referral */
int saveReferral(Referral *ref) {
    FILE *fp = fopen(REFERRALS_FILE, "ab");
    if (!fp) {
        fprintf(stderr, "%sFailed to open referrals file: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return 0;
    }
    int result = fwrite(ref, sizeof(*ref), 1, fp) == 1;
    fflush(fp);
    fclose(fp);
    return result;
}

/* Log transaction */
void logTransaction(int accNo, const char *type, float amount, const char *note) {
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(fname, "a");
    if (!fp) {
        fprintf(stderr, "%sFailed to open transaction log: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
        return;
    }
    char d[DATE_BUF];
    nowstr(d, sizeof(d));
    fprintf(fp, "[%s] %s: %.2f", d, type, amount);
    if (note && strlen(note) > 0) fprintf(fp, " (%s)", note);
    fprintf(fp, "\n");
    fclose(fp);
}

/* Show mini statement */
void showMiniStatement(int accNo, int n) {
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("%sNo transactions found.%s\n", COLOR_RED, COLOR_RESET);
        waitForEnter();
        return;
    }
    char **lines = calloc(n, sizeof(char *));
    int count = 0;
    char buf[LINE_BUF];
    while (fgets(buf, sizeof(buf), fp)) {
        if (count < n) {
            lines[count] = strdup(buf);
        } else {
            free(lines[0]);
            for (int k = 0; k < n - 1; k++) lines[k] = lines[k + 1];
            lines[n - 1] = strdup(buf);
        }
        count++;
    }
    fclose(fp);
    printf("\n%s--- Mini Statement (Last %d) ---%s\n", COLOR_GOLD, n, COLOR_RESET);
    for (int i = 0; i < n && i < count; i++) {
        if (lines[i]) {
            printf("%s", lines[i]);
            free(lines[i]);
        }
    }
    for (int i = count; i < n; i++) {
        if (lines[i]) free(lines[i]);
    }
    free(lines);
    waitForEnter();
}

/* Show full transaction history */
void transactionHistory(int accNo) {
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("%sNo transactions found.%s\n", COLOR_RED, COLOR_RESET);
        waitForEnter();
        return;
    }
    printf("\n%s--- Transaction History ---%s\n", COLOR_GOLD, COLOR_RESET);
    char buf[LINE_BUF];
    while (fgets(buf, sizeof(buf), fp)) {
        printf("%s", buf);
    }
    fclose(fp);
    waitForEnter();
}

/* Search transactions by date range */
void searchTransactionsByDate(int accNo) {
    char startDate[12], endDate[12];
    printf("%sEnter start date (YYYY-MM-DD): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(startDate, sizeof(startDate), stdin);
    startDate[strcspn(startDate, "\n")] = '\0';

    printf("%sEnter end date (YYYY-MM-DD): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(endDate, sizeof(endDate), stdin);
    endDate[strcspn(endDate, "\n")] = '\0';

    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("%sNo transactions found.%s\n", COLOR_RED, COLOR_RESET);
        waitForEnter();
        return;
    }

    printf("\n%s--- Transactions from %s to %s ---%s\n", COLOR_GOLD, startDate, endDate, COLOR_RESET);
    char buf[LINE_BUF];
    int found = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        // Extract date from transaction line [YYYY-MM-DD HH:MM:SS]
        if (strlen(buf) >= 11) {
            char txDate[11];
            strncpy(txDate, buf + 1, 10);
            txDate[10] = '\0';

            if (strcmp(txDate, startDate) >= 0 && strcmp(txDate, endDate) <= 0) {
                printf("%s", buf);
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("%sNo transactions found in the specified date range.%s\n", COLOR_RED, COLOR_RESET);
    }
    waitForEnter();
}

/* Search transactions by amount range */
void searchTransactionsByAmount(int accNo) {
    float minAmount, maxAmount;
    printf("%sEnter minimum amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &minAmount) != 1) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    printf("%sEnter maximum amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &maxAmount) != 1) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("%sNo transactions found.%s\n", COLOR_RED, COLOR_RESET);
        waitForEnter();
        return;
    }

    printf("\n%s--- Transactions between %.2f and %.2f ---%s\n", COLOR_GOLD, minAmount, maxAmount, COLOR_RESET);
    char buf[LINE_BUF];
    int found = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        // Extract amount from transaction line
        char *amountStart = strstr(buf, ": ");
        if (amountStart) {
            amountStart += 2;
            float amount = atof(amountStart);
            if (amount >= minAmount && amount <= maxAmount) {
                printf("%s", buf);
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("%sNo transactions found in the specified amount range.%s\n", COLOR_RED, COLOR_RESET);
    }
    waitForEnter();
}

/* Download statement (save to file) */
void downloadStatement(int accNo) {
    char txFname[TX_FILENAME_BUF];
    snprintf(txFname, sizeof(txFname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *txFp = fopen(txFname, "r");
    if (!txFp) {
        printf("%sNo transactions found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    char stmtFname[TX_FILENAME_BUF];
    snprintf(stmtFname, sizeof(stmtFname), "%s/statement_%d.txt", DATA_DIR, accNo);
    FILE *stmtFp = fopen(stmtFname, "w");
    if (!stmtFp) {
        printf("%sFailed to create statement file.%s\n", COLOR_RED, COLOR_RESET);
        fclose(txFp);
        sleep_ms(1000);
        return;
    }
    char buf[LINE_BUF];
    while (fgets(buf, sizeof(buf), txFp)) {
        fputs(buf, stmtFp);
    }
    fclose(txFp);
    fclose(stmtFp);
    printf("%sStatement saved to %s.%s\n", COLOR_GREEN, stmtFname, COLOR_RESET);
    sleep_ms(1000);
}

/* About Annex Pay Information */
void aboutAnnexPay() {
    clearScreen();
    printf("%s=== About Annex Pay ===%s\n", COLOR_GOLD, COLOR_RESET);
    printf("Welcome to Annex Pay by team Hash Include.\n\n");

    printf("ANNEX PAY is a banking system written in the C programming language.\n");
    printf("It is developed by the Students of Bangladesh University of Business\n");
    printf("and Technology (BUBT). In this program, users can create accounts,\n");
    printf("deposit and withdraw money, transfer funds, pay bills, recharge mobile\n");
    printf("balance, and request loans or funds and others. There is also an admin\n");
    printf("panel where the admin can manage accounts, approve loans, and apply\n");
    printf("profit and many more...\n\n");

    printf("Why Choose Annex Pay?\n\n");

    printf("Security Features\n");
    printf("- Military-grade encryption for all transactions\n");
    printf("- Multi-layer authentication system\n");
    printf("- Real-time fraud detection and prevention\n");
    printf("- Secure PIN and security question protection\n\n");

    printf("Fast Services\n");
    printf("- Instant fund transfers and payments\n");
    printf("- Real-time transaction processing\n");
    printf("- Quick loan approvals\n");
    printf("- Immediate mobile recharges\n\n");

    printf("Financial Services\n");
    printf("- Competitive interest rates\n");
    printf("- Flexible loan options\n");
    printf("- Smart savings tools\n");
    printf("- Comprehensive financial tracking\n\n");

    printf("All-in-One Platform\n");
    printf("- Complete banking operations\n");
    printf("- Bill payments and utilities\n");
    printf("- Mobile recharges\n");
    printf("- Fund requests and management\n");
    printf("- Loan services and checkbook requests\n\n");

    printf("Key Features\n");
    printf("- Digital Wallet - Manage your money effortlessly\n");
    printf("- Smart Transfers - Send money anywhere, anytime\n");
    printf("- Bill Payments - Pay utilities in seconds\n");
    printf("- Loan Services - Quick access to financial support\n");
    printf("- 24/7 Support - Always here to help you\n");
    printf("- Referral Program - Earn while sharing with friends\n\n");

    printf("Our Commitment\n");
    printf("At Annex Pay, we're not just a banking service - we're your financial\n");
    printf("partner. We believe in transparency, innovation, and putting our\n");
    printf("customers first. Every feature is designed with your financial\n");
    printf("well-being in mind.\n\n");

    printf("Development Team\n");
    printf("Project: Annex Pay Banking System\n");
    printf("Course: Software Development Project I\n");
    printf("Institution: Bangladesh University of Business and Technology (BUBT)\n");
    printf("Department: Computer Science and Engineering (CSE)\n\n");

    printf("Team Hash Include Members & there contact info :\n\n");
    printf("1. Md. Monirul Hasan, | ID: 20255103087\n");
    printf("Email: monirul.hasan513@gmail.com\n\n");
    printf("2. Hafsa Akter,       | ID: 20255103119\n");
    printf("Email: raisa.kabir17@gmail.com\n\n");
    printf("3. Ahana Islam,       | ID: 20255103118\n");
    printf("Email: ahanaislam1632@gmail.com\n\n");
    printf("4. Bithi Rani Debi,   | ID: 20255103086\n");
    printf("Email: bithinath4056@gmail.com\n\n");
    printf("5. Arpita Deb Aishee, | ID: 20255103083\n");
    printf("Email: aishi.anuu@gmail.com\n\n");

    printf("Project Supervisor\n");
    printf("Ashfia Jannat Keya\n");
    printf("Lecturer, Department of Computer Science and Engineering\n");
    printf("Email: ashfiaj@bubt.edu.bd\n");
    printf("Bangladesh University of Business and Technology (BUBT)\n\n");

    printf("Helpline & Support\n");
    printf("web: bubt.edu.bd\n");
    printf("Email Support: info@bubt.edu.bd\n\n");

    printf("Service Hours\n");
    printf("Customer Service: 24/7\n");
    printf("Banking Operations: 6:00 AM - 12:00 AM (Local Time)\n");
    printf("Technical Support: 24/7\n");
    printf("Emergency Services: 24/7\n\n");

    printf("Quick Tips\n");
    printf("Keep your PIN confidential\n");
    printf("Never share OTPs with anyone\n");
    printf("Update contact details regularly\n");
    printf("Use official channels for support\n\n");

    printf("Your security and satisfaction are our top priorities!\n");

    waitForEnter();
}

/* Create new account - FIXED REFERRAL SYSTEM */
void createAccountInteractive() {
    clearScreen();
    Account a;
    a.accNo = nextAccountNumber();
    printf("%s=== Create New Account ===%s\n", COLOR_GOLD, COLOR_RESET);
    printf("%sAccount Number: %d%s\n", COLOR_WHITE, a.accNo, COLOR_RESET);

    char fullName[MAX_NAME];
    printf("%sEnter full name: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(fullName, sizeof(fullName), stdin);
    fullName[strcspn(fullName, "\n")] = '\0';

    if (!validateName(fullName)) {
        printf("%sInvalid name. Use only letters and single spaces.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1500);
        return;
    }

    strncpy(a.name, fullName, MAX_NAME - 1);
    a.name[MAX_NAME - 1] = '\0';

    // Ask for referral code with Yes/No option
    printf("%sDo you have a referral code? (Yes/No): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    char response[10];
    fgets(response, sizeof(response), stdin);
    response[strcspn(response, "\n")] = '\0';

    if (strcasecmp(response, "yes") == 0 || strcasecmp(response, "y") == 0) {
        printf("%sEnter referral code: %s", COLOR_WHITE, COLOR_RESET);
        fflush(stdout);
        int referralCode;
        if (scanf("%d", &referralCode) != 1) {
            printf("%sInvalid referral code. Continuing without referral.%s\n", COLOR_RED, COLOR_RESET);
            a.referredBy = 0;
        } else {
            Account referrer;
            if (loadAccountByNo(referralCode, &referrer)) {
                a.referredBy = referralCode;
                printf("%sReferral code applied!%s\n", COLOR_GREEN, COLOR_RESET);
            } else {
                printf("%sInvalid referral code. Continuing without referral.%s\n", COLOR_RED, COLOR_RESET);
                a.referredBy = 0;
            }
        }
        clearInputBuffer();
    } else {
        a.referredBy = 0;
    }

    if (!readPin("Enter 4-digit PIN: ", &a.pin)) return;

    a.balance = 0.0f;
    a.wrongAttempts = 0;
    a.locked = 0;

    showLoading("Creating account...", 1500);
    if (saveNewAccount(&a)) {
        printf("%sAccount created successfully!%s\n", COLOR_GREEN, COLOR_RESET);
        printf("Account Number: %d\nName: %s\n", a.accNo, a.name);

        // FIXED: Give referral bonus to BOTH referrer and new user if applicable
        if (a.referredBy > 0) {
            Account referrer;
            if (loadAccountByNo(a.referredBy, &referrer)) {
                // Give bonus to referrer
                referrer.balance += REFERRAL_BONUS;
                updateAccount(&referrer);
                logTransaction(a.referredBy, "Referral Bonus", REFERRAL_BONUS, "Referred new user");

                // FIX: Give bonus to new user as well
                a.balance += REFERRAL_BONUS;
                updateAccount(&a);
                logTransaction(a.accNo, "Referral Bonus", REFERRAL_BONUS, "Signed up with referral");

                // Save referral record
                Referral ref;
                ref.referrerAcc = a.referredBy;
                ref.referredAcc = a.accNo;
                getCurrentDate(ref.referralDate, sizeof(ref.referralDate));
                ref.bonusGiven = 1;
                saveReferral(&ref);

                printf("%sReferral bonus of %.2f given to BOTH accounts!%s\n",
                       COLOR_GREEN, REFERRAL_BONUS, COLOR_RESET);
                printf("%s- Referrer (%d) received: %.2f%s\n", COLOR_YELLOW, a.referredBy, REFERRAL_BONUS, COLOR_RESET);
                printf("%s- New user (%d) received: %.2f%s\n", COLOR_YELLOW, a.accNo, REFERRAL_BONUS, COLOR_RESET);
            }
        }
    } else {
        printf("%sFailed to create account.%s\n", COLOR_RED, COLOR_RESET);
    }
    waitForEnter();
}

/* Authenticate account */
int authenticateAccount(int accNo, Account *out) {
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        return 0;
    }
    if (a.locked) {
        printf("%sAccount is locked. Contact admin.%s\n", COLOR_RED, COLOR_RESET);
        return 0;
    }
    int pinTry;
    if (!readPin("Enter PIN: ", &pinTry)) return 0;
    if (pinTry == a.pin) {
        a.wrongAttempts = 0;
        updateAccount(&a);
        *out = a;
        return 1;
    } else {
        a.wrongAttempts++;
        printf("%sIncorrect PIN. Attempt %d/3.%s\n", COLOR_RED, a.wrongAttempts, COLOR_RESET);
        if (a.wrongAttempts >= 3) {
            a.locked = 1;
            printf("%sAccount locked due to 3 failed attempts.%s\n", COLOR_RED, COLOR_RESET);
        }
        updateAccount(&a);
        sleep_ms(1000);
        return 0;
    }
}

/* Change PIN */
void changePIN(int accNo) {
    clearScreen();
    printf("%s=== Change PIN ===%s\n", COLOR_GOLD, COLOR_RESET);
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    int oldPin;
    if (!readPin("Enter current PIN: ", &oldPin)) return;
    if (oldPin != a.pin) {
        printf("%sIncorrect current PIN.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    int newPin;
    if (!readPin("Enter new 4-digit PIN: ", &newPin)) return;
    showLoading("Updating PIN...", 1000);
    a.pin = newPin;
    if (updateAccount(&a)) {
        printf("%sPIN changed successfully.%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("%sFailed to update PIN.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Deposit */
void depositInteractive(int accNo) {
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    float amt;
    printf("%sEnter deposit amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    showLoading("Processing deposit...", 1000);
    a.balance += amt;
    updateAccount(&a);
    logTransaction(accNo, "Deposit", amt, "");
    printf("%sDeposit successful. New balance: %.2f%s\n", COLOR_GREEN, a.balance, COLOR_RESET);
    sleep_ms(1000);
}

/* Withdraw */
void withdrawInteractive(int accNo) {
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    resetDailyLimitIfNewDay(accNo);
    float todayWithdrawn = getTodayWithdrawn(accNo);

    float amt;
    printf("%sEnter withdrawal amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();

    if (amt > a.balance) {
        printf("%sInsufficient balance.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1500);
        return;
    }
    if (todayWithdrawn + amt > MAX_DAILY_WITHDRAW) {
        printf("%sDaily withdrawal limit exceeded!\n", COLOR_RED);
        printf("Limit: %.2f | Withdrawn today: %.2f | Available: %.2f%s\n",
               MAX_DAILY_WITHDRAW, todayWithdrawn, MAX_DAILY_WITHDRAW - todayWithdrawn, COLOR_RESET);
        sleep_ms(2000);
        return;
    }

    showLoading("Processing withdrawal...", 1000);
    a.balance -= amt;
    updateAccount(&a);
    addWithdrawalToDaily(accNo, amt);
    logTransaction(accNo, "Withdraw", amt, "ATM/Cash");
    printf("%sWithdrawal successful! New balance: %.2f%s\n", COLOR_GREEN, a.balance, COLOR_RESET);
    printf("%sDaily limit used: %.2f / %.2f%s\n", COLOR_YELLOW, todayWithdrawn + amt, MAX_DAILY_WITHDRAW, COLOR_RESET);
    sleep_ms(1500);
}

/* Transfer */
void transferInteractive(int fromAcc) {
    Account src;
    if (!loadAccountByNo(fromAcc, &src)) {
        printf("%sSource account not found.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    int toAcc;
    printf("%sEnter receiver account number: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &toAcc) != 1 || toAcc <= 0) {
        printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    Account tgt;
    if (!loadAccountByNo(toAcc, &tgt)) {
        printf("%sReceiver account not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    float amt;
    printf("%sEnter amount to transfer: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    if (amt > src.balance) {
        printf("%sInsufficient balance.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    showLoading("Processing transfer...", 1500);
    src.balance -= amt;
    tgt.balance += amt;
    updateAccount(&src);
    updateAccount(&tgt);
    char note[100];
    snprintf(note, sizeof(note), "To %d", toAcc);
    logTransaction(fromAcc, "Transfer Out", amt, note);
    snprintf(note, sizeof(note), "From %d", fromAcc);
    logTransaction(toAcc, "Transfer In", amt, note);
    printf("%sTransfer successful. New balance: %.2f%s\n", COLOR_GREEN, src.balance, COLOR_RESET);
    sleep_ms(1000);
}

/* Bill Payment */
void billPayment(int accNo) {
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    printf("%sBill types:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("1) Electricity\n");
    printf("2) Water\n");
    printf("3) Internet\n");
    printf("4) University Fees\n");
    printf("5) Other Fees\n");
    printf("%sSelect bill type: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    int billChoice;
    if (scanf("%d", &billChoice) != 1 || billChoice < 1 || billChoice > 5) {
        printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    char billType[50];
    char reference[50] = {0};
    char note[100];
    switch (billChoice) {
        case 1:
            strcpy(billType, "Electricity");
            printf("%sEnter Meter Number: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            fgets(reference, sizeof(reference), stdin);
            reference[strcspn(reference, "\n")] = '\0';
            break;
        case 2:
            strcpy(billType, "Water");
            printf("%sEnter ID Number: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            fgets(reference, sizeof(reference), stdin);
            reference[strcspn(reference, "\n")] = '\0';
            break;
        case 3:
            strcpy(billType, "Internet");
            printf("%sEnter Service ID: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            fgets(reference, sizeof(reference), stdin);
            reference[strcspn(reference, "\n")] = '\0';
            break;
        case 4:
            strcpy(billType, "University Fees");
            printf("%sEnter Student ID: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            fgets(reference, sizeof(reference), stdin);
            reference[strcspn(reference, "\n")] = '\0';
            break;
        case 5:
            strcpy(billType, "Other Fees");
            printf("%sEnter fees type: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            fgets(reference, sizeof(reference), stdin);
            reference[strcspn(reference, "\n")] = '\0';
            if (strlen(reference) == 0) {
                strcpy(reference, "General");
            }
            break;
    }
    float amt;
    printf("%sEnter bill amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    if (amt > a.balance) {
        printf("%sInsufficient balance.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    if (billChoice == 5) {
        snprintf(note, sizeof(note), "Bill Payment: %s - %s", billType, reference);
    } else if (strlen(reference) > 0) {
        snprintf(note, sizeof(note), "Bill Payment: %s - Ref: %s", billType, reference);
    } else {
        snprintf(note, sizeof(note), "Bill Payment: %s", billType);
    }
    showLoading("Processing payment...", 1000);
    a.balance -= amt;
    updateAccount(&a);
    logTransaction(accNo, "Payment", amt, note);
    printf("%sBill payment successful. New balance: %.2f%s\n", COLOR_GREEN, a.balance, COLOR_RESET);
    sleep_ms(1000);
}

/* Mobile Recharge */
void mobileRecharge(int accNo) {
    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    char mobile[20];
    printf("%sEnter mobile number: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(mobile, sizeof(mobile), stdin);
    mobile[strcspn(mobile, "\n")] = '\0';
    float amt;
    printf("%sEnter recharge amount: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    if (amt > a.balance) {
        printf("%sInsufficient balance.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    showLoading("Processing recharge...", 1000);
    a.balance -= amt;
    updateAccount(&a);
    char note[100];
    snprintf(note, sizeof(note), "Mobile Recharge: %s", mobile);
    logTransaction(accNo, "Recharge", amt, note);
    printf("%sMobile recharge successful. New balance: %.2f%s\n", COLOR_GREEN, a.balance, COLOR_RESET);
    sleep_ms(1000);
}

/* Fund Request */
void fundRequest(int accNo) {
    int payerAcc;
    printf("%sEnter payer account number (who will send funds): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &payerAcc) != 1 || payerAcc <= 0) {
        printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    Account payer;
    if (!loadAccountByNo(payerAcc, &payer)) {
        printf("%sPayer account not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    float amt;
    printf("%sEnter amount to request: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    char note[100];
    printf("%sEnter note (optional): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(note, sizeof(note), stdin);
    note[strcspn(note, "\n")] = '\0';
    FundRequest r;
    r.id = nextFundReqId();
    r.fromAcc = accNo;  // Requester
    r.toAcc = payerAcc; // Payer
    r.amount = amt;
    strncpy(r.note, note, sizeof(r.note) - 1);
    r.note[sizeof(r.note) - 1] = '\0';
    r.status = 0;  // Pending
    showLoading("Sending request...", 1000);
    if (saveNewFundRequest(&r)) {
        printf("%sFund request sent successfully. ID: %d%s\n", COLOR_GREEN, r.id, COLOR_RESET);
    } else {
        printf("%sFailed to send fund request.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Process Requests */
void processRequests(int accNo) {
    FILE *fp = fopen(REQ_FILE, "rb+");
    if (!fp) {
        printf("%sNo fund requests found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }
    printf("\n%s--- Pending Fund Requests ---%s\n", COLOR_GOLD, COLOR_RESET);
    FundRequest r;
    int found = 0;
    while (fread(&r, sizeof(r), 1, fp)) {
        if (r.toAcc == accNo && r.status == 0) {
            printf("ID: %d | From: %d | Amount: %.2f | Note: %s\n", r.id, r.fromAcc, r.amount, r.note);
            found = 1;
        }
    }
    if (!found) {
        printf("%sNo pending requests.%s\n", COLOR_RED, COLOR_RESET);
        fclose(fp);
        sleep_ms(1000);
        return;
    }
    int reqId;
    printf("%sEnter request ID to process: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &reqId) != 1) {
        printf("%sInvalid ID.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        fclose(fp);
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    rewind(fp);
    found = 0;
    while (fread(&r, sizeof(r), 1, fp)) {
        if (r.id == reqId && r.toAcc == accNo && r.status == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("%sRequest not found or not pending.%s\n", COLOR_RED, COLOR_RESET);
        fclose(fp);
        sleep_ms(1000);
        return;
    }
    Account payer, requester;
    if (!loadAccountByNo(accNo, &payer) || !loadAccountByNo(r.fromAcc, &requester)) {
        printf("%sAccount error.%s\n", COLOR_RED, COLOR_RESET);
        fclose(fp);
        sleep_ms(1000);
        return;
    }
    printf("1) Approve\n2) Deny\n%sChoice: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        fclose(fp);
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    if (choice == 1) {
        if (r.amount > payer.balance) {
            printf("%sInsufficient balance to approve.%s\n", COLOR_RED, COLOR_RESET);
            fclose(fp);
            sleep_ms(1000);
            return;
        }
        showLoading("Approving request...", 1000);
        payer.balance -= r.amount;
        requester.balance += r.amount;
        updateAccount(&payer);
        updateAccount(&requester);
        char note[100];
        snprintf(note, sizeof(note), "Fund Request Approved to %d", r.fromAcc);
        logTransaction(accNo, "Transfer Out", r.amount, note);
        snprintf(note, sizeof(note), "Fund Request Approved from %d", accNo);
        logTransaction(r.fromAcc, "Transfer In", r.amount, note);
        r.status = 1;
        fseek(fp, -((long)sizeof(r)), SEEK_CUR);
        fwrite(&r, sizeof(r), 1, fp);
        printf("%sRequest approved.%s\n", COLOR_GREEN, COLOR_RESET);
    } else if (choice == 2) {
        showLoading("Denying request...", 500);
        r.status = 2;
        fseek(fp, -((long)sizeof(r)), SEEK_CUR);
        fwrite(&r, sizeof(r), 1, fp);
        printf("%sRequest denied.%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
    }
    fclose(fp);
    sleep_ms(1000);
}

/* Request Loan */
void requestLoan(int accNo) {
    float amt;
    printf("%sEnter loan amount (max %.2f): %s", COLOR_WHITE, MAX_LOAN_AMOUNT, COLOR_RESET);
    fflush(stdout);
    if (scanf("%f", &amt) != 1 || amt <= 0 || amt > MAX_LOAN_AMOUNT) {
        printf("%sInvalid amount.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    int tenure;
    printf("%sEnter tenure in months (max %d): %s", COLOR_WHITE, MAX_TENURE_MONTHS, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &tenure) != 1 || tenure <= 0 || tenure > MAX_TENURE_MONTHS) {
        printf("%sInvalid tenure.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();
    LoanRequest l;
    l.id = nextLoanReqId();
    l.accNo = accNo;
    l.amount = amt;
    l.tenureMonths = tenure;
    l.status = 0;  // Pending
    showLoading("Submitting loan request...", 1500);
    if (saveNewLoanRequest(&l)) {
        printf("%sLoan request submitted. ID: %d%s\n", COLOR_GREEN, l.id, COLOR_RESET);
    } else {
        printf("%sFailed to submit loan request.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Security Questions Setup */
void setupSecurityQuestions(int accNo) {
    clearScreen();
    printf("%s=== Security Questions Setup ===%s\n", COLOR_GOLD, COLOR_RESET);

    SecurityQA qa;
    qa.accNo = accNo;

    strcpy(qa.question, "What was the name of your first pet?");
    printf("%sSecurity Question: %s%s\n", COLOR_WHITE, qa.question, COLOR_RESET);
    printf("%sAnswer: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(qa.answer, sizeof(qa.answer), stdin);
    qa.answer[strcspn(qa.answer, "\n")] = '\0';

    showLoading("Saving security question...", 1000);
    if (saveSecurityQA(&qa)) {
        printf("%sSecurity question set up successfully!%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("%sFailed to save security question.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Account Recovery using Security Questions */
void accountRecovery() {
    clearScreen();
    printf("%s=== Account Recovery ===%s\n", COLOR_GOLD, COLOR_RESET);

    int accNo;
    printf("%sEnter your account number: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &accNo) != 1) {
        printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();

    SecurityQA qa;
    if (!loadSecurityQA(accNo, &qa)) {
        printf("%sNo security question found for this account.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    printf("%sAnswer the following security question:%s\n", COLOR_WHITE, COLOR_RESET);

    char answer[50];
    printf("%s%s%s\n", COLOR_YELLOW, qa.question, COLOR_RESET);
    printf("%sYour answer: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(answer, sizeof(answer), stdin);
    answer[strcspn(answer, "\n")] = '\0';

    if (strcasecmp(answer, qa.answer) != 0) {
        printf("%sIncorrect answer.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    // Answer is correct, allow PIN reset
    printf("%sSecurity question answered correctly!%s\n", COLOR_GREEN, COLOR_RESET);
    int newPin;
    if (readPin("Enter new 4-digit PIN: ", &newPin)) {
        a.pin = newPin;
        a.locked = 0;
        a.wrongAttempts = 0;
        if (updateAccount(&a)) {
            printf("%sPIN reset successfully! You can now login with your new PIN.%s\n", COLOR_GREEN, COLOR_RESET);
        } else {
            printf("%sFailed to reset PIN.%s\n", COLOR_RED, COLOR_RESET);
        }
    }
    sleep_ms(1000);
}

/* Checkbook Request */
void requestCheckbook(int accNo) {
    clearScreen();
    printf("%s=== Checkbook Request ===%s\n", COLOR_GOLD, COLOR_RESET);

    int leaves;
    printf("%sEnter number of leaves (25/50/100): %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    if (scanf("%d", &leaves) != 1 || (leaves != 25 && leaves != 50 && leaves != 100)) {
        printf("%sInvalid number of leaves. Choose 25, 50 or 100.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
        sleep_ms(1000);
        return;
    }
    clearInputBuffer();

    CheckbookRequest cr;
    cr.id = nextCheckbookReqId();
    cr.accNo = accNo;
    cr.numberOfLeaves = leaves;
    getCurrentDate(cr.requestDate, sizeof(cr.requestDate));
    cr.status = 0; // Pending

    showLoading("Submitting checkbook request...", 1000);
    if (saveCheckbookRequest(&cr)) {
        printf("%sCheckbook request submitted successfully! ID: %d%s\n", COLOR_GREEN, cr.id, COLOR_RESET);
    } else {
        printf("%sFailed to submit checkbook request.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Send Message to Admin */
void sendMessageToAdmin(int accNo) {
    clearScreen();
    printf("%s=== Send Message to Admin ===%s\n", COLOR_GOLD, COLOR_RESET);

    char message[500];
    printf("%sEnter your message: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    if (strlen(message) == 0) {
        printf("%sMessage cannot be empty.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    Message msg;
    msg.id = nextMessageId();
    msg.fromAcc = accNo;
    msg.toAcc = 0; // 0 represents admin
    strncpy(msg.message, message, sizeof(msg.message) - 1);
    msg.message[sizeof(msg.message) - 1] = '\0';
    nowstr(msg.timestamp, sizeof(msg.timestamp));
    msg.readStatus = 0; // Unread

    showLoading("Sending message...", 1000);
    if (saveMessage(&msg)) {
        printf("%sMessage sent to admin successfully!%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("%sFailed to send message.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* View Messages */
void viewMessages(int accNo) {
    FILE *fp = fopen(MESSAGES_FILE, "rb+");
    if (!fp) {
        printf("%sNo messages found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    printf("\n%s--- Messages ---%s\n", COLOR_GOLD, COLOR_RESET);
    Message msg;
    int found = 0;
    while (fread(&msg, sizeof(msg), 1, fp)) {
        if (msg.toAcc == accNo || (accNo == 0 && msg.fromAcc != 0)) {
            // For admin: show messages from users, for users: show messages from admin
            if ((accNo == 0 && msg.fromAcc != 0) || (accNo != 0 && msg.toAcc == accNo)) {
                printf("ID: %d | From: %d | Time: %s\n", msg.id, msg.fromAcc, msg.timestamp);
                printf("Message: %s\n", msg.message);
                printf("Status: %s\n\n", msg.readStatus ? "Read" : "Unread");
                found = 1;

                // Mark as read
                if (msg.readStatus == 0) {
                    msg.readStatus = 1;
                    fseek(fp, -((long)sizeof(msg)), SEEK_CUR);
                    fwrite(&msg, sizeof(msg), 1, fp);
                    fseek(fp, 0, SEEK_CUR); // Reset position
                }
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("%sNo messages found.%s\n", COLOR_RED, COLOR_RESET);
    }
    waitForEnter();
}

/* Reply to Message (Admin only) */
void replyToMessage(int messageId) {
    clearScreen();
    printf("%s=== Reply to Message ===%s\n", COLOR_GOLD, COLOR_RESET);

    FILE *fp = fopen(MESSAGES_FILE, "rb");
    if (!fp) {
        printf("%sMessage not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    Message originalMsg;
    int found = 0;
    while (fread(&originalMsg, sizeof(originalMsg), 1, fp)) {
        if (originalMsg.id == messageId && originalMsg.fromAcc != 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("%sMessage not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    printf("Original message from account %d:\n", originalMsg.fromAcc);
    printf("%s\n\n", originalMsg.message);

    char reply[500];
    printf("%sEnter your reply: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);
    fgets(reply, sizeof(reply), stdin);
    reply[strcspn(reply, "\n")] = '\0';

    if (strlen(reply) == 0) {
        printf("%sReply cannot be empty.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    Message replyMsg;
    replyMsg.id = nextMessageId();
    replyMsg.fromAcc = 0; // Admin
    replyMsg.toAcc = originalMsg.fromAcc;
    strncpy(replyMsg.message, reply, sizeof(replyMsg.message) - 1);
    replyMsg.message[sizeof(replyMsg.message) - 1] = '\0';
    nowstr(replyMsg.timestamp, sizeof(replyMsg.timestamp));
    replyMsg.readStatus = 0; // Unread

    showLoading("Sending reply...", 1000);
    if (saveMessage(&replyMsg)) {
        printf("%sReply sent successfully!%s\n", COLOR_GREEN, COLOR_RESET);
    } else {
        printf("%sFailed to send reply.%s\n", COLOR_RED, COLOR_RESET);
    }
    sleep_ms(1000);
}

/* Refer a User - UPDATED MESSAGE */
void referAUser(int accNo) {
    clearScreen();
    printf("%s=== Refer a User ===%s\n", COLOR_GOLD, COLOR_RESET);

    Account a;
    if (!loadAccountByNo(accNo, &a)) {
        printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    printf("%sYour referral code: %d%s\n", COLOR_GREEN, accNo, COLOR_RESET);
    printf("%sShare this code with your friends!%s\n", COLOR_WHITE, COLOR_RESET);
    printf("%sWhen they create an account using your code, you BOTH get %.2f bonus!%s\n",
           COLOR_YELLOW, REFERRAL_BONUS, COLOR_RESET);

    waitForEnter();
}

/* Check admin password */
int checkAdminPassword(const char *input) {
    return strcmp(input, "bubt1234") == 0;
}

/* Admin Panel */
void adminPanel() {
    clearScreen();
    printf("%s=== Admin Panel ===%s\n", COLOR_GOLD, COLOR_RESET);

    char pass[50];
    printf("%sEnter admin password: %s", COLOR_WHITE, COLOR_RESET);
    fflush(stdout);

    readPassword(pass, sizeof(pass));

    if (!checkAdminPassword(pass)) {
        printf("%sIncorrect password.%s\n", COLOR_RED, COLOR_RESET);
        sleep_ms(1000);
        return;
    }

    showLoading("Authenticating...", 1000);
    printf("%sLogin successful.%s\n", COLOR_GREEN, COLOR_RESET);
    sleep_ms(1000);

    while (1) {
        clearScreen();
        printf("%s=== Admin Menu ===%s\n", COLOR_GOLD, COLOR_RESET);
        printf("1) List All Accounts\n2) Unlock Account\n3) Process Loan Requests\n");
        printf("4) Delete Account\n5) Process Checkbook Requests\n6) View Messages\n");
        printf("7) Reply to Message\n8) Logout\n");
        printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
        fflush(stdout);
        int ch;
        if (scanf("%d", &ch) != 1) {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            clearInputBuffer();
            sleep_ms(1000);
            continue;
        }
        clearInputBuffer();

        if (ch == 1) {
            FILE *fp = fopen(ACC_FILE, "rb");
            if (!fp) {
                printf("%sNo accounts found.%s\n", COLOR_RED, COLOR_RESET);
                waitForEnter();
                continue;
            }
            printf("\n%s--- All Accounts ---%s\n", COLOR_GOLD, COLOR_RESET);
            Account a;
            while (fread(&a, sizeof(a), 1, fp)) {
                printf("AccNo: %d | Name: %s | Balance: %.2f | Locked: %d\n", a.accNo, a.name, a.balance, a.locked);
            }
            fclose(fp);
            waitForEnter();
        } else if (ch == 2) {
            int accNo;
            printf("%sEnter account number to unlock: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &accNo) != 1 || accNo <= 0) {
                printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            Account a;
            if (!loadAccountByNo(accNo, &a)) {
                printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            showLoading("Unlocking account...", 500);
            a.locked = 0;
            a.wrongAttempts = 0;
            updateAccount(&a);
            printf("%sAccount unlocked.%s\n", COLOR_GREEN, COLOR_RESET);
            sleep_ms(1000);
        } else if (ch == 3) {
            FILE *fp = fopen(LOAN_FILE, "rb+");
            if (!fp) {
                printf("%sNo loan requests found.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            printf("\n%s--- Pending Loan Requests ---%s\n", COLOR_GOLD, COLOR_RESET);
            LoanRequest l;
            int found = 0;
            while (fread(&l, sizeof(l), 1, fp)) {
                if (l.status == 0) {
                    printf("ID: %d | Acc: %d | Amount: %.2f | Tenure: %d\n", l.id, l.accNo, l.amount, l.tenureMonths);
                    found = 1;
                }
            }
            if (!found) {
                printf("%sNo pending requests.%s\n", COLOR_RED, COLOR_RESET);
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            int reqId;
            printf("%sEnter request ID to process: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &reqId) != 1) {
                printf("%sInvalid ID.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            rewind(fp);
            found = 0;
            while (fread(&l, sizeof(l), 1, fp)) {
                if (l.id == reqId && l.status == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("%sRequest not found or not pending.%s\n", COLOR_RED, COLOR_RESET);
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            Account a;
            if (!loadAccountByNo(l.accNo, &a)) {
                printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            printf("1) Approve\n2) Deny\n%sChoice: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            int choice;
            if (scanf("%d", &choice) != 1) {
                printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            if (choice == 1) {
                showLoading("Approving loan...", 1000);
                a.balance += l.amount;
                updateAccount(&a);
                char note[100];
                snprintf(note, sizeof(note), "Loan Approved: %.2f for %d months", l.amount, l.tenureMonths);
                logTransaction(l.accNo, "Loan In", l.amount, note);
                l.status = 1;
                fseek(fp, -((long)sizeof(l)), SEEK_CUR);
                fwrite(&l, sizeof(l), 1, fp);
                printf("%sLoan approved.%s\n", COLOR_GREEN, COLOR_RESET);
            } else if (choice == 2) {
                showLoading("Denying loan...", 500);
                l.status = 2;
                fseek(fp, -((long)sizeof(l)), SEEK_CUR);
                fwrite(&l, sizeof(l), 1, fp);
                printf("%sLoan denied.%s\n", COLOR_GREEN, COLOR_RESET);
            } else {
                printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            }
            fclose(fp);
            sleep_ms(1000);
        } else if (ch == 4) {
            int accNo;
            printf("%sEnter account number to delete: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &accNo) != 1 || accNo <= 0) {
                printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            Account a;
            if (!loadAccountByNo(accNo, &a)) {
                printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            printf("%sAre you sure you want to delete account %d (%s)? This action cannot be undone. (y/n): %s", COLOR_RED, accNo, a.name, COLOR_RESET);
            fflush(stdout);
            char confirm;
            scanf(" %c", &confirm);
            clearInputBuffer();
            if (confirm == 'y' || confirm == 'Y') {
                if (deleteAccount(accNo)) {
                    printf("%sAccount deleted successfully.%s\n", COLOR_GREEN, COLOR_RESET);
                } else {
                    printf("%sFailed to delete account.%s\n", COLOR_RED, COLOR_RESET);
                }
            } else {
                printf("%sDeletion cancelled.%s\n", COLOR_YELLOW, COLOR_RESET);
            }
            sleep_ms(1000);
        } else if (ch == 5) {
            FILE *fp = fopen(CHECKBOOK_REQ_FILE, "rb+");
            if (!fp) {
                printf("%sNo checkbook requests found.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            printf("\n%s--- Pending Checkbook Requests ---%s\n", COLOR_GOLD, COLOR_RESET);
            CheckbookRequest cr;
            int found = 0;
            while (fread(&cr, sizeof(cr), 1, fp)) {
                if (cr.status == 0) {
                    printf("ID: %d | Acc: %d | Leaves: %d | Date: %s\n", cr.id, cr.accNo, cr.numberOfLeaves, cr.requestDate);
                    found = 1;
                }
            }
            if (!found) {
                printf("%sNo pending requests.%s\n", COLOR_RED, COLOR_RESET);
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            int reqId;
            printf("%sEnter request ID to process: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &reqId) != 1) {
                printf("%sInvalid ID.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            rewind(fp);
            found = 0;
            while (fread(&cr, sizeof(cr), 1, fp)) {
                if (cr.id == reqId && cr.status == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("%sRequest not found or not pending.%s\n", COLOR_RED, COLOR_RESET);
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            printf("1) Approve\n2) Deny\n%sChoice: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            int choice;
            if (scanf("%d", &choice) != 1) {
                printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                fclose(fp);
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            if (choice == 1) {
                showLoading("Approving checkbook request...", 1000);
                cr.status = 1;
                fseek(fp, -((long)sizeof(cr)), SEEK_CUR);
                fwrite(&cr, sizeof(cr), 1, fp);
                printf("%sCheckbook request approved.%s\n", COLOR_GREEN, COLOR_RESET);
            } else if (choice == 2) {
                showLoading("Denying checkbook request...", 500);
                cr.status = 2;
                fseek(fp, -((long)sizeof(cr)), SEEK_CUR);
                fwrite(&cr, sizeof(cr), 1, fp);
                printf("%sCheckbook request denied.%s\n", COLOR_GREEN, COLOR_RESET);
            } else {
                printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            }
            fclose(fp);
            sleep_ms(1000);
        } else if (ch == 6) {
            viewMessages(0); // 0 for admin
        } else if (ch == 7) {
            int msgId;
            printf("%sEnter message ID to reply: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &msgId) != 1) {
                printf("%sInvalid message ID.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();
            replyToMessage(msgId);
        } else if (ch == 8) {
            break;
        } else {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            sleep_ms(1000);
        }
    }
}

/* Calculate AnnexPay Points */
int calculateAnnexPayPoints(int accNo) {
    int points = 750;  // base

    Account a;
    if (!loadAccountByNo(accNo, &a)) return 300;

    if (a.balance > 10000) points += 100;
    if (a.balance < 500) points -= 150;

    char txname[TX_FILENAME_BUF];
    snprintf(txname, sizeof(txname), "%s/acc_%d_tx.txt", DATA_DIR, accNo);
    FILE *fp = fopen(txname, "r");
    if (fp) {
        char line[256];
        int billPayments = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "Payment") || strstr(line, "Recharge")) billPayments++;
        }
        fclose(fp);
        points += billPayments * 5;
    }

    if (points > 900) points = 900;
    if (points < 300) points = 300;
    return points;
}

/* Reset daily limit if new day */
void resetDailyLimitIfNewDay(int accNo) {
    FILE *fp = fopen(DAILY_LIMIT_FILE, "rb+");
    if (!fp) {
        fp = fopen(DAILY_LIMIT_FILE, "wb");
        if (!fp) return;
        fclose(fp);
        return;
    }

    DailyLimit dl;
    int found = 0;
    long pos = 0;
    char today[12];
    time_t t = time(NULL);
    strftime(today, sizeof(today), "%Y-%m-%d", localtime(&t));

    while (fread(&dl, sizeof(dl), 1, fp)) {
        if (dl.accNo == accNo) {
            found = 1;
            if (strcmp(dl.lastDate, today) != 0) {
                dl.todayWithdrawn = 0;
                strcpy(dl.lastDate, today);
                fseek(fp, pos, SEEK_SET);
                fwrite(&dl, sizeof(dl), 1, fp);
            }
            break;
        }
        pos = ftell(fp);
    }
    if (!found) {
        dl.accNo = accNo;
        dl.todayWithdrawn = 0;
        strcpy(dl.lastDate, today);
        fseek(fp, 0, SEEK_END);
        fwrite(&dl, sizeof(dl), 1, fp);
    }
    fclose(fp);
}

/* Get today withdrawn */
float getTodayWithdrawn(int accNo) {
    FILE *fp = fopen(DAILY_LIMIT_FILE, "rb");
    if (!fp) return 0;
    DailyLimit dl;
    while (fread(&dl, sizeof(dl), 1, fp)) {
        if (dl.accNo == accNo) {
            fclose(fp);
            return dl.todayWithdrawn;
        }
    }
    fclose(fp);
    return 0;
}

/* Add withdrawal to daily */
void addWithdrawalToDaily(int accNo, float amt) {
    FILE *fp = fopen(DAILY_LIMIT_FILE, "rb+");
    if (!fp) return;
    DailyLimit dl;
    while (fread(&dl, sizeof(dl), 1, fp)) {
        if (dl.accNo == accNo) {
            dl.todayWithdrawn += amt;
            // FIXED: Added cast to fix overflow warning
            fseek(fp, -(long)sizeof(dl), SEEK_CUR);
            fwrite(&dl, sizeof(dl), 1, fp);
            break;
        }
    }
    fclose(fp);
}

/* ======================================================================
   ROCK PAPER SCISSORS GAME INTEGRATION
   ====================================================================== */

/* Game utility functions */
#ifdef _WIN32
void set_game_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#else
void set_game_color(int color) {
    // For non-Windows systems, use ANSI codes (simplified)
    printf("\033[1;%dm", color);
}
#endif

const char* game_choice_to_string(char choice) {
    switch (choice) {
        case 'R': return "Rock";
        case 'P': return "Paper";
        case 'S': return "Scissors";
        default: return "Invalid";
    }
}

void game_display_score(int user_wins, int annex_pay_wins, int ties) {
    set_game_color(COLOR_GAME_SCORE);
    printf("\n===================================\n");
    printf("            CURRENT SCORE\n");
    set_game_color(COLOR_GAME_WIN);
    printf("  You: %d  ", user_wins);
    set_game_color(COLOR_GAME_LOSE);
    printf("Annex Pay: %d  ", annex_pay_wins);
    set_game_color(COLOR_GAME_TIE);
    printf("Ties: %d\n", ties);
    set_game_color(COLOR_GAME_SCORE);
    printf("===================================\n\n");
    set_game_color(COLOR_GAME_DEFAULT);
}

void game_animate_choice() {
    set_game_color(COLOR_GAME_APP_NAME);
    printf("Annex Pay");
    set_game_color(COLOR_GAME_DEFAULT);
    printf(" is choosing...\n\n");

    set_game_color(COLOR_GAME_ANIMATION);

    // Animation Frames
    const char* frames[] = {
        "  _________     \n"
        " |         |    \n"
        " |   ROCK  |    \n"
        " |_________|    \n",

        "  _________     \n"
        " |         |    \n"
        " |  PAPER  |    \n"
        " |_________|    \n",

        "  _________     \n"
        " |         |    \n"
        " | SCISSORS|    \n"
        " |_________|    \n"
    };

    // Loop through the animation frames
    for (int i = 0; i < 6; i++) {
        int frame_index = i % 3;
        printf("%s", frames[frame_index]);
        sleep_ms(300);

        // Move cursor back up to overwrite previous frame
        printf("\033[F\033[F\033[F\033[F");
    }

    // Clear animation area after loop
    printf("\033[K\n\033[K\n\033[K\n\033[K\n");

    set_game_color(COLOR_GAME_DEFAULT);
    printf("\n");
}

char game_get_user_choice() {
    char input;
    printf("Rock, Paper, Scissors\n");
    printf("Enter your choice (r/p/s): ");

    if (scanf(" %c", &input) != 1) {
        while (getchar() != '\n');
        return '\0';
    }

    input = toupper(input);

    if (input != 'R' && input != 'P' && input != 'S') {
        while (getchar() != '\n');
        return '\0';
    }

    while (getchar() != '\n');
    return input;
}

void game_determine_winner(char user_choice, char computer_choice, int *user_wins, int *annex_pay_wins, int *ties) {
    clearScreen();

    set_game_color(COLOR_GAME_DEFAULT);
    printf("--- Final Choices ---\n");
    printf("You chose: %s\n", game_choice_to_string(user_choice));

    set_game_color(COLOR_GAME_APP_NAME);
    printf("Annex Pay");
    set_game_color(COLOR_GAME_DEFAULT);
    printf(" chose: %s\n", game_choice_to_string(computer_choice));

    printf("\n--- Result ---\n");
    if (user_choice == computer_choice) {
        set_game_color(COLOR_GAME_TIE);
        printf("It's a TIE! You both picked the same.\n");
        (*ties)++;
    } else if (
        (user_choice == 'R' && computer_choice == 'S') ||
        (user_choice == 'P' && computer_choice == 'R') ||
        (user_choice == 'S' && computer_choice == 'P')
    ) {
        set_game_color(COLOR_GAME_WIN);
        printf("You WIN! Enjoy your break!\n");
        (*user_wins)++;
    } else {
        set_game_color(COLOR_GAME_LOSE);
        printf("Annex Pay WINS! Better luck next time.\n");
        (*annex_pay_wins)++;
    }

    set_game_color(COLOR_GAME_DEFAULT);
    printf("----------------\n");
}

/* Main game function */
void playRockPaperScissors() {
    char user_choice;
    char computer_choice;
    char play_again_input[10];
    int play_again = 1;
    int user_wins = 0;
    int annex_pay_wins = 0;
    int ties = 0;

    srand(time(NULL));

    clearScreen();
    set_game_color(COLOR_GAME_DEFAULT);
    printf("Welcome to Rock, Paper, Scissors, brought to you by ");
    set_game_color(COLOR_GAME_APP_NAME);
    printf("Annex Pay!\n");
    set_game_color(COLOR_GAME_DEFAULT);
    printf("Take a break and have some fun!\n\n");

    while (play_again == 1) {
        game_display_score(user_wins, annex_pay_wins, ties);

        // Get User Choice
        printf("\n--- New Round ---\n");

        do {
            user_choice = game_get_user_choice();
            if (user_choice == '\0') {
                printf("Invalid choice. Please enter R, P, or S.\n");
                sleep_ms(1000);
            }
        } while (user_choice == '\0');

        // Animation and Computer Choice
        clearScreen();
        game_animate_choice();

        // Generate computer choice
        int comp_rand = rand() % 3;
        computer_choice = (comp_rand == 0) ? 'R' : (comp_rand == 1 ? 'P' : 'S');

        // Determine and Display Winner
        game_determine_winner(user_choice, computer_choice, &user_wins, &annex_pay_wins, &ties);

        // Ask to Play Again
        printf("\nReady for another round? (y/n): ");
        if (scanf("%s", play_again_input) != 1) {
            play_again = 0;
        } else {
            if (strcasecmp(play_again_input, "Y") == 0 || strcasecmp(play_again_input, "Yes") == 0) {
                play_again = 1;
            } else {
                play_again = 0;
            }
        }
        while (getchar() != '\n');

        if (play_again == 1) {
            clearScreen();
        }
    }

    // Final Score Display
    clearScreen();
    printf("\n--- Final Results ---\n");
    game_display_score(user_wins, annex_pay_wins, ties);
    printf("Thanks for playing! Get back to Annex Pay.\n");
    waitForEnter();
}

/* User session */
void userSession(int accNo) {
    while (1) {
        clearScreen();
        printf("%s=== User Menu (Acc %d) ===%s\n", COLOR_GOLD, accNo, COLOR_RESET);
        printf("1) View Profile\n2) Change PIN\n3) Mini Statement\n4) Transaction History\n");
        printf("5) Download Statement\n6) Deposit\n7) Withdraw\n8) Transfer\n");
        printf("9) Bill Payment\n10) Mobile Recharge\n11) Fund Request\n");
        printf("12) Process Requests\n13) Request Loan\n14) Security Questions\n");
        printf("15) Checkbook Request\n16) Messages\n17) Refer a User\n");
        printf("18) Search Transactions\n19) Bored? Play A Game!\n20) Logout\n"); // Updated: Game at 19, Logout at 20
        printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
        fflush(stdout);
        int ch;
        if (scanf("%d", &ch) != 1) {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            clearInputBuffer();
            sleep_ms(1000);
            continue;
        }
        clearInputBuffer();
        if (ch == 1) {
            Account a;
            if (loadAccountByNo(accNo, &a)) {
                printf("%sAccNo: %d | Name: %s | Balance: %.2f | Locked: %d%s\n",
                       COLOR_WHITE, a.accNo, a.name, a.balance, a.locked, COLOR_RESET);
                int points = calculateAnnexPayPoints(accNo);
                printf("AnnexPay Points: %d ", points);
                if (points >= 850) printf("%s(Excellent)%s\n", COLOR_GREEN, COLOR_RESET);
                else if (points >= 750) printf("%s(Good)%s\n", COLOR_YELLOW, COLOR_RESET);
                else printf("%s(Fair)%s\n", COLOR_RED, COLOR_RESET);
            }
            waitForEnter();
        } else if (ch == 2) changePIN(accNo);
        else if (ch == 3) showMiniStatement(accNo, MINI_STATEMENT_COUNT);
        else if (ch == 4) transactionHistory(accNo);
        else if (ch == 5) downloadStatement(accNo);
        else if (ch == 6) depositInteractive(accNo);
        else if (ch == 7) withdrawInteractive(accNo);
        else if (ch == 8) transferInteractive(accNo);
        else if (ch == 9) billPayment(accNo);
        else if (ch == 10) mobileRecharge(accNo);
        else if (ch == 11) fundRequest(accNo);
        else if (ch == 12) processRequests(accNo);
        else if (ch == 13) requestLoan(accNo);
        else if (ch == 14) {
            SecurityQA qa;
            if (loadSecurityQA(accNo, &qa)) {
                printf("%sSecurity question already set up.%s\n", COLOR_YELLOW, COLOR_RESET);
                printf("1) Reset Security Question\n2) Back\n");
                printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
                fflush(stdout);
                int subch;
                if (scanf("%d", &subch) != 1) {
                    clearInputBuffer();
                    continue;
                }
                clearInputBuffer();
                if (subch == 1) {
                    setupSecurityQuestions(accNo);
                }
            } else {
                setupSecurityQuestions(accNo);
            }
        }
        else if (ch == 15) requestCheckbook(accNo);
        else if (ch == 16) {
            printf("1) Send Message to Admin\n2) View Messages\n");
            printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            int subch;
            if (scanf("%d", &subch) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();
            if (subch == 1) sendMessageToAdmin(accNo);
            else if (subch == 2) viewMessages(accNo);
        }
        else if (ch == 17) referAUser(accNo);
        else if (ch == 18) {
            printf("1) Search by Date Range\n2) Search by Amount Range\n");
            printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            int subch;
            if (scanf("%d", &subch) != 1) {
                clearInputBuffer();
                continue;
            }
            clearInputBuffer();
            if (subch == 1) searchTransactionsByDate(accNo);
            else if (subch == 2) searchTransactionsByAmount(accNo);
        }
        else if (ch == 19) playRockPaperScissors(); // New game feature
        else if (ch == 20) break; // Logout moved to 20
        else {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            sleep_ms(1000);
        }
    }
}

/* Main function */
int main() {
    ensureDataDirectory();
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    while (1) {
        clearScreen();
        printf("%s=== Annex Pay Banking System ===%s\n", COLOR_GOLD, COLOR_RESET);
        printf("1) Create Account\n2) Login\n3) Admin Panel\n4) Account Recovery\n");
        printf("5) About Annex Pay\n6) Exit\n");
        printf("%sChoice: %s", COLOR_WHITE, COLOR_RESET);
        fflush(stdout);
        int ch;
        if (scanf("%d", &ch) != 1) {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            clearInputBuffer();
            sleep_ms(1000);
            continue;
        }
        clearInputBuffer();

        if (ch == 1) {
            createAccountInteractive();
        } else if (ch == 2) {
            clearScreen();
            printf("%s=== Login ===%s\n", COLOR_GOLD, COLOR_RESET);
            int accNo;
            printf("%sEnter account number: %s", COLOR_WHITE, COLOR_RESET);
            fflush(stdout);
            if (scanf("%d", &accNo) != 1 || accNo <= 0) {
                printf("%sInvalid account number.%s\n", COLOR_RED, COLOR_RESET);
                clearInputBuffer();
                sleep_ms(1000);
                continue;
            }
            clearInputBuffer();

            Account a;
            if (!loadAccountByNo(accNo, &a)) {
                printf("%sAccount not found.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            if (a.locked) {
                printf("%sAccount is locked. Contact admin.%s\n", COLOR_RED, COLOR_RESET);
                sleep_ms(1000);
                continue;
            }
            if (!authenticateAccount(accNo, &a)) {
                sleep_ms(1000);
                continue;
            }
            if (accNo == 1337) {
                clearScreen();
                printf("%sWelcome, Elite Hacker. Access Granted.%s\n", COLOR_RED, COLOR_RESET);
                printf("   _____\n  /     \\\n | () () |\n  \\  ^  /\n   |||||\n   |||||\n");
                sleep_ms(3000);
            }
            if (accNo == 6969) {
                printf("%sNice.%s\n", COLOR_YELLOW, COLOR_RESET);
                sleep_ms(2000);
            }
            showLoading("Logging in...", 1000);
            printf("%sLogin successful. Welcome %s!%s\n", COLOR_GREEN, a.name, COLOR_RESET);
            sleep_ms(1000);
            userSession(accNo);
        } else if (ch == 3) {
            adminPanel();
        } else if (ch == 4) {
            accountRecovery();
        } else if (ch == 5) {
            aboutAnnexPay();
        } else if (ch == 6) {
            clearScreen();
            printf("%sThank you for using Annex Pay. Goodbye.%s\n", COLOR_GREEN, COLOR_RESET);
            sleep_ms(1000);
            break;
        } else {
            printf("%sInvalid choice.%s\n", COLOR_RED, COLOR_RESET);
            sleep_ms(1000);
        }
    }
    return 0;
}
