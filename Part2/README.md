# Part 2: Challenge-Response (Online) Authentication 

### Build Instructions
1. Create and enter a build directory to keep source files clean:
    `mkdir build && cd build`
2. Configure the project and generate build files:
    `cmake ..`
3. Move `user_table.txt` located in `Part2/` into `build/`
4. Compile the code into an executable:
    `cmake --build .`
5. Run the executable:
    `./part2`

### Results
Resulting output of program is located in `results.txt`.

### Findings & Observations
1. Confidentiality principles are upheld by removing the need for the plaintext password and hashed password to be transmitted to the server. By performing a bitwise XOR on the hashed password, the private data (password) is further obfuscated. 

2. Because the server generates the random number and sends it back to the client/user, only the user and server have access to the generated value. This allows the user and server to transmit data securely by use of encrypting hashed passwords with the number, which acts as a local implementation of a challenge-response authenticator.

3. Building off of point 2: the "challenge" comes in the form of the 32-bit random number sent to the user, and the "response" comes in the form of the XOR of the number and the user's hashed password that is sent to the server. The server validates this response by calculating the XOR of the same random number with the stored password hash for the corresponding userid in `user_table.txt`. When they match, the challenge has been validated and access is granted.

4. For better organization, parts 1 and 2 of this lab can be combined. This is because part 2 relies on multiple functions used in part 1, such as the encryption function, the hash computing function, and the password validity check function. While we completed these parts in a disjoint organization, this can be improved upon by creating a header file with the implementations of all functions in parts 1-2, and importing the header file in each respective lab part.
