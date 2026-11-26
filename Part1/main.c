#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

// Constants
#define TABLE_FILE "user_table.txt"
#define MAX_LINE_LEN 128
#define NUM_ATTEMPTS 3

// Global login attempt counter (resets every time program runs)
int n = 0;

// Provided block encryption function (processes a 4-byte block)
void E(const char *in, char *out) {
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
}

// Validates password composition rules (Alphanumeric only)
bool is_valid_password(const char *pass) {
    for (int i = 0; pass[i] != '\0'; i++) {
        if (!isalnum((unsigned char)pass[i])) {
            return false;
        }
    }
    return true;
}

// Pre-processes password using the steps:
//      1. converts lowercase to uppercase
//      2. pad (or truncate) to 12 chars
//      4. hashes 3 blocks 
//      3. converts to hex
void compute_olm_hash(const char *raw_pass, char *hex_output) {
    char processed[12];
    
    // 1. Convert to uppercase and copy up to 12 characters
    int len = 0;
    while (raw_pass[len] != '\0' && len < 12) {
        processed[len] = toupper((unsigned char)raw_pass[len]);
        len++;
    }
    
    // 2. Pad remaining space up to 12 characters with null bytes
    while (len < 12) {
        processed[len] = '\0';
        len++;
    }
    
    // 3. Process the three 4-character blocks (B0, B1, B2) using E()
    char binary_hash[12];
    E(&processed[0], &binary_hash[0]);
    E(&processed[4], &binary_hash[4]);
    E(&processed[8], &binary_hash[8]);
    
    // 4. Convert binary hash to a 24-character hexadecimal printable string
    for (int i = 0; i < 12; i++) {
        // casts the binary hash bytes into an unsighed char so that the resulting
        // hexadecimal number is a positive hex value and not a negative value
        sprintf(&hex_output[i * 2], "%02X", (unsigned char)binary_hash[i]);
    }
    hex_output[24] = '\0';
}

// function to create table if running for the first time
void create_table() {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "w");

    // table header with dynamic padding
    fprintf(fp, "%-15s %-32s\n", "USERID", "HASH");
    fprintf(fp, "--------------------------------------------------------------------\n");

    fclose(fp);
}

// Checks if the file has a table
void check_file() {
    FILE *fp = fopen(TABLE_FILE, "r");
    if (fp == NULL) {
        FILE *fp = fopen(TABLE_FILE, "w");
    } 

    //first character in the file
    int ch = fgetc(fp);

    // check if the character is the EOF (if file is empty)
    if (ch == EOF) {
        create_table();
    }

    fclose(fp);
}

// Requirement 1: Registers a brand new user
void add_user(const char* user_id) {
    // 64 char user password - can be truncated to 12 so length is arbitrary
    char user_pass[64];
   
    // asks until an alphanumeric password is typed
    while (1) {
        printf("Enter a new password (max 12 alphanumeric chars): ");
        scanf("%63s", user_pass);
        
        if (is_valid_password(user_pass)) {
            break; // exits loop once validated password is found
        }
        printf("Invalid characters! Passwords must only contain letters and numbers.\n");
    }

    // 25 char hexadecimal hash using the hashing steps in the document
    // 12 byte binary hash -> * 2 bytes per ASCII hex value -> 12*2 = 24 byte hex hash
    // +1 for /0 termination byte at the end of the hash
    char hex_hash[25];
    compute_olm_hash(user_pass, hex_hash);

    // append the information to the file
    FILE *fp = fopen(TABLE_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%-15s %-15s\n", user_id, hex_hash);
        fclose(fp);
        printf("Account created successfully!\n");
    }
}

