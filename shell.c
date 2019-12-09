/* This is the only file you should update and submit. */

/* Fill in your Name and GNumber in the following two comment fields
 * Name: Helena Gray
 * GNumber: G01086877
 */

#include "logging.h"
#include "shell.h"
typedef struct job job;
typedef struct jobList jobList;

/**Structs defining a job and a job list. bgIndicator indicates if the job is in the background or not.*/
struct job {
    int jobID;
    int processID;
    char* status;
    char* cmdline;
    job* nextJob;
    int bgIndicator;
};

struct jobList {
    job* head;
    job* tail;
    int numJobs;
    int numJobsRunning;
};

//prototypes
void printJobs(void);
job* getJob(int jobPID);
job* getJobFG(int jobID);
job* getJobBG(int jobID);
void addJobFG(job** newJob);
void addJobBG(job** newJob);
int deleteJobFG(int jobID);
int deleteJobBG(int jobID);
void child_handler(int sig);
void fg(char* jobMoveID);
void bg(char* jobMoveID);
job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg);

//The pidH variable is used for various places in the code to suspend the process
pid_t pidH = 0;

//These variables are used for determining if the SIGINT and SIGTSTP handlers have been called or not.
int intSig = 0;
int tstpSig = 0;

//Foreground and background job lists for maintaing records of job control
jobList jobsBG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0};
jobList jobsFG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0};


/*
 * @param jobPID the process id of the desired job
 * @return the job with the matching pid or NULL
 * Purpose: Used in the SIGCHLD handler to get the job with the pid returned from waitpid()
 */

job* getJob(int jobPID){
    if(jobsFG.head != NULL && (jobsFG.head->processID == jobPID)){
            return jobsFG.head;
            printf("Lemon\n");
    }
    
    job* currentJob = jobsBG.head;
    printf("Desired pid is %d\n", jobPID);
    if(currentJob != NULL){
        while(currentJob != NULL && (currentJob->processID != jobPID)){
            printf("Current job pid is %d", currentJob->processID);
            sleep(2);
            currentJob = currentJob->nextJob;
            if(currentJob->jobID < 1 || currentJob->processID == 0){
                currentJob = NULL;
                break;
            }
            printf("La bla\n");
        }
    } 
    //printf("Current job command is %s", currentJob->cmdline);
    return currentJob;
}

/**
 * @param jobID the job id of the desired job
 * @return the job with the matching job id or NULL
 * Purpose: Fetch a job from the foreground
 */

job* getJobFG(int jobID){
    printf("Cantaloupe\n");
    if(jobsFG.head != NULL && jobsFG.head->jobID == jobID){
        return jobsFG.head;
    } else {
        log_fg_notfound_error(jobID);
        return NULL;
    }
}

/**
 * @param jobID the job id of the desired job
 * @return the job with the matching job id or NULL
 * Purpose: Fetch a job from the background
 */

job* getJobBG(int jobID){
    printf("Lettuce\n");
    job* currentJob = jobsBG.head;
    if(currentJob != NULL){
        printf("Coca cola\n");
        while(currentJob != NULL && currentJob->jobID != jobID){
            currentJob = currentJob->nextJob;
        }
    } else {
        log_no_bg_error();
    }
    return currentJob;
}

/**
 * @param newJob a pointer to a pointer of a new job
 * Purpose: Add a job to the foreground
 * */

void addJobFG(job** newJob){
    printf("Radish\n");
    if(jobsFG.head == NULL){
        if((*newJob)->jobID < 0){
            printf("Darn\n");
            (*newJob)->jobID = 0;
        }
        if((*newJob)->bgIndicator != 0){
            (*newJob)->bgIndicator = 0;
        }

        jobsFG.head = *newJob;
        jobsFG.tail = *newJob;
        printf("New job id is %d\n", (*newJob)->jobID);
        printf("New job id for jobsFG is %d\n", (jobsFG.head)->jobID); 
        jobsFG.numJobs++;
        jobsFG.numJobsRunning++;
    }
    return;
}
/**
 * @param newJob a pointer to a pointer of a new job
 * Purpose: Add job to the background
 */

