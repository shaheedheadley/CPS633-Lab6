#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define HASH_TABLE_FILE "../hash_table.txt"
#define MAX_LINE_LEN 128 

// Provided block encryption function (processes a 4-byte block)
void E(const char *in, char *out) {
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
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

// adds hash to the hash table
void add_hash(const char* password) {
    // 25 char hexadecimal hash using the hashing steps in the document
    // 12 byte binary hash -> * 2 bytes per ASCII hex value -> 12*2 = 24 byte hex hash
    // +1 for /0 termination byte at the end of the hash
    char hex_hash[25];
    compute_olm_hash(password, hex_hash);

    // append the information to the file
    FILE *fp = fopen(HASH_TABLE_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%-10s %-15s\n", password, hex_hash);
        fclose(fp);
        // Note: removed this print to reduce terminal clutter (better for debugging purposes)
        // printf("Hash %s for password %s added successfully!\n", hex_hash, password);
    }
}

// Adds all 4 digit uppercase password hashes to the table
void generate_hash_table() {
    // 26 letter uppercase alphabet
    char letters[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    // 4 letter password + 1 for null terminator
    char curr_pass[5];
    curr_pass[4] = '\0';

    // n^4 for loop to generate every 4-letter uppercase password possibility
    for (int first=0; first < 26; first++) {
        for (int second=0; second < 26; second++) {
            for (int third=0; third < 26; third++) {
                for (int fourth=0; fourth < 26; fourth++) {
                    curr_pass[0] = letters[first]; 
                    curr_pass[1] = letters[second];
                    curr_pass[2] = letters[third];
                    curr_pass[3] = letters[fourth];
                    
                    add_hash(curr_pass);
                }
            }
        }
    }

    // Confirm that the hash table is finished
    printf("Hash table generated!\n");
}


// function to create table if running for the first time
void create_table() {
    // open the file in write mode
    FILE *fp = fopen(HASH_TABLE_FILE, "w");

    // table header with dynamic padding
    fprintf(fp, "%-10s %-32s\n", "PASSWORD", "HASH");
    fprintf(fp, "--------------------------------------------------------------------\n");

    fclose(fp);
    
    generate_hash_table();
}

// Checks if the file has a table
void check_file() {
    FILE *fp = fopen(HASH_TABLE_FILE, "r");
    if (fp == NULL) {
        fp = fopen(HASH_TABLE_FILE, "w");
    }

    //first character in the file
    int ch = fgetc(fp);

    // check if the character is the EOF (if file is empty)
    if (ch == EOF) {
        create_table();
    }

    fclose(fp);
}

// find the hash and return the associated password
void find_pass(char* pass_hash, char* pass_out) {
    FILE *fp = fopen(HASH_TABLE_FILE, "r");
    char line[MAX_LINE_LEN];
    char table_pass[64];
    char table_hash[64];

    // try to find the password hash in the table by checking the first word line by line
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63s %63s", table_pass, table_hash) == 2) {
            if (strcmp(table_hash, pass_hash) == 0) {
                strcpy(pass_out, table_pass);
                break;
            }
        }
    }
    fclose(fp);
} 


int main() {
    check_file();

    char cracked_pass[5];
    char input_hash[25];

    printf("Enter the hash of the password you wish to find: ");
    scanf("%63s", input_hash);

    find_pass(input_hash, cracked_pass);

    printf("The password of this hash is: %s\n", cracked_pass);
}
