
/* ANNEX PAY - Full Featured Mini Banking System in C
   Features:
   - Multiple accounts (accounts.dat)
   - Login (accNo + PIN), 3 wrong attempts -> lock
   - Create account, change PIN, view profile
   - Deposit,  transfer
   - Transaction history per account (acc_<accNo>_tx.txt)
   - Mini-statement (last 5) and download statement
   - Bill payment & mobile recharge
   - Fund requests between users (request file)
   - Loan requests (admin approval) & EMI calculation
   - Admin panel (admin password: admin123)
   - Apply interest to all accounts (admin)
   - Simple loading animation
   - No external libraries beyond C stdlib
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ACC_FILE "accounts.dat"
#define REQ_FILE "fund_requests.dat"
#define LOAN_FILE "loan_requests.dat"
#define ADMIN_PASS "admin123"
#define START_ACC_NO 1001
#define TX_FILENAME_BUF 64
#define DATE_BUF 64
#define LINE_BUF 256
#define MINI_STATEMENT_COUNT 5
#define MAX_NAME 50

/* Account structure */
typedef struct
{
    int accNo;
    char name[MAX_NAME];
    int pin;
    float balance;
    int wrongAttempts;
    int locked; /* 1 if locked */
} Account;

/* Fund request */
typedef struct
{
    int id;
    int fromAcc;
    int toAcc;
    float amount;
    char note[100];
    int status; /* 0 = pending, 1 = approved, 2 = declined */
} FundRequest;

/* Loan request */
typedef struct
{
    int id;
    int accNo;
    float amount;
    int tenureMonths;
    int status; /* 0 pending, 1 approved, 2 declined */
} LoanRequest;

/* Utility: current date/time string */
void nowstr(char *buf, size_t n)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", tm);
}

/* Loading animation (brief) */
void loading(const char *msg)
{
    if (msg)
        printf("%s", msg);
    for (int i = 0; i < 3; ++i)
    {
        fflush(stdout);
        printf(".");
        /* small delay */
        for (volatile long k = 0; k < 20000000L; ++k)
            ;
    }
    printf("\n");
}

/* File helpers for accounts */

/* Get next account number by scanning file */
int nextAccountNumber()
{
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
        return START_ACC_NO;
    Account a;
    int last = START_ACC_NO - 1;
    while (fread(&a, sizeof(a), 1, fp))
        last = a.accNo;
    fclose(fp);
    return last + 1;
}

/* Save new account (append) */
int saveNewAccount(Account *a)
{
    FILE *fp = fopen(ACC_FILE, "ab");
    if (!fp)
        return 0;
    fwrite(a, sizeof(*a), 1, fp);
    fclose(fp);
    return 1;
}

