#include <stdio.h>
#include <string.h>

// where user data will be stored
#define TABLE_FILE "../user_table.txt"
#define MAX_LINE_LEN 512
#define MAX_WORD_LEN 256

// imported encryption function from the lab document
void E(char *in, char *out)
{
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
}

// check if the table (even if empty) exists in the file
int check_table() {
    FILE *fp = fopen(TABLE_FILE, "r");

    // sanity check to verify file exists
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1; // return error code
    }

    printf("%c", fgetc(fp));
}

// function to create table if running for the first time
int create_table() {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "w");

    // sanity check to verify file exists
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1; // return error code
    }

    // table header with dynamic padding
    fprintf(fp, "%0s %15s %15s\n", "USERID", "PASSWORD", "HASH");
    fprintf(fp, "--------------------------------------------------------------------\n");

    fclose(fp);
}

// function to add a new user if user does not exist
int add_user() {
    // open the file in write mode
    FILE *fp = fopen(TABLE_FILE, "w");
    
    // sanity check to verify file exists
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    
}

// function to get a user's hash if the user exists
char find_user(int userid) {
    // open the file in read mode
    FILE *fp = fopen(TABLE_FILE, "r");

    char line_buffer[MAX_LINE_LEN];
    char first_word[MAX_WORD_LEN];

    while(fgets(line_buffer, sizeof(line_buffer), fp)) {
        printf("%s\n", line_buffer);
    }

    return "c";
}

int main() {
    int userid;

    // Take in user id from user
    printf("Enter your userid: ");
    scanf("%d", &userid);
    
    // Check if user exists and stores their password in password
    char user_pass[12] = find_user(userid);

    // if user not found, add the user and ask for password
    printf("Enter your desired password: ");
    scanf("%s\n", user_pass);

    // if user is found, use their password to compare and authenticate the current user
    

    printf("%d\n", userid);
    create_table();
}
