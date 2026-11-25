# CPS633-Lab6

This lab project was created for the Compuer Security course at TMU.

Authors (by last name):
- Shaheed Headley
- Alexandra Julian
- Alex Nielsen
- Daniel Trakas

# Lab Project Structure

This lab contains 4 parts, each implemented independently:
1. OLMHash Password Scheme
2. Challenge-Response (Online) Authentication
3. Password Cracker
4. Real-Life Password Crackers

# Part 1: OLMHash Password Scheme

### Brief Description
LMHash (and NTLMHash v1) were password schemes that was used in older Windows systems; this is an implementation that mimics their functionality.

# Part 2: Challenge-Response (Online) Authentication

### Brief Description
The purpose of challenge-response authentication is to verify users without having to send any secret information across a network. There are two roles:
1. Server: Generates a challenge, which could be a random number or data that is only used once 
2. Client: Uses the challenge and a shared secret (passwords or keys that the server may know) to generate a response

After the challenge and response have been generated, we can verify the response by comparing it with the expected result or signature of the response; authenticating the user.

# Part 3: Password Cracker

### Brief Description
Dictionary attacks can be used for subsets of password hashes, by listing and searching through all of the possible encrypted results of a password in a system. 
When given a hash from our implementation in Part 1, use this dictionary of uppercase 4-character password hashes to return the actual password. 

# Part 4: Real-Life Password Crackers

### Brief Description
There is a tool called ophcrack that can crack real-life passwords; using LMHash to generate hashes, we can use ophcrack (and rainbow tables if needed) to find the original passwords

