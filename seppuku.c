/*
 * seppuku.c - A shared library to terminate a process after a specified time
 *
 * Usage: LD_PRELOAD=./seppuku.so TIME=<seconds> <command>
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>  //printf
#include <stdlib.h> //atoi and getenv
#include <unistd.h> //sleep and getpid
#include <signal.h> //handle sigterm
#include <pthread.h> //creation of threads
#include <stdbool.h> //bool

static pthread_t timer_thread;
static int DEFAULT_SECONDS = 10;
static bool DEBUG = true;

// Timer thread function
void* timer_function() 
{
    // Sleep for the specified number of seconds
    sleep(DEFAULT_SECONDS);
    
    //check debug before printing
    if (DEBUG) printf("\n[seppuku.so] Time limit of %d seconds reached. Terminating process.\n", 
        DEFAULT_SECONDS);
    
    // Send SIGTERM to the current process
    kill(getpid(), SIGTERM);
    
    return NULL;
}

// Constructor function that will be executed when the library is loaded
__attribute__((constructor))
static void setup_seppuku(void) 
{
    char* seppuku_str = getenv("TIME");
    if (seppuku_str != NULL) 
    {
        DEFAULT_SECONDS = atoi(seppuku_str);
    } else 
    {
        if (DEBUG) printf("[seppuku.so] No time found in environment, reverting to default: %ds.\n", 
            DEFAULT_SECONDS);
    }

    if (DEFAULT_SECONDS > 0) 
    {
        if (DEBUG) printf("[seppuku.so] Setting seppuku to %d seconds\n", 
            DEFAULT_SECONDS);
        
        // Create a thread to handle the timer
        if (pthread_create(&timer_thread, NULL, timer_function, NULL) != 0) 
        {
            if (DEBUG) perror("[seppuku.so] Failed to create timer thread");
            return;
        }
        
        // Detach the thread so its resources are freed automatically
        pthread_detach(timer_thread);
    } else {
        if (DEBUG) printf("[seppuku.so] Invalid seppuku_SECONDS value: %s\n", 
            seppuku_str);
    }
} 

// Destructor function that will be executed when the library is unloaded
__attribute__((destructor))
static void cleanup_seppuku(void) 
{
    // Cancel the timer thread if it's still running
    if (DEFAULT_SECONDS > 0) 
    {
        if(DEBUG) printf("[seppuku.so] Process end before timer\n");
        
        pthread_cancel(timer_thread);
    }
}
