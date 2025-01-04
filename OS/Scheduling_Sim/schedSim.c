/**
 ******************************************************************************
 * @author		   : Micah Briney
 * @date		   : 5-12-2023
 * @version        : 1
 * @file           : schedSim.c
 * @brief          : This code is a C program that simulates a scheduling 
 *                    algorithm for a set of jobs. The code takes in command 
 *                    line arguments specifying the input file name, scheduling
 *                    algorithm, and quantum (if applicable). It then reads in 
 *                    job information from the input file, adds them to a job 
 *                    queue, and schedules them using the chosen algorithm. The
 *                    program outputs the results of the simulation including 
 *                    job turnaround time and waiting time.
 *                   The program consists of the main function, which calls 
 *                    several helper functions including parseOptions, 
 *                    safefOpen, readFile, scheduleJobs, printOut, and several
 *                    others. The helper functions are responsible for reading 
 *                    in input, scheduling jobs, calculating job statistics, 
 *                    and handling errors. The three scheduling algorithms 
 *                    implemented in this program are FIFO, RR, and SRTN 
 *                    (shortest remaining time next).
 *
 * OS	    : Unix 
 * compiler	: gcc
 ******************************************************************************
 */

#include "options.h"
#include "schedSim.h"
#include "safeutil.h"

/**
 * @fn     : main
 * @brief  : The application entry point.
 * @retval : int
 * @parms  : int argc, char *argv[]
 */
int main(int argc, char *argv[]){
    uint8_t algorithm; 
    unsigned int quantum;
    char *text_file;
    jobs jobs_queue;
    FILE *file_ptr;
    uint16_t job_count;
    tuples tuple_list;

    text_file = argv[FILE_INDEX_];
    
    parseOptions(argc, argv, &algorithm, &quantum);

    file_ptr = safefOpen(text_file);

    readFile(file_ptr, &job_count, &tuple_list);

    scheduleJobs(&jobs_queue, tuple_list, job_count, algorithm, quantum);

    printOut(jobs_queue, job_count);

    free(tuple_list);
    free(jobs_queue);

    return 0;
}

/**
 * @fn	   : readFile
 * @brief  : This function will read the user file and and populate a list of 
 *            tuples.
 * @retval : void
 * @parms  : FILE *file_ptr, uint16_t *job_count, tuples *tuple_list
 */
void readFile(FILE *file_ptr, uint16_t *job_count, tuples *tuple_list){
    uint16_t run_time, arrival_time;
    uint16_t running_count = 0;
    size_t buffer_size = JOBS_BUFFER_;
    tuples all_tuples = NULL;

    all_tuples = (tuple *)safeMalloc(sizeof(tuple) * buffer_size); /* Make space*/

    errno = 0;
    while(!feof(file_ptr)){ /* While not at EOF*/
        fscanf(file_ptr, "%hu %hu", &run_time, &arrival_time); /* Scan for tuples*/
        if(verbosity)
            printf("Run time: %hu Arrival time: %hu\n", run_time, arrival_time);
        all_tuples[running_count].run_time = run_time; /* Store tuples info*/
        all_tuples[running_count].arrival_time = arrival_time;
        running_count++; /* Increment running job counter*/
        if(running_count >= buffer_size){ /* Need more job space*/
            all_tuples = (tuple *)safeRealloc(all_tuples, buffer_size * 2, buffer_size, sizeof(tuple));
            buffer_size *= 2; /* Increase buffer size*/
        }
    }

    if(errno) /* Something went wrong*/
        errorout("ReadFile failed.\n");

    if(verbosity)
        printf("Total number of jobs: %hu\n", running_count);
    *job_count = running_count; /* Save job count*/


    *tuple_list = (tuple *)safeMalloc(sizeof(tuple) * running_count);
    memcpy(*tuple_list, all_tuples, sizeof(tuple) * running_count);

    free(all_tuples);
    return;
}