/* Overwrite account record given accNo: read all and overwrite matching record */
int updateAccount(Account *a)
{
    FILE *fp = fopen(ACC_FILE, "rb+");
    if (!fp)
        return 0;
    Account tmp;
    while (fread(&tmp, sizeof(tmp), 1, fp))
    {
        if (tmp.accNo == a->accNo)
        {
            fseek(fp, -((long)sizeof(tmp)), SEEK_CUR);
            fwrite(a, sizeof(*a), 1, fp);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Load account by account number */
int loadAccountByNo(int accNo, Account *out)
{
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
        return 0;
    Account a;
    while (fread(&a, sizeof(a), 1, fp))
    {
        if (a.accNo == accNo)
        {
            *out = a;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Find account by name (first match) */
int loadAccountByName(const char *name, Account *out)
{
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
        return 0;
    Account a;
    while (fread(&a, sizeof(a), 1, fp))
    {
        if (strcmp(a.name, name) == 0)
        {
            *out = a;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* List all accounts (for admin) */
void listAllAccounts()
{
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
    {
        printf("No accounts found.\n");
        return;
    }
    printf("\n--- All Accounts ---\n");
    printf("%-8s %-20s %-12s %-6s\n", "AccNo", "Name", "Balance", "Locked");
    Account a;
    while (fread(&a, sizeof(a), 1, fp))
    {
        printf("%-8d %-20s %-12.2f %-6d\n", a.accNo, a.name, a.balance, a.locked);
    }
    fclose(fp);
}

/* Append transaction record for account */
void logTransaction(int accNo, const char *type, float amount, const char *note)
{
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "acc_%d_tx.txt", accNo);
    FILE *fp = fopen(fname, "a");
    if (!fp)
        return;
    char d[DATE_BUF];
    nowstr(d, sizeof(d));
    fprintf(fp, "[%s] %s: %.2f", d, type, amount);
    if (note && strlen(note) > 0)
        fprintf(fp, " (%s)", note);
    fprintf(fp, "\n");
    fclose(fp);
}

/* Show transaction history */
void showTransactionHistory(int accNo)
{
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "acc_%d_tx.txt", accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        printf("No transactions found for account %d.\n", accNo);
        return;
    }
    printf("\n--- Transaction History for %d ---\n", accNo);
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), fp))
    {
        printf("%s", line);
    }
    fclose(fp);
}

/* Show mini-statement (last N lines) */
void showMiniStatement(int accNo, int n)
{
    char fname[TX_FILENAME_BUF];
    snprintf(fname, sizeof(fname), "acc_%d_tx.txt", accNo);
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        printf("No transactions found.\n");
        return;
    }
    /* read into buffer lines */
    char **lines = malloc(sizeof(char *) * n);
    for (int i = 0; i < n; i++)
        lines[i] = NULL;
    int count = 0;
    char buf[LINE_BUF];
    while (fgets(buf, sizeof(buf), fp))
    {
        if (count < n)
        {
            lines[count] = strdup(buf);
        }
        else
        {
            /* shift */
            free(lines[0]);
            for (int k = 0; k < n - 1; k++)
                lines[k] = lines[k + 1];
            lines[n - 1] = strdup(buf);
        }
        count++;
    }
    fclose(fp);
    printf("\n--- Mini Statement (last %d) ---\n", n);
    int start = (count < n) ? 0 : 0;
    for (int i = 0; i < n; i++)
    {
        if (lines[i])
        {
            printf("%s", lines[i]);
            free(lines[i]);
        }
    }
    free(lines);
}

/* Download statement (copy tx file to statement_<accNo>.txt) */
void downloadStatement(int accNo)
{
    char src[TX_FILENAME_BUF], dst[TX_FILENAME_BUF];
    snprintf(src, sizeof(src), "acc_%d_tx.txt", accNo);
    snprintf(dst, sizeof(dst), "statement_%d.txt", accNo);
    FILE *f1 = fopen(src, "r");
    if (!f1)
    {
        printf("No transactions to download.\n");
        return;
    }
    FILE *f2 = fopen(dst, "w");
    if (!f2)
    {
        fclose(f1);
        printf("Failed to create statement file.\n");
        return;
    }
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f1))
        fputs(line, f2);
    fclose(f1);
    fclose(f2);
    printf("Statement downloaded to %s\n", dst);
}

/* Create new account interactive */
void createAccountInteractive()
{
    Account a;
    a.accNo = nextAccountNumber();
    printf("Creating new account. Assigned Account Number: %d\n", a.accNo);
    printf("Enter full name: ");
    getchar(); /* consume newline */
    fgets(a.name, sizeof(a.name), stdin);
    a.name[strcspn(a.name, "\n")] = '\0';
    printf("Set 4-digit PIN: ");
    scanf("%d", &a.pin);
    a.balance = 0.0f;
    a.wrongAttempts = 0;
    a.locked = 0;
    loading("Creating account");
    if (saveNewAccount(&a))
    {
        printf("Account created successfully. Account Number: %d\n", a.accNo);
    }
    else
    {
        printf("Failed to create account.\n");
    }
}

/* Authentication: returns 1 if success and fills out account */
int authenticateAccount(int accNo, Account *out)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return 0;
    }
    if (a.locked)
    {
        printf("Account is locked due to multiple failed PIN attempts.\n");
        return 0;
    }
    int pinTry;
    printf("Enter PIN: ");
    scanf("%d", &pinTry);
    if (pinTry == a.pin)
    {
        a.wrongAttempts = 0;
        updateAccount(&a);
        *out = a;
        return 1;
    }
    else
    {
        a.wrongAttempts++;
        printf("Incorrect PIN. Attempt %d/3\n", a.wrongAttempts);
        if (a.wrongAttempts >= 3)
        {
            a.locked = 1;
            printf("Account locked due to 3 failed attempts.\n");
        }
        updateAccount(&a);
        return 0;
    }
}

