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
pid_t pidH = 0;


//used in child handler
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
            currentJob = currentJob->nextJob;
            printf("La bla\n");
        }
    } 
    //printf("Current job command is %s", currentJob->cmdline);
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
        //while(currentJob != NULL){
            currentJob = currentJob->nextJob;
        }
    
        if(currentJob == NULL){
        //    log_bg_notfound_error(jobID);
                printf("WTf!\n");
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
        printf("New job id is %d\n", newJob->jobID);
        if(newJob->jobID < 0){
            printf("Darn\n");
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
            if(jobsBG.tail->nextJob == NULL){
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
            }
                beforeTemp->nextJob = *newJob;
                (*newJob)->nextJob = tempJob;
                if(tempJob == NULL){
                    jobsBG.tail = *newJob;
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

int deleteJobBG(int jobID){  
    job* deleteJob = jobsBG.head;
    job* beforeDelete = jobsBG.head;

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
            *deleteJob = NULL;
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
                *deleteJob = NULL;
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
    int child_status  = NULL;
    printf("Sig is %d\n", sig);
    printJobs();
    if(sig == SIGCHLD){
        printf("Tomatoes\n");
        pidH = waitpid(-1, &child_status, WUNTRACED|WCONTINUED);
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
        job* tempJob = getJob(pidH);
        if(WIFSTOPPED(child_status)){
            if(sig == SIGINT){
                tempJob = getJobFG(tempJob->jobID);
                kill(tempJob->processID, SIGTERM);
                printf("Hello\n");
            }    
            printf("Mangos\n");
            strcpy(tempJob->status, "Stopped");
            if(sig == SIGTSTP){
                tempJob = getJobFG(tempJob->jobID);
                addJobBG(&tempJob);
                deleteJobFG(tempJob->jobID);
            }
            if(tempJob->bgIndicator == 0){
                log_job_fg_stopped(tempJob->processID, tempJob->cmdline);
            } else {
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
            }
            if(tempJob->bgIndicator == 0){
                log_job_fg_term(tempJob->processID, tempJob->cmdline);
                deleteJobFG(tempJob->jobID);
                printf("Bubblegum\n");
            } else {
                log_job_bg_term(tempJob->processID, tempJob->cmdline);
                deleteJobBG(tempJob->jobID);
            }
        } else if(WIFSIGNALED(child_status)){
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
    printf("Corn\n");

}

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
    } else {
        printf("Cucumber\n");
        jobID = atoi(jobMoveID);
    }

    //try to find the job in the background list
    job* jobBG = getJobBG(jobID);
    if(jobBG != NULL){
        jobBG->bgIndicator = 0;
        printf("Job id is %d process id is %d status is %s cmdline is %s bdIndicator is %d\n", jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, jobBG->bgIndicator);
        job* newJob = makeJob(jobBG->jobID, jobBG->processID, jobBG->status, jobBG->cmdline, jobBG->nextJob, jobBG->bgIndicator);
        addJobFG(newJob);
        printf("Pickles\n");
        deleteJobBG(jobID);
        printf("Fries\n");
        printf("Job id is now %d and process id is %d\n", newJob->jobID, newJob->processID);
        //strcpy(newJob->status, "Running");
        if(jobBG == NULL){
            printf("Mustard\n");
        } else {
            printf("process Id is %d job id is %d job status is %s job cmd is %s\n", jobBG->processID, jobBG->jobID, jobBG->status, jobBG->cmdline);
        }
        if(strcmp(newJob->status, "Stopped") == 0){
            if(kill(newJob->processID, SIGCONT) == -1){
                //are we supposed to abort if this fails and reverse all our actions?
                log_job_fg_fail(newJob->processID, newJob->cmdline);
            }
        } 

        if(strcmp(newJob->status, "Running") == 0){
             sigprocmask(SIG_BLOCK, &mask, &prev_mask);
             printf("Star fruit\n");
            pidH = 0;
            while(!pidH){
                sigsuspend(&prev_mask);
                printf("PidH is %d", pidH);
            }
            printf("Green melon\n");
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            log_job_fg(jobBG->processID, jobBG->cmdline);
        }
    } else {
            printf("Blah!\n");
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
         printf("job id of bg head is %d", (jobsBG.head)->jobID);
        log_job_number(jobsBG.numJobsRunning);
        printf("Temp job command line is %s\n", tempJob->cmdline);
        printf("First char of command line is %c\n", *(tempJob->cmdline));
        //while(tempJob != NULL && tempJob->cmdline != NULL && *(tempJob->cmdline) != '\0'){
        while(tempJob != NULL){
            log_job_details(tempJob->jobID, tempJob->processID, tempJob->status, tempJob->cmdline);
            tempJob = tempJob->nextJob; 
        }
    }
    if(jobsFG.head != NULL){
        printf("FG jobs\n");
        log_job_details(jobsFG.head->jobID, jobsFG.head->processID, jobsFG.head->status, jobsFG.head->cmdline);
    }
    return;
}

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
 
/* main */
/* The entry of your shell program */


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

    sigset_t mask, prev_mask, block_mask;
    //sigemptyset(&block_mask);
    struct sigaction sig_child;
    sig_child.sa_handler = child_handler; 
    sig_child.sa_flags = 0;
    //sigaddset(&block_mask, SIGCHLD);
    //sigaddset(&block_mask, SIGTSTP);
    //sigaddset(&block_mask, SIGINT);
    //sigaddset(&block_mask, SIGINT);
    //sigaddset(&block_mask, SIGTSTP);
    //sigaddset(&block_mask, SIGCONT);
    //sigaddset(&block_mask, SIGTERM);
    //what other sigs to add to block
    sig_child.sa_mask = block_mask;
    sigaction(SIGCHLD, &sig_child, NULL);
    //sigaction(SIGTSTP, &sig_child, NULL);
    //sigaction(SIGINT, &sig_child, NULL);
    //sigaction(SIGTERM, &sig_child, NULL);
    //int child_status = NULL;
    int pid;
    job* newJob;


    while (1) {
        pid = 0;
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
                printf("Token is currently %s\n", token);
                printf("Arg num is %d", argNum); 
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
                    if((fdWrite = open(token, O_CREAT|O_WRONLY, 0600)) < 0){
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
                //job* temp = getJobFG(atoi(args[1]));
                //waitpid(temp->processID, NULL, WNOHANG);
            } else if(strcmp(progName, "bg") == 0){
                bg(args[1]);
            } else if(strcmp(progName, "kill") == 0){
                //run kill ... signal to certain job
                printf("kill!\n");
                //how to remove first - off of first argument
                memmove(args[1], args[1]+1, strlen(args[1]));
                int killVal = NULL;
                if(killVal = kill(atoi(args[2]), atoi(args[1])) == -1){
                    printf("Kill val is %d", killVal);
                    log_kill_error(atoi(args[2]), atoi(args[1]));
                }
            } else {
                newJob = makeJob(-1,-1, "Running", cmdlineOriginal, NULL, bgIndicator); 
                //must block child signal before fork
                sigemptyset(&mask);
                sigaddset(&mask, SIGCHLD);
                sigprocmask(SIG_BLOCK, &mask, &prev_mask);
                //if parent process
                if(pid = fork()){
                    //update job list for child! 
                    if(pid != -1){
                        if(bgIndicator == 1){
                            log_start_bg(newJob->processID, progName);
                        }
                    }
                    newJob->processID = pid;
                    if(bgIndicator == 0){
                        addJobFG(newJob);
                    } else {
                        addJobBG(&newJob);
                    }
                    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
                    if(bgIndicator == 0){
                        pid = 0;
                        while(!pid){
                            sigsuspend(&prev_mask);
                        }
                    }
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
                   setpgid(newJob->processID, 0);
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
                    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
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