/**
 * @fn	   : scheduleJobs
 * @brief  : This function starts the scheduling process by dertermining
 *            which algorithm to run and scheduling it.
 * @retval : void
 * @parms  : jobs *jobs_queue, tuples tuple_list, uint16_t job_count, uint8_t algorithm, unsigned int quantum
 */
void scheduleJobs(jobs *jobs_queue, 
                  tuples tuple_list, 
                  uint16_t job_count, 
                  uint8_t algorithm, 
                  unsigned int quantum){

    if(algorithm == FIFO)
        scheduleFIFO(jobs_queue, tuple_list, job_count);
    else if(algorithm == RR)
        scheduleRR(jobs_queue, tuple_list, job_count, quantum);
    else if(algorithm == SRTN)
        scheduleSRTN(jobs_queue, tuple_list, job_count);
    return;
}

/**
 * @fn	   : scheduleFIFO
 * @brief  : This function will schedule a the jobs using a FIFO algorithm.
 *            First it will populate the jobs queue using the list of tuples
 *            then it will execute the simluation.
 * @retval : void
 * @parms  : jobs *jobs_queue, tuples tuple_list, uint16_t job_count
 */
void scheduleFIFO(jobs *jobs_queue, tuples tuple_list, uint16_t job_count){
    addTuplesToJobs(jobs_queue, tuple_list, job_count);
    executeFIFOSim(*jobs_queue, job_count);
}

/**
 * @fn	   : scheduleRR
 * @brief  : This function will schedule a the jobs using a Round Robin
 *            algorithm. First it will populate the jobs queue using the list
 *             of tuples then it will execute the simluation.
 * @retval : void
 * @parms  : jobs *jobs_queue, tuples tuple_list, uint16_t job_count, unsigned int quantum
 */
void scheduleRR(jobs *jobs_queue, tuples tuple_list, uint16_t job_count, unsigned int quantum){
    addTuplesToJobs(jobs_queue, tuple_list, job_count);
    executeRRSim(*jobs_queue, job_count, quantum);
}

/**
 * @fn	   : scheduleRR
 * @brief  : This function will schedule a the jobs using a Round Robin
 *            algorithm. First it will populate the jobs queue using the list
 *             of tuples then it will execute the simluation.
 * @retval : void
 * @parms  : jobs *jobs_queue, tuples tuple_list, uint16_t job_count, unsigned int quantum
 */
void scheduleSRTN(jobs *jobs_queue, tuples tuple_list, uint16_t job_count){
    addTuplesToJobs(jobs_queue, tuple_list, job_count);
    executeSRTNSim(*jobs_queue, job_count);
}

/**
 * @fn	   : addTuplesToJobs
 * @brief  : This function will populate a list of jobs using the list of 
 *            tuples. The jobs will have their id set be the order in which
 *            they were read from the input file. The total_run_time, 
 *            arrival_time, remaining_run_time, last_ran_time and priority are
 *            all deteremined by the run and arrival time. The turnaround and 
 *            wait times will both be set to 0.0 and the status will is set
 *            to QUEUED.
 * @retval : void
 * @parms  : jobs *jobs_queue, tuples tuple_list, uint16_t job_count
 */
void addTuplesToJobs(jobs *jobs_queue, tuples tuple_list, uint16_t job_count){
    int i;
    jobs queue_jobs;
    queue_jobs = (job *)safeMalloc(sizeof(job) * job_count);

    for(i = 0; i < job_count; i++){
        queue_jobs[i].job_id             = i;        
        queue_jobs[i].total_run_time     = tuple_list[i].run_time;
        queue_jobs[i].arrival_time       = tuple_list[i].arrival_time;
        queue_jobs[i].remaining_run_time = tuple_list[i].run_time;
        queue_jobs[i].last_ran_time      = tuple_list[i].arrival_time;  /* Last run time starts at arr time*/
        queue_jobs[i].turnaround_time    = 0.0;
        queue_jobs[i].wait_time          = 0.0;
        queue_jobs[i].priority           = tuple_list[i].arrival_time;
        queue_jobs[i].status             = QUEUED;
    }

    *jobs_queue = (job *)safeMalloc(sizeof(job) * job_count);
    memcpy(*jobs_queue, queue_jobs, sizeof(job) * job_count);
    free(queue_jobs);

    if(verbosity){ /* Print jobs queue*/
        printf("Jobs added to queue jobs then memcopied to jobs queue:\n");
        for(i = 0; i < job_count; i++)
            printJobInfo((*jobs_queue)[i]);
    }

    return;
}

