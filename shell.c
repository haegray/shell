/* This is the only file you should update and submit. */

/* Fill in your Name and GNumber in the following two comment fields
 * Name: Helena Gray
 * GNumber: G01086877
 */

#include "logging.h"
#include "shell.h"
typedef struct job job;
typedef struct jobList jobList;

/* Feel free to define additional functions and update existing lines */
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

void printJobs(jobList jobs);
job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg);


int pid = -1;

/**
//used in child handler
job* getJob(int jobPID){
    if(jobsFG.head != NULL && (jobsFG.head->processID == jobPID)){
            return jobsFG.head;
    }
    
    job* currentJob = jobsBG.head;
    if(currentJob != NULL){
        while(currentJob != NULL && (currentJob->processID != jobPID)){
            currentJob = currentJob->nextJob;
        }
    } 
    return currentJob;
}

//used in the fg and bg functions
job* getJobFG(int jobID){
    if(jobsFG.head != NULL && jobsFG.head->jobID == jobID){
        return jobsFG.head;
    } else {
        log_fg_notfound_error(jobID);
        return NULL;
    }
}

void moveJobFG(int jobID){
    job* newJob = jobsFG.head;
    job* tempJob = jobsFG.head;
    job* beforeTemp = NULL;
    if(jobsBG.head != NULL && jobsBG.head->cmdline != NULL){
        //if its a new job and the list is not empty
        if(newJob->jobID < 1){
            jobsBG.tail->nextJob = newJob;
            newJob->jobID = (jobsBG.tail->jobID) + 1;
            jobsBG.tail = jobsBG.tail->nextJob;
        } else {
            //it is not a new job and list is not empty
            //even if head of list has equal jobID we insert after
            while(tempJob->jobID >= newJob->jobID){
                beforeTemp = tempJob;
                tempJob = tempJob->nextJob;
            }
                beforeTemp->nextJob = newJob;
                newJob->nextJob = tempJob;
                if(tempJob == NULL){
                    jobsBG.tail = tempJob;
                }
        }
    } else {
        //list is empty
        jobsBG.head = newJob;
        jobsBG.tail = newJob;
        newJob->jobID = 1;
    }
    jobsBG.numJobs++;
    jobsBG.numJobsRunning++;
    jobsFG.head = NULL;
    
    printf("Testing add jobs!\n");
    return;
}

void moveJobBG(int jobID){
    job* moveJob = jobsBG.head;
    job* beforeMove = jobsBG.head;

    if(moveJob != NULL){
        while(moveJob->jobID != jobID){
                printf("Job is: %d", moveJob->jobID);
                beforeMove = moveJob;
                moveJob = moveJob->nextJob;
        }
        //if this is the head nide
        if(moveJob->jobID == beforeMove->jobID){
            jobsBG.head = moveJob->nextJob;
        } else if(moveJob->nextJob == NULL){
            //if its the tail node
            jobsBG.tail = beforeMove;
        } else {
            beforeMove->nextJob = moveJob->nextJob;
        }
    }
    jobsFG.head = moveJob;
    jobsFG.tail = moveJob;
    jobsFG.numJobs++;
    jobsBG.numJobs--;
    return;
}

    
job* getJobBG(int jobID){
    job* currentJob = jobsBG.head;
    if(currentJob != NULL){
        while(currentJob->jobID != jobID && currentJob != NULL){
            currentJob = currentJob->nextJob;
        }
        if(currentJob == NULL){
            log_bg_notfound_error(jobID);
        }
    } else {
        log_no_bg_error();
    }
    return currentJob;
}



void addJobFG(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg){
    job* newJob = makeJob(jobID, pid, status, cmd, nextJob, bg);
    if(jobsFG.head != NULL){
        printf("Job error!\n");
    } else {
        printf("Successful!");
        jobsFG.head = newJob;
        if(newJob->jobID < 0){
            newJob->jobID = 0;
        }
        printf("Foreground head command is %s", jobsFG.head->cmdline);
        jobsFG.numJobs++;
        jobsFG.numJobsRunning++;
    }
    return;
}
*/
void addJobBG(jobList* jobsBG, job** newJob){
      printf("The pid of the current process is %d\n", getpid());

    job* tempJob = jobsBG->head;
    job* beforeTemp = NULL;
    if(jobsBG->head != NULL && jobsBG->head->cmdline != NULL){
        //if its a new job and the list is not empty
        if((*newJob)->jobID < 1){
            jobsBG->tail->nextJob = *newJob;
            (*newJob)->jobID = jobsBG->tail->jobID + 1;
            jobsBG->tail = jobsBG->tail->nextJob;
        } else {
            //it is not a new job and list is not empty
            //even if head of list has equal jobID we insert after
            while(tempJob->jobID >= (*newJob)->jobID){
                beforeTemp = tempJob;
                tempJob = tempJob->nextJob;
            }
                beforeTemp->nextJob = (*newJob);
                (*newJob)->nextJob = tempJob;
                if(tempJob == NULL){
                    jobsBG->tail = tempJob;
                }
        }
    } else {
        //list is empty
        jobsBG->head = *newJob;
        jobsBG->tail = *newJob;
        (*newJob)->jobID = 1;
    }
    jobsBG->numJobs++;
    jobsBG->numJobsRunning++;
    printf("Testing add jobs!\n");
    return;
}
/**
int deleteJobFG(int jobID){
    printf("Job id of foreground head is %d\n", jobsFG.head->jobID);
    if(jobsFG.head != NULL && (jobsFG.head->jobID == jobID)){
        free(jobsFG.head->status);
        free(jobsFG.head->cmdline);
        free(jobsFG.head->nextJob);
        free(jobsFG.head);
        jobsFG.head = NULL;
        jobsFG.tail = NULL;
        return 0;
    } else {
        return 2;
    }
}
*/
int deleteJobBG(jobList* jobsBG, int jobID){  
    job* deleteJob = jobsBG->head;
    job* beforeDelete = jobsBG->head;
    if(deleteJob != NULL){
        //if job is head
        if(jobID == deleteJob->jobID){
            if(deleteJob->nextJob != NULL){
                job* alias = jobsBG->head->nextJob;
                beforeDelete = makeJob(alias->jobID,alias->processID, alias->status,alias->cmdline,alias->nextJob,alias->bgIndicator);
                jobsBG->head = beforeDelete;
            } else {
                jobsBG->head = NULL;
                jobsBG->tail = NULL;
            }
            free(deleteJob->status);
            free(deleteJob->cmdline);
            free(deleteJob->nextJob);
            free(deleteJob);
        } else {
            while(deleteJob->jobID != jobID){
                    printf("Job is: %d\n", deleteJob->jobID);
                    beforeDelete = deleteJob;
                    deleteJob = deleteJob->nextJob;
                    if(deleteJob == NULL){
                        printf("Job not found!\n");
                        return -1;
                    }
            }
           //if job is tail
            if(deleteJob->processID == jobsBG->tail->processID){
                free(deleteJob->status);
                free(deleteJob->cmdline);
                free(deleteJob->nextJob);
                free(deleteJob);
                beforeDelete->nextJob = NULL;
                jobsBG->tail = beforeDelete;
            } else {
                //if job is intermediate node
                if(deleteJob->nextJob != NULL && deleteJob->nextJob->cmdline != NULL){
                    job* alias = (deleteJob)->nextJob;
                    printf("Alias process id is %d\n", alias->processID);
                     beforeDelete->nextJob = makeJob(alias->jobID, alias->processID, alias->status,alias->cmdline,alias->nextJob,alias->bgIndicator);
                    printf("Alias process id is %d\n", alias->processID);

                    free(deleteJob->status);
                    free(deleteJob->cmdline);
                    deleteJob->nextJob = NULL;
                    free(deleteJob);
                     printf("Alias process id is %d\n", alias->processID);

                }    
            }
        }

        (jobsBG->numJobs)--;
        return 0;
    } else {
        printf("Empty job list!\n");
        return 2;
    }
}
/**
void child_handler(int sig){
    int child_status;
    pid_t pidH = waitpid(pid, &child_status, WNOHANG|WCONTINUED|WUNTRACED);
    job* tempJob = getJob(pidH);
    if(WIFCONTINUED(child_status)){
        tempJob->status = "Running";
        if(tempJob->bgIndicator == 0){
            log_job_fg_cont(tempJob->processID, tempJob->cmdline);
        } else {
            log_job_bg_cont(tempJob->processID, tempJob->cmdline);
        }
    } else if(WIFSTOPPED(child_status)){
        tempJob->status = "Stopped";
        if(tempJob->bgIndicator == 0){
            log_job_fg_stopped(tempJob->processID, tempJob->cmdline);
        } else {
            log_job_bg_stopped(tempJob->processID, tempJob->cmdline);
        }
    } else if(WIFEXITED(child_status)){
        if(tempJob->bgIndicator == 0){
            log_job_fg_term(tempJob->processID, tempJob->cmdline);
        } else {
            log_job_bg_term(tempJob->processID, tempJob->cmdline);
        }
        if(tempJob->bgIndicator == 0){
            deleteJobFG(tempJob->jobID);
        } else {
            //deleteJobBG(tempJob->jobID);
        }
    } else if(WTERMSIG(child_status)){
        //what to do if job is terminated???
        if(tempJob->bgIndicator == 0){
            log_job_fg_term_sig(tempJob->processID, tempJob->cmdline);
        } else {
            log_job_bg_term_sig(tempJob->processID, tempJob->cmdline);
        }
         if(tempJob->bgIndicator == 0){
            deleteJobFG(tempJob->jobID);
        } else {
            //deleteJobBG(tempJob->jobID);
        }
    }
}



void fg(jobList* jobsBG, jobList* jobsFG, int jobID, char* jobIDMove){
 //move job in arg[1] to foreground
         
            printf("fg!\n");
            //if job ID arg is blank then get job id of bg job with highest id
            if(jobIDMove == NULL){
                jobID = jobsBG->tail->jobID;
            } else {
                jobID = atoi(jobIDMove);
            }

            //try to find the job in the background list
            job* jobBG = getJobBG(jobID);
            if(jobBG != NULL){
                jobBG->bgIndicator = 0;
                addJobFG(jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, jobBG->nextJob, jobBG->bgIndicator);
                // deleteJobBG(jobID);
                //strcopy(newJob->status, "Running");
                if(kill(jobBG->processID, SIGCONT) == -1){
                    log_job_fg_fail(jobBG->processID, jobBG->cmdline);
                    //strcopy(newJob->status, "Stopped");
                } else {
                    log_job_fg(jobBG->processID, jobBG->cmdline);
                    (jobsBG->numJobsRunning)--;
                    (jobsFG->numJobsRunning)++;//switch job status to running
                }
                //use signal handler to show child is running again
            } else {
                log_bg_notfound_error(jobID);
            }
    return;
}

void bg(jobList* jobsBG, char* jobID){
    int jobNum = 0;
    if(jobID != NULL){
        jobNum = atoi(jobID);
    }
    job* bgJob = getJobBG(jobNum);
    if(bgJob->cmdline != NULL && strcmp((bgJob->status), "Running")){
        if(kill(bgJob->processID, SIGCONT) == -1){
            //should the process be stopped if signal fails to send?
            strcpy(bgJob->status, "Stopped");
            log_job_bg_fail(bgJob->processID, bgJob->cmdline);
        } else {
            strcpy(bgJob->status, "Running");
            log_job_bg(bgJob->processID, bgJob->cmdline); 
            (jobsBG->numJobsRunning)++;
        }
    } 
    return;
}
*/
void printJobs(jobList jobsBG){
    //printf("BLAH!");
    job* tempJob = jobsBG.head;
    if(tempJob == NULL){
        printf("No jobs!\n");
    } else {
        log_job_number(jobsBG.numJobsRunning);
        while(tempJob != NULL && tempJob->jobID > 0){
            log_job_details(tempJob->jobID, tempJob->processID, tempJob->status, tempJob->cmdline);
            tempJob = tempJob->nextJob; 
        }
    }
   // if(jobsFG.head != NULL){
        printf("FG jobs\n");
   //     log_job_details(jobsFG.head->jobID, jobsFG.head->processID, jobsFG.head->status, jobsFG.head->cmdline);
   // }
    return;
}