/* Change PIN for logged-in account */
void changePIN(int accNo)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    int oldPin;
    printf("Enter current PIN: ");
    scanf("%d", &oldPin);
    if (oldPin != a.pin)
    {
        printf("Incorrect current PIN.\n");
        return;
    }
    int newPin;
    printf("Enter new 4-digit PIN: ");
    scanf("%d", &newPin);
    a.pin = newPin;
    updateAccount(&a);
    printf("PIN changed successfully.\n");
}

/* Deposit */
void depositInteractive(int accNo)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    float amt;
    printf("Enter deposit amount: ");
    scanf("%f", &amt);
    if (amt <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }
    a.balance += amt;
    updateAccount(&a);
    logTransaction(accNo, "Deposit", amt, "");
    loading("Processing deposit");
    printf("Deposit successful. New balance: %.2f\n", a.balance);
}

/* Withdraw */
void withdrawInteractive(int accNo)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    float amt;
    printf("Enter withdraw amount: ");
    scanf("%f", &amt);
    if (amt <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }
    if (amt > a.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }
    a.balance -= amt;
    updateAccount(&a);
    logTransaction(accNo, "Withdraw", amt, "");
    loading("Processing withdrawal");
    printf("Withdrawal successful. New balance: %.2f\n", a.balance);
}

/* Transfer */
void transferInteractive(int fromAcc)
{
    Account src;
    if (!loadAccountByNo(fromAcc, &src))
    {
        printf("Source account not found.\n");
        return;
    }
    int toAcc;
    printf("Enter receiver account number: ");
    scanf("%d", &toAcc);
    Account tgt;
    if (!loadAccountByNo(toAcc, &tgt))
    {
        printf("Receiver account not found.\n");
        return;
    }
    float amt;
    printf("Enter amount to transfer: ");
    scanf("%f", &amt);
    if (amt <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }
    if (amt > src.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }
    src.balance -= amt;
    tgt.balance += amt;
    updateAccount(&src);
    updateAccount(&tgt);
    char note[100];
    snprintf(note, sizeof(note), "To %d", toAcc);
    logTransaction(fromAcc, "Transfer Out", amt, note);
    snprintf(note, sizeof(note), "From %d", fromAcc);
    logTransaction(toAcc, "Transfer In", amt, note);
    loading("Processing transfer");
    printf("Transfer successful. Your new balance: %.2f\n", src.balance);
}

/* Bill payment and mobile recharge (simple) */
void billPayment(int accNo)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    printf("Bill types: 1) Electricity 2) Water 3) Internet\n");
    int choice;
    printf("Select bill type: ");
    scanf("%d", &choice);
    float amt;
    printf("Enter bill amount: ");
    scanf("%f", &amt);
    if (amt <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }
    if (amt > a.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }
    a.balance -= amt;
    updateAccount(&a);
    char note[100];
    snprintf(note, sizeof(note), "Bill type %d", choice);
    logTransaction(accNo, "Bill Payment", amt, note);
    loading("Processing bill payment");
    printf("Bill paid successfully. New balance: %.2f\n", a.balance);
}