/**
 * @fn	   : executeFIFOSim
 * @brief  : This function runs the FIFO logic simulator. The clock ticks are
 *            the unit of time measurements. The FIFO first checks for idle
 *            time. It will then run the logic until all job status are marked
 *            as finished. The curr_priority is based on the arrival time so 
 *            all jobs with the same priority will run in order of job_id.
 *            Job_id is determined by the tuples order in the input file. 
 *            The simulator effectivly runs by lowest job_id first then by
 *            priority(arrival time). The job runs to completion and wait time
 *            and turnaround time is calculated. The clock_time is updated to
 *            reflect the time it takes to run the job (total_run_time). The 
 *            job is then set to Finished and the next job will begin based on
 *            job_id and priority.
 *            
 * @retval : void
 * @parms  : jobs jobs_queue, uint16_t job_count
 */
void executeFIFOSim(jobs jobs_queue, uint16_t job_count){
    uint32_t clock_tick = 0;    /* Time measurement*/
    uint16_t jobs_complete = 0; /* Increments as jobs are FINISHED*/
    int curr_job = 0;           /* jobs queue index*/
    int curr_priority = 0;      /* Zero is highest priority. Bases on arrival time*/

    clock_tick = getIdleTime(jobs_queue, job_count); /* Get idle time*/

    while(jobs_complete < job_count){
        for(curr_job = 0; curr_job < job_count; curr_job++){ /* Find jobs */
            if(jobs_queue[curr_job].priority == curr_priority && jobs_queue[curr_job].status == QUEUED){
                jobs_queue[curr_job].status = RUNNING;  /* Context switch*/
                jobs_queue[curr_job].wait_time = clock_tick - jobs_queue[curr_job].arrival_time; /* Calc wait time*/
                clock_tick += jobs_queue[curr_job].total_run_time; /* FIFO runs entire burst time.*/
                jobs_queue[curr_job].remaining_run_time = 0; /* Job done*/
                jobs_queue[curr_job].last_ran_time = clock_tick; /* Note when process finished*/
                jobs_queue[curr_job].turnaround_time = clock_tick - jobs_queue[curr_job].arrival_time; /* Calc turnaround*/
                jobs_queue[curr_job].status = FINISHED; /* "Pop" job off queue*/
                jobs_complete++;
            }
        }
        curr_priority++; /* Look for next priority job to run. based on arrival time*/
    }
}

/**
 * @fn	   : executeRRSim
 * @brief  : This function runs the RR (round robin) logic simulator. The clock
 *            ticks are the unit of time measurements. The RR first checks for 
 *            an idle time. It will then run the logic for an amount of time 
 *            (quantum). The priority is used as a way to establish position in
 *            the queue as it is based off of the arrival time. As jobs are run 
 *            for a quantum then the job turn increments. The job turn is a way
 *            to keep track of the changing index size of the circular queue. 
 *            If a job is finished then it is effectivly removed from the queue
 *            by setting its status to FINISHED and incrementing the jobs 
 *            complete varaible and decrementing the circ_queue_size. When the 
 *            number of turns taken is equal to the circ_queue_size then the 
 *            priority(queue position) is reset to 0. This process of resetting
 *            priority to 0 is akin to perfoming a circular queue. If the Job
 *            is running then there are two cases. One, it will complete within
 *            the given quantum, two, it will not. The job fields are updated
 *            accordingly.
 *            
 * @retval : void
 * @parms  : jobs jobs_queue, uint16_t job_count, unsigned int quantum
 */