job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg){
    job* newJob = (job*) malloc(sizeof(job));
    newJob->jobID = jobID;
    newJob->processID = pid;
    newJob->status = malloc(11 * sizeof(char));
    strcat(newJob->status,"Running");
    newJob->cmdline = malloc(MAXLINE * sizeof(char));
    strcat(newJob->cmdline, cmd);
    newJob->nextJob = (job*) malloc(sizeof(job));
    newJob->nextJob = nextJob;
    newJob->bgIndicator = bg;
    return newJob;
} 
 
/* main */
/* The entry of your shell program */

/**
int main(){
    //signal(SIGCHLD, child_handler);

    char cmdline[MAXLINE]; 
    char cmdlineOriginal[MAXLINE];
    char* token;
    char* progName;
//Is maxargs for number of arguments including program name?
    char** args = calloc(MAXARGS + 1, sizeof(char*));
    int argNum = 0;
    int bgIndicator = 0; //foreground
    int jobID = -1;
    sigset_t mask, prev_mask;
    job* newJob;
    struct sigaction sig_child;
    sig_child.sa_handler = child_handler; 
    sig_child.sa_flags = 0;
    sigaction(SIGCHLD, &sig_child, NULL);
    sigaction(SIGTSTP, &sig_child, NULL);
    sigaction(SIGINT, &sig_child, NULL);
    int child_status = 0;
    int pid;


    while (1) {
        argNum = 0;
        bgIndicator = 0;
	    strcpy(cmdlineOriginal, "\0");
  	    log_prompt();

	
	    // note: fgets will keep the ending '\'
	    if (fgets(cmdline, MAXLINE, stdin)==NULL){
	   	    if (errno == EINTR){
			    continue;
                printf("Limes!\n");
	    	    exit(-1); 
            }
	    } else {
            strcat(cmdlineOriginal, cmdline);
            progName = strtok(cmdline, " \n");
            token = progName;
            args[argNum] = progName;
            argNum++;
            //printf("Prog name is %s\n", progName);
	        while(token != NULL){
                token = strtok(NULL, " \n");
               // printf("Token is %s\n", token);
               // printf("args num is %d\n", argNum);
                if(token == NULL){
                    //printf("Bananas\n");
                    break;
                }
                     
                args[argNum] = malloc(strlen(token) + 1);
                strcpy(args[argNum], token);
                argNum++;
		    }
            //check if foreground or background
            if(argNum > 1 && !strcmp(args[argNum - 1], "&")){
                bgIndicator = 1;
                args[argNum - 1] = '\0';
                argNum--;
            }
            
            newJob = makeJob(-1,-1, "Running", cmdlineOriginal, NULL, bgIndicator); 
            if(strcmp(progName, "help") == 0){
                log_help();
            } else if(strcmp(progName, "quit") == 0){
                log_quit();
                exit(0);
            } else if(strcmp(progName, "jobs") == 0){
                printJobs();
            } else if(strcmp(progName, "fg") == 0){
                //move job in argv[1] to foreground
                printf("fg!\n");
                fg(&jobsBG, &jobsFG, jobID, args[1]);
            } else if(strcmp(progName, "bg") == 0){
                //move job in argv[1] to background
                printf("bg!\n");
                bg(&jobsBG, args[1]);
            } else if(strcmp(progName, "kill") == 0){
                //run kill ... signal to certain job
                printf("kill!\n");
                //how to remove first - off of first argument
                memmove(args[0], args[0]+1, strlen(args[0]));
                if(!kill(atoi(args[0]), atoi(args[1]))){
                    log_kill_error(atoi(args[0]), atoi(args[1]));
                }
                //gotta make handlers for these types of signals
            } else {
                
                //must block child signal before fork
                sigemptyset(&mask);
                sigaddset(&mask, SIGCHLD);
                sigprocmask(SIG_BLOCK, &mask, &prev_mask);
                //if parent process
                progName = args[0];
                if(pid = fork()){
                    //update job book for child! 
                    newJob->processID = pid;
                        //printf("Apples\n");
                    addJob(&newJob);
                    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
                    if(bgIndicator == 0){
                        while(!WIFEXITED(child_status) && !WTERMSIG(child_status)){
                            pid = waitpid(pid, &child_status, WUNTRACED);
                        }
                    }
                } else {
                    if(bgIndicator == 1){
                        setpgid(0,0);
                    } else {
                        setpgid(newJob->processID, getpgrp());
                    }
                    log_start_bg(newJob->processID, progName);
                    strcpy(getJob(getpid())->status, "Running");
                    if(execv(progName, args) == -1){
                        log_command_error(progName);
                        deleteJob(newJob->jobID);
                    }
                }
            }
        }
          		
        printf("Parent id is %d, child id is %d Child status is: %d", getpid(), pid, child_status);
        printJobs();
	    if (feof(stdin)) {
                printf("This is your problem.\n");
	        	exit(0);
	    }
    } 
}
*/
/* end main */

