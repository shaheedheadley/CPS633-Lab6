# Part 3: Password Cracker

### Build Instructions
1. Create and enter a build directory to keep source files clean:
    `mkdir build && cd build`
2. Configure the project and generate build files:
    `cmake ..`
3. Compile the code into an executable:
    `cmake --build .`
4. Run the executable:
    `./part3`

### Results
Resulting output of program is located in `results.txt` and `hash_table.txt`.

### Findings & Observations
1. There are 26 uppercase letters to choose from, and 4 letters per password, making hash_tables contain 26^4 = 456,976 passwords in total.
2. Theoretically, the amount of time it would take to search for a password of any amount of letters would be relatively short compared to decrypting it with raw compute, however this attack is still most ideal for shorter and more restricted password schemes so that generating the table does not take a lot of compute (due to quantity of valid passwords and hashes).
3. This attack scales terribly in space complexity, as each new letter exponentially increases the number of passwords needed to be generated, and if the user is allowed to use any Unicode letter, number, or symbol (from any language) in their password, there is a base number of more than 100 thousand combinations of passwords, which scales horribly.