void executeRRSim(jobs jobs_queue, uint16_t job_count, unsigned int quantum){
    uint32_t clock_tick;             /* Hold time units*/
    uint16_t jobs_complete = 0;      /* How many jobs have been completed*/
    int circ_queue_size = job_count;
    int curr_job;                    /* Jobs_queue index*/
    int curr_priority = 0;           /* Zero is highest priority. Based on arrival time*/
    int job_turns = 0;               /* Holds how many jobs have had a turn*/
   
    clock_tick = getIdleTime(jobs_queue, job_count); /* Get idle time*/

    while(jobs_complete < job_count){
        for(curr_job = 0; curr_job < job_count; curr_job++){ /* Find jobs */
            if(jobs_queue[curr_job].priority == curr_priority && jobs_queue[curr_job].status == QUEUED){
                jobs_queue[curr_job].status = RUNNING;  /* Context switch*/
                if(jobs_queue[curr_job].remaining_run_time <= quantum){ /* Job is finishing up*/
                    jobs_queue[curr_job].wait_time += (clock_tick - jobs_queue[curr_job].last_ran_time);
                    clock_tick += jobs_queue[curr_job].remaining_run_time; /* Increment clock by remaining run time*/
                    jobs_queue[curr_job].remaining_run_time = 0;
                    jobs_queue[curr_job].last_ran_time = clock_tick;
                    jobs_queue[curr_job].turnaround_time = clock_tick - jobs_queue[curr_job].arrival_time;
                    jobs_complete++;
                    jobs_queue[curr_job].status = FINISHED; /* "Pop" job off queue*/
                    job_turns++;
                }
                else{ /* Run job for quantum then put it back into the queue*/
                    jobs_queue[curr_job].wait_time += (clock_tick - jobs_queue[curr_job].last_ran_time);
                    clock_tick += quantum;
                    jobs_queue[curr_job].remaining_run_time -= quantum; /* decrement remaining run time*/
                    jobs_queue[curr_job].last_ran_time = clock_tick;
                    jobs_queue[curr_job].status = QUEUED; /* Put job back in queue*/
                    job_turns++;
                }
                
                /* TODO Needs testing*/ 
                /* Possible refactor choice. Still needs testing after I get above working*/
                /* jobs_queue[curr_job].wait_time += (clock_tick - jobs_queue[curr_job].last_ran_time);
                clock_tick += (jobs_queue[curr_job].remaining_run_time <= quantum ?
                               jobs_queue[curr_job].remaining_run_time : quantum);
                jobs_queue[curr_job].remaining_run_time -= (jobs_queue[curr_job].remaining_run_time <= quantum ? 
                                                           0 : quantum);
                jobs_queue[curr_job].last_ran_time = clock_tick;
                if(jobs_queue[curr_job].remaining_run_time <= quantum){
                    jobs_queue[curr_job].turnaround_time = clock_tick - jobs_queue[curr_job].arrival_time;
                    jobs_complete++;
                    jobs_queue[curr_job].status = FINISHED;
                    job_turns++;
                }
                else{
                    job_turns++;
                    jobs_queue[curr_job].status = QUEUED; 
                } */
                
            }
        }

        if(job_turns == circ_queue_size){
            curr_priority = 0; /* All jobs in queue were processed. Start from beginning of queue*/
            job_turns = 0;     /* Reset job_turn counter*/
            circ_queue_size -= jobs_complete; /* Adjust the circular queue when jobs are finished*/
        }
        else
            curr_priority++; /* Look for next priority job to run. based on arrival time*/
    }
}

/**
 * @fn	   : executeSRTNSim
 * @brief  : This function runs the SRTN (shortest remaining job next) logic 
 *            simulator. The clock ticks are the unit of time measurements. The
 *            SRTN first checks for an idle time. It will then run the logic 
 *            one tick at a time. With each clock_tick the next shortest job is
 *            aquired. This may be the same job that is running. If this is the
 *            case then the curr_job_run_time in incremented in place of the 
 *            clock_tick until the current job is either dequeued or finishes.
 *            When either of these happen the job's fields are updated to reflect
 *            the change in remaining run time and wait time so far; turnaround
 *            if the job is finished. When the context switch occurs the
 *            clock_tick is updated with curr_job_run_time. 
 *            
 * @retval : void
 * @parms  : jobs jobs_queue, uint16_t job_count
 */