void mobileRecharge(int accNo)
{
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    char mobile[32];
    printf("Enter mobile number: ");
    scanf("%s", mobile);
    float amt;
    printf("Enter recharge amount: ");
    scanf("%f", &amt);
    if (amt <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }
    if (amt > a.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }
    a.balance -= amt;
    updateAccount(&a);
    char note[100];
    snprintf(note, sizeof(note), "Recharge %s", mobile);
    logTransaction(accNo, "Mobile Recharge", amt, note);
    loading("Processing recharge");
    printf("Recharge successful. New balance: %.2f\n", a.balance);
}

/* Fund request: create request which receiver will see and can approve */
int nextFundRequestId()
{
    FILE *fp = fopen(REQ_FILE, "rb");
    if (!fp)
        return 1;
    FundRequest r;
    int last = 0;
    while (fread(&r, sizeof(r), 1, fp))
        last = r.id;
    fclose(fp);
    return last + 1;
}

void createFundRequest(int fromAcc)
{
    FundRequest fr;
    fr.id = nextFundRequestId();
    fr.fromAcc = fromAcc;
    printf("Enter receiver account number: ");
    scanf("%d", &fr.toAcc);
    printf("Enter amount: ");
    scanf("%f", &fr.amount);
    getchar();
    printf("Enter note (optional): ");
    fgets(fr.note, sizeof(fr.note), stdin);
    fr.note[strcspn(fr.note, "\n")] = '\0';
    fr.status = 0;
    FILE *fp = fopen(REQ_FILE, "ab");
    if (!fp)
    {
        printf("Failed to create request.\n");
        return;
    }
    fwrite(&fr, sizeof(fr), 1, fp);
    fclose(fp);
    printf("Fund request created (ID %d). Receiver will see it when logged in.\n", fr.id);
}

/* Receiver checks and approves/declines requests sent to them */
void viewAndProcessRequests(int accNo)
{
    FILE *fp = fopen(REQ_FILE, "rb+");
    if (!fp)
    {
        printf("No fund requests found.\n");
        return;
    }
    FundRequest fr;
    int found = 0;
    while (fread(&fr, sizeof(fr), 1, fp))
    {
        if (fr.toAcc == accNo && fr.status == 0)
        {
            found = 1;
            printf("\nRequest ID %d - From: %d Amount: %.2f Note: %s\n", fr.id, fr.fromAcc, fr.amount, fr.note);
            printf("1) Approve  2) Decline  3) Skip\nChoose: ");
            int ch;
            scanf("%d", &ch);
            long pos = ftell(fp) - sizeof(fr);
            if (ch == 1)
            {
                /* approve: transfer funds */
                Account sender, receiver;
                if (!loadAccountByNo(fr.fromAcc, &sender))
                {
                    printf("Sender account not found. Declining.\n");
                    fr.status = 2;
                    fseek(fp, pos, SEEK_SET);
                    fwrite(&fr, sizeof(fr), 1, fp);
                    continue;
                }
                if (!loadAccountByNo(fr.toAcc, &receiver))
                {
                    printf("Receiver account not found.\n");
                    fr.status = 2;
                    fseek(fp, pos, SEEK_SET);
                    fwrite(&fr, sizeof(fr), 1, fp);
                    continue;
                }
                if (receiver.balance < fr.amount)
                {
                    printf("You do not have sufficient balance to send money (you are receiver). Declining.\n");
                    fr.status = 2;
                    fseek(fp, pos, SEEK_SET);
                    fwrite(&fr, sizeof(fr), 1, fp);
                    continue;
                }
                /* Note: spec ambiguous — common model: request means requester asks receiver to send; here we implement receiver (current logged-in) approves and sends amount to requester */
                receiver.balance -= fr.amount;
                sender.balance += fr.amount;
                updateAccount(&sender);
                updateAccount(&receiver);
                char note1[100];
                snprintf(note1, sizeof(note1), "Fund request ID %d approved by %d", fr.id, fr.toAcc);
                logTransaction(fr.fromAcc, "Fund Received", fr.amount, note1);
                char note2[100];
                snprintf(note2, sizeof(note2), "Fund request ID %d sent to %d", fr.id, fr.fromAcc);
                logTransaction(fr.toAcc, "Fund Sent", fr.amount, note2);
                fr.status = 1;
                fseek(fp, pos, SEEK_SET);
                fwrite(&fr, sizeof(fr), 1, fp);
                printf("Request approved and funds transferred.\n");
            }
            else if (ch == 2)
            {
                fr.status = 2;
                fseek(fp, pos, SEEK_SET);
                fwrite(&fr, sizeof(fr), 1, fp);
                printf("Request declined.\n");
            }
            else
            {
                /* skip (do nothing) */
                fseek(fp, pos, SEEK_SET);
                fwrite(&fr, sizeof(fr), 1, fp); /* keep unchanged */
                printf("Skipped.\n");
            }
        }
    }
    if (!found)
        printf("No pending requests for you.\n");
    fclose(fp);
}

