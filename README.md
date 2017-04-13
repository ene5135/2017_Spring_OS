# Project 02

# Rotation Lock

# Introduction
 This assignment includes five system calls in `kernel/rotation.c`. `set_rotation()` set current rotation of the artik device and maintain the rotation data by a global variable `global_rotation`. `rotlock_read()` and `rotlock_write()` decides the caller to sleep or not, according to rotation status, list of acquired processes and waiting processes. And also the system call make the caller process to sleep if it's required. `rotunlock_read()` and `rotunlock_write()` removes the lock acquired information from the list.
 
 # Implementation
 ## 1. Device's rotation data
 
 ## 2. Process lock information
 
 ## 3. Lock acquiring policy
 
 ## 4. Make a process sleep
 
 ## 5. Waking up the process
 
 ## 6. Error handling
 
 # Lessons Learned
 
 # Thank you.
