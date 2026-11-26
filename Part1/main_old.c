#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// where user data will be stored
#define TABLE_FILE "../user_table.txt"
#define MAX_LINE_LEN 64
#define MAX_WORD_LEN 32
#define NUM_ATTEMPTS 3

// global scope count variable (resets each run)
int n = 0;

// imported encryption function from the lab document
void E(char *in, char *out)
{
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
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

// check if the table (even if empty) exists in the file
void check_file() {
    FILE *fp = fopen(TABLE_FILE, "r");

    // sanity check to verify file exists
    if (fp == NULL) {
        printf("Error opening file.\n");
    }

    int ch = fgetc(fp);

    if (ch == EOF) {
        create_table();
    }

    fclose(fp);
}

// function to add a new user if user does not exist
void add_user(char* user_id) {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "a");
 
    // ask for password
    char user_pass[16]; // max 16 char password
    printf("Enter your desired password: ");
    scanf("%15s", user_pass);

    char user_hash[32];
    E(user_pass, user_hash);

    fprintf(fp, "%-15s %-15s\n", user_id, user_hash);
    fclose(fp);
}

bool auth_user(char* userid, char* user_hash) {
    // ask for password
    char pass_input[16]; // max 16 char password
    printf("Enter your password: ");
    scanf("%31s", pass_input);

    char hash_output[32];
    E(pass_input, hash_output);


    while(n < NUM_ATTEMPTS-1) {
        if (strcmp(user_hash, hash_output) == 0) {
            printf("user authenticated!\n");
            return 1;        
        }

        n++;
        printf("Sorry, try again.\nEnter your password: ");
        scanf("%31s", pass_input);
    }
    
    printf("Too many unsuccessful attempts - Your account is locked.\n");
    return 0; // 0 = false/failed
}

// function to get a user's hash if the user exists
void find_user(char* user_id) {
    // open the file in read mode
    FILE *fp = fopen(TABLE_FILE, "r");
    
    bool user_found = 0;
    char line_buffer[MAX_LINE_LEN];
    char table_id[MAX_WORD_LEN];
    char user_pass[MAX_WORD_LEN];
    char user_hash[MAX_WORD_LEN];

    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
        sscanf(line_buffer, "%100s %100s", table_id, user_hash);
        if (strcmp(table_id, user_id) == 0) {
            user_found = 1;
            auth_user(user_id, user_hash);
        }
    }

    // if user was not found, add the user
    if (user_found == 0) {
        add_user(user_id);
        fclose(fp);
    }
}

int main() {
    check_file();
    
    char user_id[16]; // max 16 character userid

    while(1) { // until user quits program
        // Take in user id from user
        printf("Enter your userid: ");
        scanf("%15s", &user_id);
        
        // Check if user exists and authenticates them
        find_user(user_id);
    }
}
