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

jobList jobsBG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0};
jobList jobsFG = {.head = NULL, .tail = NULL, .numJobs = 0, .numJobsRunning = 0};
job* head;

void printJobs(void);
job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg);
job* getJob(int jobPID);
job* getJobFG(int jobID);
job* getJobBG(int jobID);
void addJobFG(job* newJob);
void addJobBG(job** newJob);
int deleteJobFG(int jobID);
int deleteJobBG(int jobID);
void child_handler(int sig);
void fg(char* jobMoveID);
void bg(char* jobID);
job* makeJob(int jobID, int pid, char* status, char* cmd, job* nextJob, int bg);
int pid = -1;


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


job* getJobFG(int jobID){
    printf("Cantaloupe\n");
    if(jobsFG.head != NULL && jobsFG.head->jobID == jobID){
        return jobsFG.head;
    } else {
        log_fg_notfound_error(jobID);
        return NULL;
    }
}

    
job* getJobBG(int jobID){
    printf("Lettuce\n");
    job* currentJob = jobsBG.head;
    if(currentJob != NULL){
        while(currentJob != NULL && currentJob->jobID != jobID){
            currentJob = currentJob->nextJob;
        }
        //return 2 when job not found
        if(currentJob == NULL){
            log_bg_notfound_error(jobID);
        }
    } else {
    //return 0 when job list is empty
        log_no_bg_error();
    }
    return currentJob;
}


void addJobFG(job* newJob){
    printf("Radish\n");
    if(jobsFG.head == NULL){
        jobsFG.head = newJob;
        jobsFG.tail = newJob;
        if(newJob->jobID < 0){
            newJob->jobID = 0;
        }
        jobsFG.numJobs++;
        jobsFG.numJobsRunning++;
    }
    return;
}

void addJobBG(job** newJob){
    printf("Limes\n");
    job* tempJob = jobsBG.head;
    job* beforeTemp = NULL;
    if(jobsBG.head != NULL && jobsBG.head->cmdline != NULL){
        //if its a new job and the list is not empty
        if((*newJob)->jobID < 1){
            jobsBG.tail->nextJob = *newJob;
            (*newJob)->jobID = (jobsBG.tail->jobID) + 1;
            jobsBG.tail = jobsBG.tail->nextJob;
        } else {
            //it is not a new job and list is not empty
            //even if head of list has equal jobID we insert after
            while(tempJob->jobID >= (*newJob)->jobID){
                beforeTemp = tempJob;
                tempJob = tempJob->nextJob;
            }
                beforeTemp->nextJob = *newJob;
                (*newJob)->nextJob = tempJob;
                if(tempJob == NULL){
                    jobsBG.tail = tempJob;
                }
        }
    } else {
        //list is empty
        jobsBG.head = *newJob;
        jobsBG.tail = *newJob;
        (*newJob)->jobID = 1;
    }
    jobsBG.numJobs++;
    jobsBG.numJobsRunning++;
    return;
}

int deleteJobFG(int jobID){
    printf("Job id of foreground head is %d\n", jobsFG.head->jobID);
    if(jobsFG.head != NULL && (jobsFG.head->jobID == jobID)){
        free(jobsFG.head->status);
        free(jobsFG.head->cmdline);
        free(jobsFG.head->nextJob);
        free(jobsFG.head);
        jobsFG.tail = NULL;
        return 0;
    } else {
        return 2;
    }
    jobsFG.numJobs--;
    jobsFG.numJobsRunning--;
}

int deleteJobBG(int jobID){  
    job* deleteJob = jobsBG.head;
    job* beforeDelete = jobsBG.head;

    if(deleteJob != NULL){
        //if job is head
        if(jobID == deleteJob->jobID){
            if(deleteJob->nextJob != NULL){             
                jobsBG.head = jobsBG.head->nextJob;
            } else {
                jobsBG.head = NULL;
                jobsBG.tail = NULL;
            }
            free(deleteJob->status);
            free(deleteJob->cmdline);
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

                //if job is intermediate node
                if(deleteJob->nextJob != NULL && deleteJob->nextJob->cmdline != NULL){
                    beforeDelete->nextJob = deleteJob->nextJob;
                } else if(deleteJob->processID == jobsBG.tail->processID){
                    jobsBG.tail = beforeDelete;
                    jobsBG.tail->nextJob = NULL;
                }
                
                free(deleteJob->status);
                free(deleteJob->cmdline);
                free(deleteJob);
        }

        jobsBG.numJobsRunning--;
        jobsBG.numJobs--;
        return 0;
    } else {
        printf("Empty job list!\n");
        return 2;
    }
}