void addJobBG(job** newJob){
    if((*newJob)->bgIndicator != 1){
        (*newJob)->bgIndicator = 1;
    }
    printf("Limes\n");
    job* tempJob = jobsBG.head;
    job* beforeTemp = NULL;
    if(jobsBG.head != NULL && jobsBG.head->cmdline != NULL){
        //if its a new job and the list is not empty
        if((*newJob)->jobID < 1){
            printf("Beets\n");
            if(jobsBG.tail->nextJob == NULL){
                printf("Rutabaga\n");
                jobsBG.tail->nextJob = (job*) malloc(sizeof(job));
            }
            jobsBG.tail->nextJob = *newJob;
            (*newJob)->jobID = (jobsBG.tail->jobID) + 1;
            jobsBG.tail = jobsBG.tail->nextJob;
        } else {
            //it is not a new job and list is not empty
            //even if head of list has equal jobID we insert after
            while(tempJob->jobID >= (*newJob)->jobID){
                beforeTemp = tempJob;
                tempJob = tempJob->nextJob;
                printf("Ok\n");
            }
                beforeTemp->nextJob = *newJob;
                (*newJob)->nextJob = tempJob;
                if(tempJob == NULL){
                    jobsBG.tail = *newJob;
                }
        }
    } else {
        //list is empty
        printf("Potatos\n");
        jobsBG.head = *newJob;
        jobsBG.tail = *newJob;
        (*newJob)->jobID = 1;
        printf("Job id of bg head is %d", (jobsBG.head)->jobID);
    }
    jobsBG.numJobs++;
    jobsBG.numJobsRunning++;
    return;
}

/**
 * @param jobID the job id of the desired job
 * @return returns different integers depending on the success of the operation
 * Purpose: Deletes a job with the desired job id from the foreground
 */

int deleteJobFG(int jobID){
    printf("Job ID is %d", jobID);
    printf("Job id of foreground head is %d\n", jobsFG.head->jobID);
    if(jobsFG.head == NULL){
        printf("I do declare\n");
    }
    if(jobsFG.head != NULL && (jobsFG.head->jobID == jobID)){
        free(jobsFG.head->status);
        free(jobsFG.head->cmdline);
        //free(jobsFG.head->nextJob);
        free(jobsFG.head);
        jobsFG.head = NULL;
        jobsFG.tail = NULL;
        jobsFG.numJobs--;
        jobsFG.numJobsRunning--;
        printf("Plantains\n");
        return 0;
    } else {
        printf("Clementines\n");
        return 2;
    }
}

/**
 * @param jobID the job id of the desired job
 * @return returns different integers depending on the success of the operation
 * Purpose: Deletes a job with the desired job id from the background
 */

