# Part 1: OLMHash Password Scheme

### Build Instructions
1. Create and enter a build directory to keep source files clean:
    `mkdir build && cd build`
2. Configure the project and generate build files:
    `cmake ..`
3. Compile the code into an executable:
    `cmake --build .`
4. Run the executable:
    `./part1`

### Results
Resulting output of program is located in `results.txt` and `user_table.txt`.

### Findings & Observations
1. It is very hard to guess the user's password from their hash in the user_table since it is not a simple cipher technique
2. The lockout system prevents brute force of simple attacks (such as the program in `Part3/`, for example)
3. We chose not to store the passwords and only store the hash in the table for security purposes; as storing the password in the table would make the hashing a useless defense in the scenario where an attacker gains possession of the table (and does not need to figure out how to decrypt it)