// Requirement 2: Rewrites file updating target user's password hash
void update_password(const char* target_uid) {
    // same reasoning for the first user_pass assignment; will be truncated
    char new_pass[64];
    // made this a validation loop, since the document specified only A-Z + 0-9...?
    while (1) {
        printf("Enter your NEW password (max 12 alphanumeric chars): ");
        scanf("%63s", new_pass);
        if (is_valid_password(new_pass)) {
            break;
        }
        printf("Invalid characters! Letters and numbers only.\n");
    }

    // same hashing steps as performed in the add_user() above 
    char new_hex_hash[25];
    compute_olm_hash(new_pass, new_hex_hash);

    // read the existing file content and substitute hash line
    FILE *fp = fopen(TABLE_FILE, "r");
    FILE *temp = fopen("temp.txt", "w"); // temp file used to replace the new file
    
    char line[MAX_LINE_LEN];
    char current_uid[64], current_hash[64];

    // change the hash of the line that has the specific matching userid
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63s %63s", current_uid, current_hash) == 2) {
            if (strcmp(current_uid, target_uid) == 0) {
                fprintf(temp, "%-15s %-15s\n", current_uid, new_hex_hash); // write updated
            } else {
                fprintf(temp, "%-15s %-15s\n", current_uid, current_hash); // keep old
            }
        }
    }

    fclose(fp);
    fclose(temp);
    
    // perform file swap by renaming the temp file with the updated information
    // to the name of the table file constants we set at the top of the script
    remove(TABLE_FILE);
    rename("temp.txt", TABLE_FILE);
    printf("Password updated successfully!\n");
}

// Requirement 3: Authenticates existing user with brute-force tracking
bool auth_user(const char* user_id, const char* expected_hash) {
    // password input and its resulting hash
    char pass_input[64];
    char computed_hex[25];

    // triple attempt loop for password verification
    while (n < NUM_ATTEMPTS) {
        printf("Enter your old password: ");
        scanf("%63s", pass_input);

        // calculates the hash of that password
        compute_olm_hash(pass_input, computed_hex);

        // authentication succeeds if hashes match
        if (strcmp(expected_hash, computed_hex) == 0) {
            printf("User authenticated successfully!\n");
            return true; 
        }

        n++; // increment attempt count if it fails
        if (n < NUM_ATTEMPTS) { // user failure attempt notification
            printf("Sorry, try again. (%d/%d attempts used)\n", n, NUM_ATTEMPTS);
        }
    }
   

    printf("Too many unsuccessful attempts - your account is locked.\n");
    
    // we decided to make the program auto-exit at this point, because the user
    // cannot interact with almost all of the login features if they run out of 
    // attempts; exit(0) simply just quits the program for them
    exit(0);
}

// Process logins using the given userid and the table file as an account reference
void process_login(const char* user_id) {
    FILE *fp = fopen(TABLE_FILE, "r");
    char line[MAX_LINE_LEN];
    char table_id[64];
    char table_hash[64];
    bool user_found = false; // to distinguish between adding or authenticating a user

    // try to find the userid in the table by checking the first word line by line
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63s %63s", table_id, table_hash) == 2) {
            if (strcmp(table_id, user_id) == 0) {
                user_found = true;
                break;
            }
        }
    }
    fclose(fp);

    // authenticate user via login if they exist in the table, add them if not
    if (user_found) {
        if (auth_user(user_id, table_hash)) {
            update_password(user_id); // changes hash, not just passowrd
        }
    } else {
        add_user(user_id);
    }
}

// runs first
int main() {
    // verifies that the table exists in the file
    check_file();
    char user_id[64];

    // runs until user (or program) quits
    while (1) {
        printf("\n==================================\n"); // ASCII visual separator
        printf("Enter your userid: ");
        if (scanf("%63s", user_id) == EOF) break;

        // validate user id length
        size_t id_len = strlen(user_id);
        if (id_len < 4 || id_len >= 32) {
            printf("Error: User ID must be between 4 and 31 characters long.\n");
            continue;
        }

        process_login(user_id); // implements the three requirements from the doc
    }
    return 0; //typical success return in main() functions
}
