#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// Generates a random 32-bit integer
//   - rand() works with srand(time(NULL)) in main for rng
//   - since rand() can be of range 2^15-2^32, force 2^32 through bitwise ops  
uint32_t gen_rnd_32() {
	
	uint32_t rnd = rand() & 0xff;			// initial 8-bit rnd int
	rnd = rnd | (rand() & 0xff) << 8;	// expanded to 16-bit with shift
	rnd = rnd | (rand() & 0xff) << 16;	// expanded to 24-bit with shift
	rnd = rnd | (rand() & 0xff) << 24;	// expanded to 32-bit with shift

	return rnd;
}

// Function that mimics the user sending their userid to the server
//   - "authenticates" user by checking user_table for userid
//   - if auth check success, returns true; otherwise false
bool send_userid_server(char* user_id) {
	FILE *fp = fopen("user_table.txt", "r");
	char line[128];	
	char table_id[64];	

	while(fgets(line, sizeof(line), fp)) {
		sscanf(line, "%63s", table_id);
		if(strcmp(table_id, user_id) == 0) {
			fclose(fp);
			return true;
		}
	}

	fclose(fp);
	return false;
}

// Calculates bit-wise XOR between passed char hash and 32-bit random int
uint32_t xor_func(uint32_t rnd, char* hash) {
    char B1[8]; 
	char B2[8];
	char B3[8]; 
	
	// Grabs B1/B2/B3 (where each block is 32-bits of hex)
	snprintf(B1, sizeof(B1), "%.*s", 8, hash);
    snprintf(B2, sizeof(B2), "%.*s", 8, hash+8);
	snprintf(B3, sizeof(B3), "%.*s", 8, hash+16);

	// Converts each char array block into a 32-bit uint
	uint32_t B1int = (uint32_t)strtoul(B1, NULL, 16);
	uint32_t B2int = (uint32_t)strtoul(B2, NULL, 16);
	uint32_t B3int = (uint32_t)strtoul(B3, NULL, 16);

	// XOR random number against all three blocks
	uint32_t result = rnd ^ B1int;
	result = result ^ B2int;
	result = result ^ B3int;
	
	return result;
}

// Verifies user xor matches with server xor
//   - retrieves password hash of corresponding userid in user_table
//   - computes xor using previous random number and table password hash
//	 - compares result xor with passed user xor and checks equality 
bool verify_hash_server(uint32_t user_xor, uint32_t server_int, char* user_id) {
	FILE *fp = fopen("user_table.txt", "r");
	char line[128];
	char table_id[64];
	char table_hash[64];

	while(fgets(line, sizeof(line), fp)) {
		sscanf(line, "%63s %63s", table_id, table_hash);
		if(strcmp(table_id, user_id) == 0) {
			break;
		}
	}
	
	uint32_t server_xor = xor_func(server_int, table_hash);

	fclose(fp);
	return (user_xor == server_xor);
}

// ENCRYPTION, OLM HASH FUNCTION, VALID PASSWORD (PROVIDED FROM PART1)
//-------------------------------------------------------------------------
void E(const char *in, char *out) {
    out[0]=(in[0]&0x80)^(((in[0]>>1)&0x7F)^((in[0])&0x7F));
    out[1]=((in[1]&0x80)^((in[0]<<7)&0x80))^(((in[1]>>1)&0x7F)^((in[1])&0x7F));
    out[2]=((in[2]&0x80)^((in[1]<<7)&0x80))^(((in[2]>>1)&0x7F)^((in[2])&0x7F));
    out[3]=((in[3]&0x80)^((in[2]<<7)&0x80))^(((in[3]>>1)&0x7F)^((in[3])&0x7F));
}

void compute_olm_hash(const char *raw_pass, char *hex_output) {
    char processed[12];
    
    int len = 0;
    while (raw_pass[len] != '\0' && len < 12) {
        processed[len] = toupper((unsigned char)raw_pass[len]);
        len++;
    }
    
    while (len < 12) {
        processed[len] = '\0';
        len++;
    }
    
    char binary_hash[12];
    E(&processed[0], &binary_hash[0]);
    E(&processed[4], &binary_hash[4]);
    E(&processed[8], &binary_hash[8]);
    
    for (int i = 0; i < 12; i++) {
        sprintf(&hex_output[i * 2], "%02X", (unsigned char)binary_hash[i]);
    }
    hex_output[24] = '\0';
}

bool is_valid_password(char *pass) {
	for (int i=0; pass[i] != '\0'; i++) {
		if(!isalnum((unsigned char)pass[i])) {
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------

// Main function mimics the "client", taking input and producing outputs
//   - "server" is implemented through a series of functions, for simplicity
int main(void) {
		
	// Seeds rand with current time, to get different values
	srand(time(NULL));
	
	char user_id[64];
	char password[64];
	char hex_hash[25];
	uint32_t server_res;
	uint32_t user_xor;

	// Scans userid and validates length (from section 1)
	printf("enter userid: ");
	if (scanf("%63s", user_id) == EOF) return (0);	

	if (strlen(user_id) < 4 || strlen(user_id) > 32) {
		printf("Error: user id must be between 4-31 characters\n");
		return (-1);
	}

	// Sends userid to server, which validates user
	if (!send_userid_server(user_id)) {
		printf("Access Denied: user_id not found\n");
		return (-1);
	}

	// Retrieves 32-bit input from server
	server_res = gen_rnd_32();

	// Scans password and validates based on length/characters 
	printf("enter password: ");
	if (scanf("%63s", password) == EOF) return (0);

	// NOTE: As password length handling was handled in Part 1,
	//       only a simple length/char enforcement is done here.	
	if (strlen(password) < 1 || strlen(password) > 12) {
		printf("Error: password must be between 1-12 characters\n");
		return (-1);
	}
	
	if (!is_valid_password(password)) {
		printf("Error: password must only contain alphanumeric characters\n");
		return (-1);
	}

	// Calculate user hash with hash/encryption functions
	compute_olm_hash(password, hex_hash);

	// Calculates XOR
	user_xor = xor_func(server_res, hex_hash);

	// Grants/Denies access based on comparison of user/server XOR:
	if (!verify_hash_server(user_xor, server_res, user_id)) {
		printf("Access Denied.\n");
		return (-1);
	}

	printf("Access Granted.\n");
	
	//printf("number: %d\n", server_int);
	return (0);
}
