void executeSRTNSim(jobs jobs_queue, uint16_t job_count){
    uint32_t clock_tick;             /* Hold time units*/
    uint16_t jobs_complete = 0;      /* How many jobs have been completed*/
    uint16_t running_job_id;         /* Hold running job id*/
    uint16_t next_job_id;            /* Hold id for next shortest remaining job*/
    int curr_job_run_time;           /* How many clock ticks does the curr job run for*/
    double turnaround_time;

    clock_tick = getIdleTime(jobs_queue, job_count); /* Get idle time*/
    
    /* Initial shortest remaining job*/
    running_job_id = getShortestJobID(jobs_queue, job_count, clock_tick); /* Get shortest remaining job*/
    curr_job_run_time = 0;
    while(jobs_complete < job_count){
        jobs_queue[running_job_id].status = RUNNING;
        jobs_queue[running_job_id].remaining_run_time--; /* Decrement reamaining run time*/
        curr_job_run_time++; /* Increament job run time*/
        next_job_id = getShortestJobID(jobs_queue, job_count, (clock_tick + curr_job_run_time));

        /* Check if job will run again*/
        if(next_job_id != running_job_id){ /* If the next job isn't same job*/
            jobs_queue[running_job_id].wait_time += (clock_tick - jobs_queue[running_job_id].last_ran_time);
            clock_tick += curr_job_run_time;
            jobs_queue[running_job_id].last_ran_time = clock_tick; /* set last ran time*/
            jobs_queue[running_job_id].status = QUEUED;
            curr_job_run_time = 0; /* Reset current jobs run time*/
        }

        if(jobs_queue[running_job_id].remaining_run_time == 0){  /* Job is finished*/

            turnaround_time = clock_tick - jobs_queue[running_job_id].arrival_time;
            jobs_queue[running_job_id].turnaround_time = turnaround_time;
            jobs_complete++;
            jobs_queue[running_job_id].status = FINISHED;
            curr_job_run_time = 0; /* Reset current jobs run time*/
        }

        running_job_id = next_job_id; /* Set next shortest remaining job id*/
    }
}

/**
 * @fn	   : getIdleTime
 * @brief  : This function checks for idle time. 
 *            
 * @retval : uint32_t
 * @parms  : jobs jobs_queue, uint16_t job_count
 */
uint32_t getIdleTime(jobs jobs_queue, uint16_t job_count){
    uint32_t idle_ticks;   /* Hold idel time units*/
    int curr_job = 0;      /* Job index*/

    idle_ticks = jobs_queue[0].arrival_time; /* Assume first job arrived first*/
    for(curr_job = 1; curr_job < job_count; curr_job++){     /* Look for idel time*/
        if(idle_ticks > jobs_queue[curr_job].arrival_time)   /* If idle_ticks is larger*/
            idle_ticks = jobs_queue[curr_job].arrival_time; /* Start clock at smaller arrival time*/
    }
    return idle_ticks;
}

/**
 * @fn	   : getShortestJobID
 * @brief  : This function looks for the id of shortest remaining job. It 
 *            always check from the begginning of the queue for shortest 
 *            remaining job.
 *            
 * @retval : uint16_t
 * @parms  : jobs jobs_queue, uint16_t job_count, uint32_t clock_tick
 */

