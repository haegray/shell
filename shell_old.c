/* This is the only file you should update and submit. */

/* Fill in your Name and GNumber in the following two comment fields
 * Name: Helena Gray
 * GNumber: G01086877
 */

#include "logging.h"
#include "shell.h"

/* Feel free to define additional functions and update existing lines */
typedef struct job {
    int jobID;
    int processID;
    int status;
    char cmdline[MAXLINE];
}job;

typedef struct jobList {
    job* jobs;
    int head;
    int tail;
    int numJobs;
    int arrayLen;
}jobList;

int checkGaps(jobList** jobList){
    int j = 0;
    if((*jobList)->numJobs > 0){
        for(int i = 1; i < (*jobList)->tail; i++){
            j = i;
            if((*jobList)->jobs[i] == NULL && (i < (*jobList)->tail)){
                while((*jobList)->jobs[j] != NULL && (j < (*jobList)->tail)){
                    j++;
                }
                (*jobList)->jobs[i] = (*jobList)->jobs[j];
                (*jobList)->jobs[j] = NULL;
                if(j == (((*jobList)->tail) - 1)){
                    (*jobList)->tail = i;
                }
            }
        }
    }
}
void growArray(jobList** jobList){
    
    if((*jobList)->numJobs > (((*jobList)->arrayLen) * 0.75)){
        (*jobList)->jobs = realloc((*jobList)->jobs, (((*jobList)->arrayLen) * 2) * sizeof(job));
        (*jobList)->arrayLen = (*jobList)->arrayLen * 2;
    }
}

void shrinkArray(jobList** jobList){
    if((*jobList)->numJobs < (((*jobList)->arrayLen) * 0.25)){
        (*jobList)->jobs = realloc((*jobList)->jobs, (((*jobList)->arrayLen)/2) * sizeof(job));
        (*jobList)->arrayLen = (*jobList)->arrayLen / 2;
    }
}

int getJob(jobList* jobList, int jobID, job* fetchedJob){
    if(jobList->jobs[1] != NULL){
        for(int i = 1; i < jobList->tail; i++){
            if(jobList->jobs[i]->jobID == jobID){
                fetchedJob = jobList->jobs[i];
                return 1;
            }
        }
        return 2;
    }
    return 0;
}

int addJob(jobList* jobList, job* newJob, int bgIndicator){
    //check if foreground or background
    checkGaps(&jobList);
    if(bgIndicator == 1){
        if(tail >= jobList->arrayLen){
            growArray(&jobList);
        }
        if(jobList->jobs[(jobList->tail) - 1] != NULL && (((jobList->tail) - 1) != 0)){
            newJob->jobID = jobList->jobs[(jobList->tail) - 1]->jobID + 1;
        } else {
            newJob->jobID = jobList->tail;
        }
        jobList->jobs[tail] = newJob;
        jobList->tail = jobList->tail + 1; //last empty slot in array
    } else {
        //if foreground just check if there is already a foreground job
        //if not add to index zero of jobs table
        if(jobList->jobs[0] != NULL){
            printf("Job error!\n");
        } else {
            jobList->jobs[0] = newJob;
            newJob->jobID = 0;
        }
    }
    (jobList->numJobs)++;
}

int deleteJob(jobList* jobList, int jobPID){
    for(int i = 0; i < jobList->tail; i++){
            if(jobList->jobs[i]->processID == jobPID){
                //determine if job is already terminated or if it needs to be killed
                //if already terminated
                if(jobList->jobs[i]->status != 0){//this means if the job is terminated... put signal handler here
                    jobList->jobs[i] = NULL;
                }
                break;
            }
    }
    checkGaps(&jobList);
    shrinkArray(&jobList);
                //write function to shrink array if necessary
}

void child_handler(int sig){
    printf("Something!\n");

}

/* main */
/* The entry of your shell program */
int main() 
{
    char cmdline[MAXLINE]; /* Command line */
    char* token;
    char* progName;
    char* args[MAXARGS] = { NULL };
    int argNum = 0;
    int pid = 1;
    int statusPtr;
    int bgIndicator = 0; //foreground
    int jobGetSuccess;
    int jobID;

    jobList jobsMain = {.jobs = malloc(100 * sizeof(*array)),.head = 1, .tail = 1, .numJobs = 0, .arrayLen = 100};
    job newJob;
    while (1) {
        argNum = 0;
	    /* Print prompt */
  	    log_prompt();

	    /* Read a line */
	    // note: fgets will keep the ending '\'
	    if (fgets(cmdline, MAXLINE, stdin)==NULL)
	    {
	   	    if (errno == EINTR)
			    continue;
	    	    exit(-1); 
	    } else {
            progName = strtok(cmdline, " \n");
            token = progName;
            printf("Prog name is %s\n", progName);
	        while(token != NULL){
                token = strtok(NULL, " \n");
                printf("Token is %s\n", token);
                printf("args num is %d\n", argNum);
                if(token == NULL){
                    break;
                }
                args[argNum] = token;
                argNum++;
		    }
   
            if(strcmp(args[argsNum], "&")){
                bgIndicator = 1;
                args[argsNum] = NULL;
            }

            if(strcmp(progName, "help") == 0){
                log_help();
            } else if(strcmp(progName, "quit") == 0){
                log_quit();
                exit(0);
            } else if(strcmp(progName, "fg") == 0){
                //move job in argv[1] to foreground
                printf("fg!\n");
                if(args[1] == NULL){
                    jobID = jobsMain->jobs[(jobsMain->tail) - 1]->jobID;
                } else {
                    jobID = args[1];
                }

                jobGetSuccess = getJob(&jobsMain, jobID, &newJob);

                if(jobGetSuccess == 1){
                    newJob->status = 0; //switch job status to running
                    addJob(&jobsMain, &newJob, 0);
                    deleteJob(&jobsMain, &newJob, 1);
                } else if(jobGetSuccess == 0){
                    log_no_bg_error();
                } else {
                    log_fg_notfound_error(jobID);
                }
            } else if(strcmp(progName, "bg") == 0){
                //move job in argv[1] to background
                printf("bg!\n");
                newJob = getJob(&jobsMain, args[1]);
                addJob(&jobsMain, &newJob, 1);
                deleteJob(&jobsMain, &newJob, 0);
            } else if(strcmp(progName, "kill") == 0){
                //run kill ... signal to certain job
                printf("kill!\n");
            } else {

                if(pid = fork()){
                    //update job book for child! 
                    newJob = (job) {0, pid, 0, cmdline}; 
                    addJob(&jobsMain, &newJob, bgIndicator);
                    statusPtr = jobsMain->jobs[newJob->jobID]->status;
                    waitpid(pid, &statusPtr, WNOHANG|WUNTRACED);
                } else {
                    progName = args[0];
                    execv(progName, args);
                }  
                
     
            }   		
	    }

	    if (feof(stdin)) {
	        	exit(0);
	    }

	/* Parse command line and evaluate */
    } 
}
/* end main */
  