int deleteJobBG(int jobID){  
    job* deleteJob = jobsBG.head;
    job* beforeDelete = NULL;

    printf("Desired job ID is %d\n", jobID);
    if(deleteJob != NULL){
        //if job is head
        printf("Raspberries\n");
        if(jobID == deleteJob->jobID){
            printf("Blackberries\n");
            //if(deleteJob->nextJob != NULL && deleteJob->cmdline != NULL){       
            if((deleteJob->nextJob) != NULL){
                printf("Peaches\n");      
                jobsBG.head = jobsBG.head->nextJob;
            }
            printf("Delete job status is %s\n", deleteJob->status);
            printf("JobsBG.head status is %s\n", (jobsBG.head)->status); 
            //Why am I not allowed to free this???
            free(deleteJob->status);
            printf("Coconuts\n");
            free(deleteJob->cmdline);
           // if(deleteJob->nextJob == NULL || deleteJob->cmdline == NULL || *(deleteJob->nextJob->cmdline) == '\0'){
           if(deleteJob->nextJob == NULL){
                printf("HELLPPP ME\n");
                jobsBG.head = NULL;
                jobsBG.tail = NULL;
            }
            free(deleteJob);
            deleteJob = NULL;
        } else {
            
            while(deleteJob->jobID != jobID){
                    printf("Job is: %d\n", deleteJob->jobID);
                    //printf("Before delete is %d\n", beforeDelete->jobID);
                    beforeDelete = deleteJob;
                    deleteJob = deleteJob->nextJob;
                    if(deleteJob == NULL){
                        printf("Job not found!\n");
                        return -1;
                    }
            }

                //if job is intermediate node
                //if(deleteJob->nextJob != NULL && deleteJob->nextJob->cmdline != NULL){
                if(deleteJob->nextJob != NULL){
                    beforeDelete->nextJob = deleteJob->nextJob;
                } else if(deleteJob->processID == jobsBG.tail->processID){
                    jobsBG.tail = beforeDelete;
                    jobsBG.tail->nextJob = NULL;
                }
                
                free(deleteJob->status);
                free(deleteJob->cmdline);
                free(deleteJob);
                deleteJob = NULL;
        }

        jobsBG.numJobsRunning--;
        jobsBG.numJobs--;
        return 0;
    } else {
        printf("Empty job list!\n");
        return 2;
    }
}

/**
 * @param sig the signal sent to the handler, SIGCHLD
 * Purpose: Performs job log maintenance and reaps children
 */

void child_handler(int sig){
    int child_status  = NULL;
    printf("Sig is %d\n", sig);
        printf("Tomatoes\n");
        printf("Pid is %d", pidH);
        while((pidH = waitpid(-1, &child_status, WNOHANG)) > 0){
        printf("Child status is %d\n pidH is %d\n", child_status, pidH);
        if (pidH < 0) {
            if (errno == ECHILD) {
              printf("Child does not exist\n");
            }
            else if (errno == EINVAL){
              printf("Bad argument passed to waitpid\n");
            } else {
                printf("Wtf\n");
          }
        }
        printf("PidH is %d", pidH);

        job* tempJob = getJob(pidH);
        if(WIFSTOPPED(child_status)){ 
            printf("Mangos\n");
            if(intSig == 1){
                intSig = 0;
            } else if(tempJob->bgIndicator == 0){
                sigset_t mask;
                sigpending(&mask);
                if(sigismember(&mask, SIGTSTP) && strcmp(tempJob->status, "Running")){
                    printf("Hold yer horses\n"); 
                    log_job_fg_stopped(tempJob->processID, tempJob->cmdline);
                    job* newJob = makeJob(tempJob->jobID, tempJob->processID, tempJob->status, tempJob->cmdline, tempJob->nextJob, tempJob->bgIndicator);
                    strcpy(newJob->status, "Stopped");
                    addJobBG(&newJob);
                    deleteJobFG(newJob->jobID);
                }
            } else {
                printf("Giddyup\n");
                strcpy(tempJob->status, "Stopped");
                log_job_bg_stopped(tempJob->processID, tempJob->cmdline);
            }   
        } else if(WIFCONTINUED(child_status)){
            strcpy(tempJob->status, "Running");
            printf("Blueberries.\n");
            if(tempJob->bgIndicator == 0){
                log_job_fg_cont(tempJob->processID, tempJob->cmdline);
            } else {
                log_job_bg_cont(tempJob->processID, tempJob->cmdline);
            }
        } else if(WIFEXITED(child_status)){
            printf("Passion fruit\n");
            if(tempJob == NULL){
                printf("Ketchup\n");
            } else {
                if(tempJob->bgIndicator == 0){
                    log_job_fg_term(tempJob->processID, tempJob->cmdline);
                    deleteJobFG(tempJob->jobID);
                    printf("Bubblegum\n");
                } else {
                    log_job_bg_term(tempJob->processID, tempJob->cmdline);
                    deleteJobBG(tempJob->jobID);
                }
            }
        } else if(WIFSIGNALED(child_status)){
            printf("Child status is %d\n", child_status);
            if(tstpSig == 1){
                tstpSig = 0;
            } else if(tempJob->bgIndicator == 0){
                //printf("Pineapples\n");
                log_job_fg_term_sig(tempJob->processID, tempJob->cmdline);
                deleteJobFG(tempJob->jobID);
            } else {
                //printf("Oranges\n");
                log_job_bg_term_sig(tempJob->processID, tempJob->cmdline);
                deleteJobBG(tempJob->jobID);
            }
        }
    } 
    printf("Corn\n");
    pidH = -1;
}
/**
 * @param the signal sent to the handler, SIGINT
 * Purpose: Kills the foreground process with receipt of CTRL+C input from keyboard
 */    