/* Loan requests handling */
int nextLoanRequestId()
{
    FILE *fp = fopen(LOAN_FILE, "rb");
    if (!fp)
        return 1;
    LoanRequest lr;
    int last = 0;
    while (fread(&lr, sizeof(lr), 1, fp))
        last = lr.id;
    fclose(fp);
    return last + 1;
}

void createLoanRequest(int accNo)
{
    LoanRequest lr;
    lr.id = nextLoanRequestId();
    lr.accNo = accNo;
    printf("Enter loan amount requested: ");
    scanf("%f", &lr.amount);
    printf("Enter tenure in months: ");
    scanf("%d", &lr.tenureMonths);
    lr.status = 0;
    FILE *fp = fopen(LOAN_FILE, "ab");
    if (!fp)
    {
        printf("Failed to place loan request.\n");
        return;
    }
    fwrite(&lr, sizeof(lr), 1, fp);
    fclose(fp);
    printf("Loan request placed (ID %d). Awaiting admin approval.\n", lr.id);
}

/* Admin: view and process loan requests */
 /*void adminProcessLoans()
{
    FILE *fp = fopen(LOAN_FILE, "rb+");
    if (!fp)
    {
        printf("No loan requests found.\n");
        return;
    }
    LoanRequest lr;
    int found = 0;
    while (fread(&lr, sizeof(lr), 1, fp))
    {
        if (lr.status == 0)
        {
            found = 1;
            printf("\nLoan ID %d - Acc: %d Amount: %.2f Tenure: %d months\n", lr.id, lr.accNo, lr.amount, lr.tenureMonths);
            printf("1) Approve 2) Decline 3) Skip : ");
            int ch;
            scanf("%d", &ch);
            long pos = ftell(fp) - sizeof(lr);
            if (ch == 1)
            {
                lr.status = 1;
                /* credit loan amount to account */
                Account acc;
                if (loadAccountByNo(lr.accNo, &acc))
                {
                    acc.balance += lr.amount;
                    updateAccount(&acc);
                    char note[100];
                    snprintf(note, sizeof(note), "Loan approved ID %d", lr.id);
                    logTransaction(lr.accNo, "Loan Credit", lr.amount, note);
                    /* compute EMI for info */
                    double r = 0.01; /* using monthly rate 1% for demo; in real use, ask admin */
                    double P = lr.amount;
                    int n = lr.tenureMonths;
                    double emi = (P * r * pow(1 + r, n)) / (pow(1 + r, n) - 1);
                    printf("Loan approved and credited. Estimated EMI: %.2f per month for %d months\n", emi, n);
                }
                else
                {
                    printf("Account not found; cannot credit loan.\n");
                    lr.status = 2;
                }
                fseek(fp, pos, SEEK_SET);
                fwrite(&lr, sizeof(lr), 1, fp);
            }
            else if (ch == 2)
            {
                lr.status = 2;
                fseek(fp, pos, SEEK_SET);
                fwrite(&lr, sizeof(lr), 1, fp);
                printf("Loan declined.\n");
            }
            else
            {
                fseek(fp, pos, SEEK_SET);
                fwrite(&lr, sizeof(lr), 1, fp);
            }
        }
    }
    if (!found)
        printf("No pending loan requests.\n");
    fclose(fp);
}