uint16_t getShortestJobID(jobs jobs_queue, uint16_t job_count, uint32_t clock_tick){
    uint16_t shortest_run_time;
    int job_index;
    job next_job;

    shortest_run_time = UINT16_MAX;
    for(job_index = 0; job_index < job_count; job_index++){ /* Look in jobs queue for shortest job next*/
        if(jobs_queue[job_index].remaining_run_time > 0){   /* Must not be a finished job*/
            if(jobs_queue[job_index].arrival_time <= clock_tick){ /* Job must have arrived*/
                if(jobs_queue[job_index].remaining_run_time < shortest_run_time){ /* Check remaining run time*/
                    shortest_run_time = jobs_queue[job_index].remaining_run_time; /* Update shortest reamaining*/
                    next_job = jobs_queue[job_index]; /* Set current shortest remaining job*/
                }
            }
        }
    }

    if(verbosity){
        int i;
        printf("\nJob info for shortest Job:\n");
        printJobInfo(next_job); /* Print shortest job next job*/
        printf("\n-------- VS --------\n");
        for(i = 0; i < job_count; i++) /* Print all jobs*/
            printJobInfo(jobs_queue[i]);
    }

    if(shortest_run_time == UINT16_MAX) /* No queued jobs found*/
        return job_count + 1; /* This job id doesn't exist so it will trigger a job finish*/
    else
        return next_job.job_id;
}

/**
 * @fn	   : printOut
 * @brief  : This function handles the programs output.
 *            
 * @retval : void
 * @parms  : jobs jobs_queue, uint16_t job_count
 */
void printOut(jobs jobs_queue, uint16_t job_count){
    int i;
    
    for(i = 0; i < job_count; i++){
        printJob(jobs_queue[i]);
    }

    printAverage(jobs_queue, job_count);
    return;
}

/**
 * @fn	   : printJob
 * @brief  : This function prints the turnaround and wait time of a single job.
 *            
 * @retval : void
 * @parms  : job jobs_queue
 */
void printJob(job jobs_queue){
    printf("Job %3hu -- Turnaround %3.2f  Wait %3.2f\n", 
           jobs_queue.job_id, jobs_queue.turnaround_time, jobs_queue.wait_time);
    return;
}

/**
 * @fn	   : printAverage
 * @brief  : This function prints the average turnaround and wait time for the 
 *            queued jobs.
 *            
 * @retval : void
 * @parms  : job jobs_queue
 */
void printAverage(jobs jobs_queue, uint16_t job_count){
    double avg_wait;
    double avg_turnaround;
 
    calculateAverages(jobs_queue, job_count, &avg_wait, &avg_turnaround);
    printf("Average -- Turnaround %3.2f  Wait %3.2f\n", avg_turnaround, avg_wait);
    return;
}

/**
 * @fn	   : calculateAverages
 * @brief  : This function calculates the average turnaround and wait time for
 *            the queued jobs.
 *            
 * @retval : void
 * @parms  : jobs jobs_queue, uint16_t job_count, double *avg_wait, double *avg_turnaround
 */
void calculateAverages(jobs jobs_queue, uint16_t job_count, double *avg_wait, double *avg_turnaround){
    double wait_sum = 0.0;
    double turnaround_sum = 0.0;
    int i;

    for(i = 0; i < job_count; i++){
        wait_sum += jobs_queue[i].wait_time;
        turnaround_sum += jobs_queue[i].turnaround_time;
    }
    
    *avg_wait = (wait_sum / job_count);
    *avg_turnaround = (turnaround_sum / job_count);

    return;
}

/**
 * @fn	   : printJobInfo
 * @brief  : This function prints all of the information of a single job. This
 *            funtion is used for testing and the verbosity flag.
 *            
 * @retval : void
 * @parms  : job print_job
 */
void printJobInfo(job print_job){
    printf("job_id: %hu.\n", print_job.job_id);        
    printf("total_run_time: %hu.\n", print_job.total_run_time);
    printf("arrival_time: %hu.\n", print_job.arrival_time);
    printf("remaining_run_time: %hu.\n", print_job.remaining_run_time);
    printf("last_ran_time: %hu.\n", print_job.last_ran_time);
    printf("turnaround_time: %f.\n", print_job.turnaround_time);
    printf("wait_time: %f.\n", print_job.wait_time);
    printf("priority: %u.\n", print_job.priority);
}

/**
 * @fn	   : errorout
 * @brief  : This function prints error information and exits the program.
 *            
 * @retval : void
 * @parms  : char *error
 */
void errorout(char *error){
    if(errno)
        perror(error);
    else
        fprintf(stderr, "%s", error);
    
    exit(EXIT_FAILURE);
}