void int_handler(int sig){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK,&mask, NULL);
    intSig = 1;
    if(jobsFG.head != NULL){
        if(kill(jobsFG.head->processID, SIGTERM) == -1){
            printf("Hello\n");
        } 
    }
    pidH = -1;
    printf("Sushi\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}
/**
 * @param the signal sent to the handler, SIGTSTP
 * Purpose: Stops the foreground process and moves to background with receipt of CTRL+Z input from keyboard
 */    

void tstp_handler(int sig){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK,&mask, NULL);
    tstpSig = 1; 
    job* tempJob = jobsFG.head;
    printf("Hola como esta\n");
    if(tempJob != NULL){
        job* newJob = makeJob(tempJob->jobID, tempJob->processID, tempJob->status, tempJob->cmdline, tempJob->nextJob, tempJob->bgIndicator);
        strcpy(newJob->status, "Stopped");
        deleteJobFG(newJob->jobID);
        printJobs();
        addJobBG(&newJob);
        printJobs();
        jobsBG.numJobsRunning--; 
        if(kill(newJob->processID, SIGTSTP) == -1){
            printf("Really?\n");
        }
    } else {
        printf("Grapefruit\n");
    }

    pidH = -1;
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

}
/**
 * @param jobMoveID the job id of the desired job
 * Purpose: Continues a background job with the given id and moves it to the foreground 
 */

void fg(char* jobMoveID){

//move job in arg[1] to foreground
//if job ID arg is blank then get job id of bg job with highest id
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigemptyset(&prev_mask);
    sigaddset(&mask, SIGCHLD);
    int jobID = 0;

    if(jobMoveID == NULL){
        printf("Melon\n");
        jobID = jobsBG.tail->jobID;
        printf("job ID is %d\n", jobID);
    } else {
        printf("Cucumber\n");
        jobID = atoi(jobMoveID);
        printf("job ID is actually %d\n", jobID);
    }

    //try to find the job in the background list
    job* jobBG = getJobBG(jobID);
    if(jobBG != NULL){
            printf("Job id is %d process id is %d status is %s cmdline is %s bdIndicator is %d\n", jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, jobBG->bgIndicator);

            job* newJob = makeJob(jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, NULL, jobBG->bgIndicator); 
            int test = 0;
            printf("New job process id %d", newJob->processID);
            if(strcmp(newJob->status, "Stopped") == 0 && (test = kill(newJob->processID, SIGCONT)) == -1){
        //do I really need this free?
                free(newJob);
                log_job_fg_fail(newJob->processID, newJob->cmdline);
            } else {
                pidH = 0;
                printf("pidH is now %d", pidH);
                sigprocmask(SIG_BLOCK, &mask, &prev_mask);
                while(!pidH){
                    printf("pidH is %d", pidH);
                    sigsuspend(&prev_mask);
                    printf("PidH is %d", pidH);
                }
                printf("Test is %d", test);
                printf("Kill is %d\n", test);
                printf("Pid of continued process %d", pidH);
                newJob->bgIndicator = 0;
                strcpy(newJob->status, "Running");
                addJobFG(&newJob);
                printf("Pickles\n");
                deleteJobBG(jobID);
                printf("Fries\n");
                printf("Job id is %d process id is %d status is %s cmdline is %s bdIndicator is %d\n", newJob->jobID, newJob->processID, newJob->status, newJob->cmdline, newJob->bgIndicator);
            }

            if(strcmp(newJob->status, "Running") == 0){
                printf("Star fruit\n");
                pidH = 0;
                printf("pidH is now %d", pidH);
                while(!pidH){
                    printf("pidH is %d", pidH);
                    sigsuspend(&prev_mask);
                    printf("PidH is %d", pidH);
                }
                printf("Green melon\n");
                log_job_fg(jobBG->processID, jobBG->cmdline);
                sigprocmask(SIG_UNBLOCK, &mask, NULL);
            }
    } else {
            printf("Blah!\n");
            log_bg_notfound_error(jobID);
    }
    return;
}
/**
 * @param the job id of the desired job
 * Purpose: Continues a background job with the given job id
 */