/* Admin: apply interest rate (%) to all accounts */
void adminApplyInterest()
{
    double rate;
    printf("Enter annual interest rate (percent, e.g., 3.5): ");
    scanf("%lf", &rate);
    /* For demo we'll apply as a single-time percent addition proportional to rate/12 (monthly) */
    FILE *fp = fopen(ACC_FILE, "rb+");
    if (!fp)
    {
        printf("No accounts.\n");
        return;
    }
    Account a;
    while (fread(&a, sizeof(a), 1, fp))
    {
        double add = a.balance * (rate / 100.0); /* simple annual addition */
        a.balance += (float)add;
        long pos = ftell(fp) - sizeof(a);
        fseek(fp, pos, SEEK_SET);
        fwrite(&a, sizeof(a), 1, fp);
        /* log for each */
        char note[100];
        snprintf(note, sizeof(note), "Interest applied %.2f%%", rate);
        logTransaction(a.accNo, "Interest", (float)add, note);
    }
    fclose(fp);
    printf("Interest applied to all accounts.\n");
}

/* Admin: reset PIN or unlock account */
void adminResetLockOrPin()
{
    int accNo;
    printf("Enter account number to modify: ");
    scanf("%d", &accNo);
    Account a;
    if (!loadAccountByNo(accNo, &a))
    {
        printf("Account not found.\n");
        return;
    }
    printf("1) Reset PIN  2) Unlock account  3) View profile\nChoose: ");
    int ch;
    scanf("%d", &ch);
    if (ch == 1)
    {
        int newPin;
        printf("Enter new 4-digit PIN: ");
        scanf("%d", &newPin);
        a.pin = newPin;
        updateAccount(&a);
        printf("PIN reset.\n");
    }
    else if (ch == 2)
    {
        a.locked = 0;
        a.wrongAttempts = 0;
        updateAccount(&a);
        printf("Account unlocked.\n");
    }
    else if (ch == 3)
    {
        printf("AccNo: %d Name: %s Balance: %.2f Locked: %d\n", a.accNo, a.name, a.balance, a.locked);
    }
    else
        printf("Invalid choice.\n");
}

/* Admin: delete account */
void adminDeleteAccount()
{///
    int accNo;
    printf("Enter account number to delete: ");
    scanf("%d", &accNo);
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp)
    {
        printf("No accounts file.\n");
        return;
    }
    FILE *tmp = fopen("tmp_acc.dat", "wb");
    Account a;
    int found = 0;
    while (fread(&a, sizeof(a), 1, fp))
    {
        if (a.accNo == accNo)
        {
            found = 1;
            continue;
        }
        fwrite(&a, sizeof(a), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);
    remove(ACC_FILE);
    rename("tmp_acc.dat", ACC_FILE);
    if (found)
        printf("Account %d deleted.\n", accNo);
    else
        printf("Account not found.\n");
}

/* Admin: view pending fund requests */
void adminViewFundRequests()
{
    FILE *fp = fopen(REQ_FILE, "rb");
    if (!fp)
    {
        printf("No fund requests.\n");
        return;
    }
    FundRequest fr;
    printf("\n--- Fund Requests ---\n");
    while (fread(&fr, sizeof(fr), 1, fp))
    {
        printf("ID %d From %d To %d Amount %.2f Status %d Note: %s\n", fr.id, fr.fromAcc, fr.toAcc, fr.amount, fr.status, fr.note);
    }
    fclose(fp);
}

