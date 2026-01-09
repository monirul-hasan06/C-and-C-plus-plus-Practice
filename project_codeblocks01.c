#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_ATTEMPTS 3
#define FILE_NAME "accounts.dat"

// Account structure
struct Account {
    int accNo;
    char name[50];
    float balance;
    int pin;
    int wrongAttempts;
    int locked;
};

// Loading animation
void loadingAnimation() {
    printf("Processing");
    for(int i = 0; i < 5; i++) {
        printf(".");
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}

// Save account to file
void saveAccount(struct Account acc) {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp) fp = fopen(FILE_NAME, "wb");
    struct Account temp;
    int found = 0;

    while(fread(&temp, sizeof(temp), 1, fp)) {
        if(temp.accNo == acc.accNo) {
            fseek(fp, -sizeof(temp), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, fp);
            found = 1;
            break;
        }
    }

    if(!found) {
        fseek(fp, 0, SEEK_END);
        fwrite(&acc, sizeof(acc), 1, fp);
    }

    fclose(fp);
}

// Load account by account number
int loadAccount(int accNo, struct Account *acc) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return 0;

    while(fread(acc, sizeof(*acc), 1, fp)) {
        if(acc->accNo == accNo) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// Log transaction to a file
void logTransaction(int accNo, char type[], float amount) {
    char filename[50];
    sprintf(filename, "acc_%d.txt", accNo);

    FILE *fp = fopen(filename, "a");
    if (!fp) return;

    time_t now = time(NULL);
    char *dt = ctime(&now);
    dt[strlen(dt)-1] = '\0'; // remove newline

    fprintf(fp, "[%s] %s: %.2f\n", dt, type, amount);
    fclose(fp);
}

// Create account
void createAccount() {
    struct Account acc;
    printf("\nEnter Account Number: ");
    scanf("%d", &acc.accNo);

    if(loadAccount(acc.accNo, &acc)) {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s", acc.name);
    printf("Enter Initial Deposit: ");
    scanf("%f", &acc.balance);
    printf("Set a 4-digit PIN: ");
    scanf("%d", &acc.pin);
    acc.wrongAttempts = 0;
    acc.locked = 0;

    printf("\nCreating Account");
    loadingAnimation();
    saveAccount(acc);
    printf("Account Created Successfully!\n");
}

// Verify PIN
int verifyPIN(struct Account *acc) {
    if(acc->locked) {
        printf("Account locked due to 3 wrong attempts!\n");
        return 0;
    }

    int pin;
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if(acc->pin == pin) {
        acc->wrongAttempts = 0;
        saveAccount(*acc);
        return 1;
    } else {
        acc->wrongAttempts++;
        printf("Incorrect PIN! Attempt %d/%d\n", acc->wrongAttempts, MAX_ATTEMPTS);
        if(acc->wrongAttempts >= MAX_ATTEMPTS) {
            acc->locked = 1;
            printf("Account locked due to 3 wrong attempts!\n");
        }
        saveAccount(*acc);
        return 0;
    }
}

// Deposit money
void depositMoney() {
    int accNo;
    printf("\nEnter Account Number: ");
    scanf("%d", &accNo);

    struct Account acc;
    if(!loadAccount(accNo, &acc)) { printf("Account not found!\n"); return; }
    if(!verifyPIN(&acc)) return;

    float amount;
    printf("Enter Deposit Amount: ");
    scanf("%f", &amount);
    acc.balance += amount;
    saveAccount(acc);
    logTransaction(acc.accNo, "Deposit", amount);

    printf("Depositing Money");
    loadingAnimation();
    printf("Deposit Successful! New Balance = %.2f\n", acc.balance);
}

// Withdraw money
void withdrawMoney() {
    int accNo;
    printf("\nEnter Account Number: ");
    scanf("%d", &accNo);

    struct Account acc;
    if(!loadAccount(accNo, &acc)) { printf("Account not found!\n"); return; }
    if(!verifyPIN(&acc)) return;

    float amount;
    printf("Enter Withdraw Amount: ");
    scanf("%f", &amount);

    if(amount > acc.balance) {
        printf("Insufficient Balance!\n");
    } else {
        acc.balance -= amount;
        saveAccount(acc);
        logTransaction(acc.accNo, "Withdraw", amount);

        printf("Processing Withdrawal");
        loadingAnimation();
        printf("Withdraw Successful! New Balance = %.2f\n", acc.balance);
    }
}

// Check balance
void checkBalance() {
    int accNo;
    printf("\nEnter Account Number: ");
    scanf("%d", &accNo);

    struct Account acc;
    if(!loadAccount(accNo, &acc)) { printf("Account not found!\n"); return; }
    if(!verifyPIN(&acc)) return;

    printf("Fetching Account Details");
    loadingAnimation();
    printf("\nAccount Number: %d\n", acc.accNo);
    printf("Name: %s\n", acc.name);
    printf("Balance: %.2f\n", acc.balance);
}

// Reset PIN
void resetPIN() {
    int accNo;
    printf("\nEnter Account Number: ");
    scanf("%d", &accNo);

    struct Account acc;
    if(!loadAccount(accNo, &acc)) { printf("Account not found!\n"); return; }

    char verifyName[50];
    printf("Enter your Name to verify: ");
    scanf("%s", verifyName);

    if(strcmp(acc.name, verifyName) == 0) {
        int newPIN;
        printf("Enter new 4-digit PIN: ");
        scanf("%d", &newPIN);
        acc.pin = newPIN;
        acc.wrongAttempts = 0;
        acc.locked = 0;
        saveAccount(acc);
        printf("PIN reset successfully! You can now login.\n");
    } else {
        printf("Verification failed. Cannot reset PIN.\n");
    }
}

// View transaction history
void viewTransactions() {
    int accNo;
    printf("\nEnter Account Number: ");
    scanf("%d", &accNo);

    struct Account acc;
    if(!loadAccount(accNo, &acc)) { printf("Account not found!\n"); return; }
    if(!verifyPIN(&acc)) return;

    char filename[50];
    sprintf(filename, "acc_%d.txt", accNo);
    FILE *fp = fopen(filename, "r");
    if (!fp) { printf("No transactions found.\n"); return; }

    printf("\nTransaction History for Account %d:\n", accNo);
    printf("---------------------------------\n");
    char line[100];
    while(fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    printf("---------------------------------\n");
    fclose(fp);
}

// List all accounts
void listAllAccounts() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if(!fp) { printf("No accounts found.\n"); return; }

    printf("\nAll Accounts:\n");
    printf("-------------------------------\n");
    struct Account acc;
    while(fread(&acc, sizeof(acc), 1, fp)) {
        printf("Acc No: %d | Name: %s | Balance: %.2f\n", acc.accNo, acc.name, acc.balance);
    }
    printf("-------------------------------\n");
    fclose(fp);
}

// Transfer money between accounts
void transferMoney() {
    int srcAccNo, tgtAccNo;
    float amount;

    printf("\nEnter Your Account Number: ");
    scanf("%d", &srcAccNo);

    struct Account srcAcc;
    if(!loadAccount(srcAccNo, &srcAcc)) { printf("Account not found!\n"); return; }
    if(!verifyPIN(&srcAcc)) return;

    printf("Enter Target Account Number: ");
    scanf("%d", &tgtAccNo);

    struct Account tgtAcc;
    if(!loadAccount(tgtAccNo, &tgtAcc)) { printf("Target account not found!\n"); return; }

    printf("Enter Transfer Amount: ");
    scanf("%f", &amount);

    if(amount > srcAcc.balance) {
        printf("Insufficient Balance!\n");
        return;
    }

    srcAcc.balance -= amount;
    tgtAcc.balance += amount;

    saveAccount(srcAcc);
    saveAccount(tgtAcc);

    logTransaction(srcAcc.accNo, "Transfer to Account", amount);
    logTransaction(tgtAcc.accNo, "Transfer from Account", amount);

    printf("Transferring Money");
    loadingAnimation();

    printf("Transfer Successful!\n");
    printf("Your New Balance: %.2f\n", srcAcc.balance);
}

// Main menu
int main() {
    int choice;
    do {
        printf("\n===== Welcome to ANNEX PAY =====\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Check Balance\n");
        printf("5. Reset PIN\n");
        printf("6. View Transaction History\n");
        printf("7. List All Accounts\n");
        printf("8. Transfer Money to other Account\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: createAccount(); break;
            case 2: depositMoney(); break;
            case 3: withdrawMoney(); break;
            case 4: checkBalance(); break;
            case 5: resetPIN(); break;
            case 6: viewTransactions(); break;
            case 7: listAllAccounts(); break;
            case 8: transferMoney(); break;
            case 9: printf("\nThank you for using Online Banking System!\n"); break;
            default: printf("Invalid Choice!\n");
        }
    } while(choice != 9);

    return 0;
}
