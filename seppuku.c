/*
 * seppuku.c - A shared library to terminate a process after a specified time
 *
 * Usage: LD_PRELOAD=./seppuku.so TIME=<seconds> <command>
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <dlfcn.h>

static pthread_t timer_thread;
static int seppuku_seconds = 0;

// Timer thread function
void* timer_function(void* arg) {
    // Sleep for the specified number of seconds
    sleep(seppuku_seconds);
    
    printf("\n[seppuku.so] Time limit of %d seconds reached. Terminating process.\n", 
           seppuku_seconds);
    
    // Send SIGTERM to the current process
    kill(getpid(), SIGTERM);
    
    return NULL;
}

// Constructor function that will be executed when the library is loaded
__attribute__((constructor))
static void setup_seppuku(void) {
    char* seppuku_str = getenv("seppuku_SECONDS");
    
    if (seppuku_str != NULL) {
        seppuku_seconds = atoi(seppuku_str);
        
        if (seppuku_seconds > 0) {
            printf("[seppuku.so] Setting seppuku to %d seconds\n", seppuku_seconds);
            
            // Create a thread to handle the timer
            if (pthread_create(&timer_thread, NULL, timer_function, NULL) != 0) {
                perror("[seppuku.so] Failed to create timer thread");
                return;
            }
            
            // Detach the thread so its resources are freed automatically
            pthread_detach(timer_thread);
        } else {
            printf("[seppuku.so] Invalid seppuku_SECONDS value: %s\n", seppuku_str);
        }
    } else {
        printf("[seppuku.so] seppuku_SECONDS environment variable not set. No seppuku will be applied.\n");
    }
}

// Destructor function that will be executed when the library is unloaded
__attribute__((destructor))
static void cleanup_seppuku(void) {
    // Cancel the timer thread if it's still running
    if (seppuku_seconds > 0) {
        pthread_cancel(timer_thread);
    }
}