void bg(char* jobMoveID){
    int jobID = 0;
    if(jobMoveID == NULL){
        printf("Melon\n");
        jobID = jobsBG.tail->jobID;
    } else {
        printf("Cucumber\n");
        jobID = atoi(jobMoveID);
    }

    job* bgJob = getJobBG(jobID);
    printf("Job BG cmd is %s\n", bgJob->cmdline);
     
    if(bgJob!= NULL){
        if(strcmp(bgJob->status, "Stopped") == 0){
            //printJobs();
            printf("Process ID of bgJob is %d", bgJob->processID);
            int test = 0;
            if((test = kill(bgJob->processID, SIGCONT)) == -1){
                log_job_bg_fail(bgJob->processID, bgJob->cmdline);
            } else {
                printf("Just try again\n");
                printf("Value of kill is %d\n", test);
                log_job_bg(bgJob->processID, bgJob->cmdline); 
                (jobsBG.numJobsRunning)++;
            }
        } else {
            printf("Job already running.\n");
        }
    } else {
        log_bg_notfound_error(jobID);
    }
    return;
}

/**
 * Purpose: prints jobs in background
 */

void printJobs(void){
    //printf("BLAH!");
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigemptyset(&prev_mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    job* tempJob = jobsBG.head;
    if(tempJob == NULL){
        printf("No jobs!\n");
    } else {
        printf("job id of bg head is %d", (jobsBG.head)->jobID);
        log_job_number(jobsBG.numJobsRunning);
        printf("Temp job command line is %s\n", tempJob->cmdline);
        printf("First char of command line is %c\n", *(tempJob->cmdline));
        //while(tempJob != NULL && tempJob->cmdline != NULL && *(tempJob->cmdline) != '\0'){
        while(tempJob != NULL && tempJob->processID != 0){
            printf("tempJop jobID is %d\n", tempJob->jobID);
            log_job_details(tempJob->jobID, tempJob->processID, tempJob->status, tempJob->cmdline);
            tempJob = tempJob->nextJob;
            printf("Spinach\n"); 
        }
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return;
}

/**
 * @param jobID the job id of the job
 * @param pid the process id of the job
 * @param status the status of the job (i.e. is it running or is it stopped?)
 * @param cmd the job command
 * @param nextJob the next job in the job list
 * @param bg indicates if the job is in the background or not
 * @return a new job with the variables specified on input
 */

job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg){
    job* newJob = (job*) malloc(sizeof(job));
    newJob->jobID = jobID;
    newJob->processID = pid;

    newJob->status = malloc(11 * sizeof(char));
    *(newJob->status) = '\0';
    if(status == NULL){
        strcat(newJob->status,"Running");
    } else {
        strcat(newJob->status, status);
    }

    newJob->cmdline = malloc(MAXLINE * sizeof(char));
    *(newJob->cmdline) = '\0';
    if(cmd != NULL){
        strcat(newJob->cmdline, cmd);
    }
    if(nextJob != NULL){
        newJob->nextJob = (job*) malloc(sizeof(job));
    //if(nextJob != NULL){
        newJob->nextJob = nextJob;
    }
    newJob->bgIndicator = bg;
    return newJob;
} 

/**
 * Purpose: A shell program that accepts input and performs minor tasks related to file maintenance.
 * Supposedly performs job control.
 */

int main(){
    //signal(SIGCHLD, child_handler);

    char cmdline[MAXLINE]; 
    char cmdlineOriginal[MAXLINE];
    char* token;
    char* progName;
    //max args include program name
    char** args = calloc(MAXARGS + 1, sizeof(char*));
    int argNum = 0;
    int bgIndicator = 0; //foreground
    int fdRead;
    int fdWrite;
    int fdApp;
    job* newJob;

    sigset_t block_mask, prev_mask, child_mask, int_mask, tstp_mask;

    sigemptyset(&child_mask);
    sigemptyset(&int_mask);
    sigemptyset(&tstp_mask);
    sigemptyset(&block_mask);

    sigaddset(&block_mask, SIGCHLD);
    sigaddset(&child_mask, SIGINT);
    sigaddset(&child_mask, SIGTSTP);

    sigaddset(&int_mask, SIGCHLD);
    sigaddset(&int_mask, SIGTSTP);

    sigaddset(&tstp_mask, SIGINT);
    sigaddset(&tstp_mask, SIGINT);

    struct sigaction sig_child;
    struct sigaction sig_int;
    struct sigaction sig_tstp;

    sig_child.sa_handler = child_handler; 
    sig_int.sa_handler = int_handler;
    sig_tstp.sa_handler = tstp_handler;

    sig_child.sa_flags = 0;
    sig_int.sa_flags = 0;
    sig_tstp.sa_flags = 0;

    //what other sigs to add to block
    sig_child.sa_mask = child_mask;
    sig_tstp.sa_mask = tstp_mask;
    sig_int.sa_mask = int_mask;

    sigaction(SIGINT, &sig_int, NULL);
    sigaction(SIGTSTP, &sig_tstp, NULL);
    sigaction(SIGCHLD, &sig_child, NULL);
    //int child_status = NULL;


    while (1) {
        argNum = 0;
        bgIndicator = 0;
        cmdlineOriginal[0] = '\0';
        cmdline[0] = '\0';
        fdRead = -1;
        fdWrite = -1;
        fdApp = -1;
        log_prompt();

    
        // note: fgets will keep the ending '\'
        if (fgets(cmdline, MAXLINE, stdin)==NULL){
            if (errno == EINTR){
                continue;
                exit(-1); 
            }
        } else {
            strcat(cmdlineOriginal, cmdline);
            progName = strtok(cmdline, " \n");
            if(progName != NULL){
                token = progName;
                args[argNum] = progName;
                argNum++;
                while(token != NULL){
                    token = strtok(NULL, " \n");
                    if(token == NULL){
                        break;
                    }
                
                    if(strchr(token, '<') != NULL){
                        printf("Oh\n");
                        if(strlen(token) > 1){
                            memmove(token, token + 1, strlen(token));
                        } else {
                            token = strtok(NULL, " \n");
                        }
                       if((fdRead = open(token, O_RDONLY, 0600)) < 0){
                            log_file_open_error(token);
                        }
                    } else if((strchr(token, '>') != NULL) && (strstr(token, ">>") == NULL)){
                        printf("Bla\n");
                        if(strlen(token) > 1){
                            memmove(token, token + 1, strlen(token));
                        } else {
                            token = strtok(NULL, " \n");
                        }
                        printf("Token is %s", token);
                        if((fdWrite = open(token, O_CREAT|O_WRONLY|O_TRUNC, 0600)) < 0){
                            log_file_open_error(token);
                        }
                    } else if(strstr(token, ">>") != NULL){
                        printf("Da\n");
                        if(strlen(token) > 2){
                            memmove(token, token + 2, strlen(token));
                        } else {
                            token = strtok(NULL, " \n");
                        }
                        if((fdApp = open(token, O_CREAT|O_WRONLY|O_APPEND, 0600)) > 0){
                            log_file_open_error(token);
                        }
                    } else {
                        args[argNum] = malloc(strlen(token) + 1);
                        strcpy(args[argNum], token);
                        argNum++;
                    }
                }
            //check if foreground or background
                if(argNum > 1 && strcmp(args[argNum - 1], "&") == 0){
                    bgIndicator = 1;
                    args[argNum - 1] = '\0';
                    argNum--;
                    printf("Popcorn\n");
                }
        
                if(strcmp(progName, "help") == 0){
                    log_help();
                } else if(strcmp(progName, "quit") == 0){
                    log_quit();
                    //make sure to reap any outstanding children here
                    exit(0);
                } else if(strcmp(progName, "jobs") == 0){
                    printJobs();
                } else if(strcmp(progName, "fg") == 0){
                    fg(args[1]);
                    printf("Pomellos\n");
                } else if(strcmp(progName, "bg") == 0){
                    bg(args[1]);
                } else if(strcmp(progName, "kill") == 0){
                    //run kill ... signal to certain job
                    printf("kill!\n");
                    memmove(args[1], args[1]+1, strlen(args[1]));
                    if(kill(atoi(args[2]), atoi(args[1])) == -1){
                        log_kill_error(atoi(args[2]), atoi(args[1]));
                    }
                } else {
                    newJob = makeJob(-1,-1, "Running", cmdlineOriginal, NULL, bgIndicator); 
                    //must block child signal before fork
                    sigprocmask(SIG_BLOCK, &block_mask, NULL);
                    //if parent process
                    if(pidH = fork()){
                        //update job list for child! 
                        if(pidH != -1){
                            if(bgIndicator == 1){
                                log_start_bg(newJob->processID, progName);
                            }
                        }          
                        newJob->processID = pidH;
                        if(bgIndicator == 0){    
                            addJobFG(&newJob);
                        } else {
                            addJobBG(&newJob);
                        }
                    
                        if(bgIndicator == 0){
                            pidH = 0;
                            printf("Squash\n");
                            while(!pidH){
                                printf("La la la\n");
                                sigsuspend(&prev_mask);
                            }
                        }
                        sigprocmask(SIG_UNBLOCK, &block_mask, NULL);

                        if(fdWrite > -1){
                            close(fdWrite);
                        }
                        if(fdRead > -1){
                            close(fdWrite);
                        }
                        if(fdApp > -1){
                            close(fdApp);
                        }

                    } else {
                        sigprocmask(SIG_UNBLOCK, &block_mask, NULL);
                  
                        int i = setpgid(0, 0);
                        printf("setpgid is %d\n", i);
                          //printf("Errno is %d", errno);
                        if(fdRead > -1){
                            dup2(fdRead, 0);
                            close(fdRead);
                        } 
                        
                        if(fdWrite > -1){
                            dup2(fdWrite, 1);
                            close(fdWrite);
                        }

                        if(fdApp > -1){
                            dup2(fdApp, 1);
                            close(fdApp);
                        }
                        
                        if(execv(progName, args) == -1){
                            log_command_error(progName);
                            if(bgIndicator == 0){    
                                deleteJobFG(newJob->jobID);
                            } else {
                                deleteJobBG(newJob->jobID);
                            }
                        }
                    }
                }
            }
        }
          		
        //printf("Parent id is %d, child id is %d Child status is: %d", getpid(), pid, child_status);
        //printJobs();
	    if (feof(stdin)) {
	        	exit(0);
	    }
    } 
}

