/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : schedSim.h
 * @brief          : This is the header file for the schedSim source code. It
 *                    contains the macros, struct definitions and function
 *                    prototypes.
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */
#ifndef SCHSIM_H
#define SCHSIM_H

#include <stdint.h>
#include <stdio.h>

#define TUPLE_SIZE_  2
#define JOBS_BUFFER_ 100
#define FILE_INDEX_  1

typedef enum {QUEUED, RUNNING, FINISHED} job_status;

typedef struct job_info{
    uint16_t   job_id;
    uint16_t   total_run_time;
    uint16_t   arrival_time;
    uint16_t   remaining_run_time;
    uint16_t   last_ran_time;      /* For RR. Hold where Process stopped running*/
    double     turnaround_time;
    double     wait_time;
    uint8_t    priority;           /* Priority for SJN*/
    job_status status;
}job;

typedef job *jobs;


typedef struct tuple{
    uint16_t run_time;
    uint16_t arrival_time;
}tuple;

typedef tuple *tuples;

void readFile(FILE *file_ptr, uint16_t *job_count, tuples *tuples_list);
void scheduleJobs(jobs *jobs_queue, 
                  tuples tuple_list, 
                  uint16_t job_count, 
                  uint8_t algorithm, 
                  unsigned int quantum);
void scheduleFIFO(jobs *jobs_queue, tuples tuple_list, uint16_t job_count);
void scheduleRR(jobs *jobs_queue, tuples tuple_list, uint16_t job_count, unsigned int quantum);
void scheduleSRTN(jobs *jobs_queue, tuples tuple_list, uint16_t job_count);
void addTuplesToJobs(jobs *jobs_queue, tuples tuple_list, uint16_t job_count);
void executeFIFOSim(jobs jobs_queue, uint16_t job_count);
void executeRRSim(jobs jobs_queue, uint16_t job_count, unsigned int quantum);
void executeSRTNSim(jobs jobs_queue, uint16_t job_count);
uint32_t getIdleTime(jobs jobs_queue, uint16_t job_count);
uint16_t getShortestJobID(jobs jobs_queue, uint16_t job_count, uint32_t clock_tick);
void printOut(jobs jobs_queue, uint16_t job_count);
void printJob(job jobs_queue);
void printAverage(jobs jobs_queue, uint16_t job_count);
void calculateAverages(jobs jobs_queue, uint16_t job_count, double *avg_wait, double *avg_turnaround);
void printJobInfo(job print_job);
void errorout(char *error);

#endif