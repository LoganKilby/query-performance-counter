/* date = May 22nd 2021 9:47 pm */

#ifndef QPC_H
#define QPC_H

#include "windows.h"
#include "stdio.h"

// Basic usage:
//   QPC_StartCounter();
//   long long Result = QPC_EndCounter();
//
//   printf("%lld\n", Result); print microseconds
//   printf("%Lf\n", Result / 1000.0l); print milliseconds

// Simple LIFO: EndCounter returns the time elapsed since the most recent call to StartCounter

#define MAX_COUNTER_CALLS 50
// MAX_COUNTER_CALLS defines how many StartCounter calls can be stored at any given time.
// You may change MAX_COUNTER_CALLS to any value you like.

void QPC_StartCounter(void);
long long QPC_EndCounter(void); // Returns microseconds since last StartCounter
void QPC_EndCounterPrint(char *Message); // Prints Your message followed by the time elapsed in ms

// ------------------------------------------------------------------------------------------------

//
// TIMED_BLOCK
//
// Timing a block of code using c++ constructor/desctuctor
#ifdef __cplusplus
struct High_Resolution_Timer {
    char *function;
    LARGE_INTEGER starting_time;
    LARGE_INTEGER frequency;
};

#endif

struct QPC_Counter
{
    LARGE_INTEGER starting_time;
    LARGE_INTEGER frequency;
};

static QPC_Counter global_qpc_counter_queue[MAX_COUNTER_CALLS] = {};
static int global_qpc_counter_count = 0;

void QPC_AddCounter(QPC_Counter counter) {
    if(global_qpc_counter_count < MAX_COUNTER_CALLS)
    {
        global_qpc_counter_queue[global_qpc_counter_count] = counter;
        global_qpc_counter_count++;
    }
}

void QPC_StartCounter(void)
{
    QPC_Counter new_counter = {};
    QueryPerformanceFrequency(&new_counter.frequency);
    QueryPerformanceCounter(&new_counter.starting_time);
    QPC_AddCounter(new_counter);
}

long long QPC_EndCounter()
{
    long long result = 0;
    
    if(global_qpc_counter_count)
    {
        LARGE_INTEGER ending_time;
        QueryPerformanceCounter(&ending_time);
        
        QPC_Counter counter = global_qpc_counter_queue[global_qpc_counter_count - 1];
        global_qpc_counter_queue[global_qpc_counter_count - 1] = {};
        global_qpc_counter_count--;
        
        LARGE_INTEGER elapsed_microseconds = {};
        elapsed_microseconds.QuadPart = ending_time.QuadPart - counter.starting_time.QuadPart;
        result = (elapsed_microseconds.QuadPart * 1000000) / counter.frequency.QuadPart;
    }
    
    return result;
}

void QPC_EndCounterPrint(char *message)
{
    if(global_qpc_counter_count)
    {
        LARGE_INTEGER ending_time;
        QueryPerformanceCounter(&ending_time);
        
        QPC_Counter counter = global_qpc_counter_queue[global_qpc_counter_count - 1];
        global_qpc_counter_queue[global_qpc_counter_count - 1] = {};
        global_qpc_counter_count--;
        
        LARGE_INTEGER elapsed_microseconds = {};
        elapsed_microseconds.QuadPart = ending_time.QuadPart - counter.starting_time.QuadPart;
        long long result = (elapsed_microseconds.QuadPart * 1000000) / counter.frequency.QuadPart;
        printf("%s %Lf ms\n", message, result / 1000.0l);
    }
    else
    {
        printf("QPC: No counter data to print.\n");
    }
}

#endif //QPC_H
