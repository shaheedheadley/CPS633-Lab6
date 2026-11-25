#include <stdio.h>
#include <string.h>

// where user data will be stored
#define TABLE_FILE "../user_table.txt"
#define MAX_LINE_LEN 64
#define MAX_WORD_LEN 32

// imported encryption function from the lab document
void E(char *in, char *out)
{
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
}

// function to create table if running for the first time
int create_table() {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "w");

    // table header with dynamic padding
    fprintf(fp, "%-15s %-15s %-32s\n", "USERID", "PASSWORD", "HASH");
    fprintf(fp, "--------------------------------------------------------------------\n");

    fclose(fp);
    return 0;
}

// check if the table (even if empty) exists in the file
int check_file() {
    FILE *fp = fopen(TABLE_FILE, "r");

    // sanity check to verify file exists
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1; // return error code
    }

    int ch = fgetc(fp);

    if (ch == EOF) {
        create_table();
    }

    return 0;
}

// function to add a new user if user does not exist
int add_user(char* userid) {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "a");
 
    // ask for password
    char user_pass[32]; // max 32 char password
    printf("Enter your desired password: ");
    scanf("%31s", user_pass);

    char user_hash[64];
    E(user_pass, user_hash);

    printf("%s\n", user_hash);

    fprintf(fp, "%-15s %-15s %-15s\n", userid, user_pass, user_hash);
}

int auth_user(char* userid) {
    return 0;
}

// function to get a user's hash if the user exists
int find_user(char* userid) {
    // open the file in read mode
    FILE *fp = fopen(TABLE_FILE, "r");
    
    char line_buffer[MAX_LINE_LEN];
    char first_word[MAX_WORD_LEN];

    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
        sscanf(line_buffer, "%100s", first_word);
        if (first_word == userid) {
            auth_user(userid);
            return 1; // user was found and has been authenticated
        }
    }

    add_user(userid);
    return 0; // user was not found and has been added
}

int main() {
    check_file();
    
    char userid[16]; // max 16 character userid

    // Take in user id from user
    printf("Enter your userid: ");
    scanf("%15s", &userid);
    
    // Check if user exists and authenticates them
    find_user(userid);
}