/* Admin menu */
void adminMenu()
{
    char pass[64];
    printf("Enter admin password: ");
    scanf("%s", pass);
    if (strcmp(pass, ADMIN_PASS) != 0)
    {
        printf("Incorrect admin password.\n");
        return;
    }
    while (1)
    {
        printf("\n--- ADMIN PANEL ---\n");
        printf("1) List all accounts\n2) Process loan requests\n3) View fund requests\n4) Apply interest to all accounts\n5) Reset PIN / Unlock Account / View\n6) Delete account\n7) Exit admin\nChoose: ");
        int ch;
        scanf("%d", &ch);
        if (ch == 1)
            listAllAccounts();
        else if (ch == 2)
            adminProcessLoans();
        else if (ch == 3)
            adminViewFundRequests();
        else if (ch == 4)
            adminApplyInterest();
        else if (ch == 5)
            adminResetLockOrPin();
        else if (ch == 6)
            adminDeleteAccount();
        else if (ch == 7)
            break;
        else
            printf("Invalid choice.\n");
    }
}

/* User session after login */
void userSession(int accNo)
{
    while (1)
    {
        printf("\n--- User Menu (Acc %d) ---\n", accNo);
        printf("1) View Profile\n2) Change PIN\n3) Mini Statement\n4) View Full Transaction History\n5) Download Statement\n6) Deposit\n7) Withdraw\n8) Transfer\n9) Bill Payment\n10) Mobile Recharge\n11) Create Fund Request\n12) View/Process Requests (if receiver)\n13) Request Loan\n14) Logout\nChoose: ");
        int ch;
        scanf("%d", &ch);
        if (ch == 1)
        {
            Account a;
            if (loadAccountByNo(accNo, &a))
                printf("AccNo: %d Name: %s Balance: %.2f Locked: %d\n", a.accNo, a.name, a.balance, a.locked);
            else
                printf("Error loading profile.\n");
        }
        else if (ch == 2)
            changePIN(accNo);
        else if (ch == 3)
            showMiniStatement(accNo, MINI_STATEMENT_COUNT);
        else if (ch == 4)
            showTransactionHistory(accNo);
        else if (ch == 5)
            downloadStatement(accNo);
        else if (ch == 6)
            depositInteractive(accNo);
        else if (ch == 7)
            withdrawInteractive(accNo);
        else if (ch == 8)
            transferInteractive(accNo);
        else if (ch == 9)
            billPayment(accNo);
        else if (ch == 10)
            mobileRecharge(accNo);
        else if (ch == 11)
            createFundRequest(accNo);
        else if (ch == 12)
            viewAndProcessRequests(accNo);
        else if (ch == 13)
            createLoanRequest(accNo);
        else if (ch == 14)
            break;
        else
            printf("Invalid choice.\n");
    }
}

/* Top-level menu */
int main()
{
    while (1)
    {
        printf("\n=== Welcome to ANNEX PAY ===\n");
        printf("1) Create Account\n2) Login\n3) Admin Panel\n4) Exit\nChoose: ");
        int ch;
        scanf("%d", &ch);
        if (ch == 1)
        {
            createAccountInteractive();
        }
        else if (ch == 2)
        {
            int accNo;
            printf("Enter account number: ");
            scanf("%d", &accNo);
            Account a;
            if (!loadAccountByNo(accNo, &a))
            {
                printf("Account not found.\n");
                continue;
            }
            if (a.locked)
            {
                printf("Account is locked. Contact admin to unlock.\n");
                continue;
            }
            if (!authenticateAccount(accNo, &a))
            {
                printf("Authentication failed.\n");
                continue;
            }
            printf("Login successful. Welcome %s!\n", a.name);
            userSession(accNo);
        }
        else if (ch == 3)
        {
            adminMenu();
        }
        else if (ch == 4)
        {
            printf("Thank you for using ANNEX PAY. Goodbye.\n");
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}