void child_handler(int sig){
    int child_status;
    pid_t pidH = 0;
    while((pidH = waitpid(-1, &child_status, WNOHANG)) > 0){
        //printJobs();
        printf("pid is %d ", pidH);
        job* tempJob = getJob(pidH);
        if(tempJob != NULL){
            printf("job ID is %d job command is %s", tempJob->jobID, tempJob->cmdline);
        } else {
            printf("Strawbs\n");
        }
        if(sig == SIGINT){
            tempJob = getJobFG(tempJob->jobID);
            //kill(tempJob->processID, SIGTERM);
            printf("Hello\n");
        } else if (sig == SIGTSTP){
            tempJob = getJobFG(tempJob->jobID);
            addJobBG(&tempJob);
            deleteJobFG(tempJob->jobID);
            printf("Carrots\n");
            //kill(tempJob->processID, SIGSTOP);
        } else if(WIFCONTINUED(child_status)){
            tempJob->status = "Running";
            printf("Blueberries.\n");
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
            printf("Passion fruit\n");
            if(tempJob == NULL){
                printf("What is it?\n");
            }
            if(tempJob->bgIndicator == 0){
                log_job_fg_term(tempJob->processID, tempJob->cmdline);
                deleteJobFG(tempJob->jobID);
            } else {
                log_job_bg_term(tempJob->processID, tempJob->cmdline);
                deleteJobBG(tempJob->jobID);
            }
        } else if(WTERMSIG(child_status)){
            printf("Child status is %d\n", child_status);
            if(tempJob->bgIndicator == 0){
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
}

void fg(char* jobMoveID){
//move job in arg[1] to foreground
//if job ID arg is blank then get job id of bg job with highest id
        int jobID = 0;
        if(jobMoveID == NULL){
            printf("Melon\n");
            jobID = jobsBG.tail->jobID;
        } else {
            printf("Cucumber\n");
            jobID = atoi(jobMoveID);
        }

        //try to find the job in the background list
        job* jobBG = getJobBG(jobID);
        if(jobBG != NULL){
            jobBG->bgIndicator = 0;
            job* newJob = makeJob(jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, jobBG->nextJob, jobBG->bgIndicator);
            addJobFG(newJob);
            //addJobFG(jobBG);
            deleteJobBG(jobID);
            strcpy(newJob->status, "Running");
            //strcpy(jobBG->status, "Running");
            printf("process Id is %d job id is %d", jobBG->processID, jobBG->jobID);
            if(kill(jobBG->processID, SIGCONT) == -1){
                log_job_fg_fail(jobBG->processID, jobBG->cmdline);
                strcpy(newJob->status, "Stopped");
                //strcpy(jobBG->status, "Stopped");
            }

            printf("Star fruit\n");
            log_job_fg(jobBG->processID, jobBG->cmdline);
        } else {
            log_bg_notfound_error(jobID);
        }
    return;
}

void bg(char* jobID){
    int jobNum = 0;
    if(jobID != NULL){
        jobNum = atoi(jobID);
    }
    job* bgJob = getJobBG(jobNum);
    if(bgJob!= NULL){
        if(!strcmp(bgJob->status, "Running")){
            if(kill(bgJob->processID, SIGCONT) == -1){
                //should the process be stopped if signal fails to send?
                strcpy(bgJob->status, "Stopped");
                log_job_bg_fail(bgJob->processID, bgJob->cmdline);
            } else {
                strcpy(bgJob->status, "Running");
                log_job_bg(bgJob->processID, bgJob->cmdline); 
                (jobsBG.numJobsRunning)++;
            }
        } else {
            printf("Job already running.\n");
        }
    } else {
        log_bg_notfound_error(atoi(jobID));
    }
    return;
}

void printJobs(void){
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

    /**
    if(jobsFG.head != NULL){
        printf("FG jobs\n");
        log_job_details(jobsFG.head->jobID, jobsFG.head->processID, jobsFG.head->status, jobsFG.head->cmdline);
    }
    */
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
    sigset_t mask, prev_mask;
    job* newJob;
    struct sigaction sig_child;
    sig_child.sa_handler = child_handler; 
    sig_child.sa_flags = 0;
    sigaction(SIGCHLD, &sig_child, NULL);
    sigaction(SIGTSTP, &sig_child, NULL);
    sigaction(SIGINT, &sig_child, NULL);
    int child_status = NULL;
    int pid;


    while (1) {
        pid = 0;
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
	        while(token != NULL){
                token = strtok(NULL, " \n");
               // printf("Token is %s\n", token);
               // printf("args num is %d\n", argNum);
                if(token == NULL){
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
                job* temp = getJobFG(atoi(args[1]));
                waitpid(temp->processID, NULL, WNOHANG);
            } else if(strcmp(progName, "bg") == 0){
                bg(args[1]);
            } else if(strcmp(progName, "kill") == 0){
                //run kill ... signal to certain job
                printf("kill!\n");
                //how to remove first - off of first argument
                memmove(args[1], args[1]+1, strlen(args[1]));
                if(!kill(atoi(args[2]), atoi(args[1]))){
                    log_kill_error(atoi(args[2]), atoi(args[1]));
                }
            } else {
                newJob = makeJob(0,-1, "Running", cmdlineOriginal, NULL, bgIndicator); 
                //must block child signal before fork
                sigemptyset(&mask);
                sigaddset(&mask, SIGCHLD);
                sigprocmask(SIG_BLOCK, &mask, &prev_mask);
                //if parent process
                if(pid = fork()){
                    //update job list for child! 
                    newJob->processID = pid;
                    if(bgIndicator == 0){
                        addJobFG(newJob);
                    } else {
                        addJobBG(&newJob);
                    }
                    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
                    if(bgIndicator == 0){
                            do{
                                pid = waitpid(newJob->processID, &child_status, WUNTRACED);
                            } while(!WIFEXITED(child_status) && !WIFSIGNALED(child_status));
                    }
                } else {
                   setpgid(newJob->processID, 0);
                    if(bgIndicator == 1){
                        log_start_bg(newJob->processID, progName);
                    }
                    if(execv(progName, args) == -1){
                        log_command_error(progName);
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