int main(){
    jobList jobsBG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0};
    //jobList jobsFG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0}; 
    //job* deleteJob = makeJob(jobsBG.head->jobID, jobsBG.head->processID, jobsBG.head->status, jobsBG.head->cmdline, jobsBG.head->nextJob, jobsBG.head->bgIndicator);
    //job* beforeDelete = makeJob(jobsBG.head->jobID, jobsBG.head->processID, jobsBG.head->status, jobsBG.head->cmdline, jobsBG.head->nextJob, jobsBG.head->bgIndicator);
    //job* deleteJob = jobsBG.head;
    //job* beforeDelete = jobsBG.head;

    job* newJob = makeJob(-1, 1234, "Running", "sleep 100", NULL, 1);
    printJobs(jobsBG);
    addJobBG(&jobsBG, &newJob);
    newJob = makeJob(-1, 5678, "Running", "fg", NULL, 0);
    printf("\n");
    printJobs(jobsBG);

    newJob = makeJob(-1, 45123, "Running", "ls -l", NULL, 1);
    addJobBG(&jobsBG, &newJob);
    printJobs(jobsBG);

    newJob = makeJob(-1, 123123, "Running", "does this work", NULL, 1);
    addJobBG(&jobsBG, &newJob);
    printJobs(jobsBG);
    //addJobFG(3, 123123, "Running", "does this work", NULL, 1);
    //printJobs();
    printf("\n");
    //deleteJob = jobsBG.head;
    //beforeDelete = jobsBG.head;
    deleteJobBG(&jobsBG, 2);
    printJobs(jobsBG);
    //deleteJob = jobsBG.head;
    //beforeDelete = jobsBG.head;
    deleteJobBG(&jobsBG, 3);
    printJobs(jobsBG);
    return 0;

}
