/* ICCS227: Project 1: icsh
 * Name: Sumet Saelow
 * StudentID: 6280154
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_CMD_BUFFER 255
#define MAXTOKENS 10
#define MAXLEN 512
#define UNDEF 0
#define FOREGROUND 1
#define BACKGROUND 2
#define STOPPED 3
#define DONE 4
#define MAXJOB 20

struct job{
	int pid;
	int job_id;
	int state;
	char** parsed_line;
};

int next_job = 1;

struct job jobs[MAXJOB];

void GeneralHandler(int signum){
	int status,id,pid;
	id = jobid_via_pid(jobs,pid);
	switch(signum){
		case 2:
			kill((jobs[id].pid),SIGINT);
                        jobs[id].state = DONE;
                        break;
                case 20:
                        kill((jobs[id].pid),SIGTSTP);
                        jobs[id].state = STOPPED;
                        printf("\n[%d]   ",jobs[id].job_id);
			printf("Stopped               ");
			modify_print(0,jobs[id].parsed_line,1);
                        break;
                default:
                        printf("ERROR");
	}
}


void initial_jobs(struct job *jobs){
	for(int i = 0; i < MAXJOB;i++){
		jobs[i].pid = 0;
		jobs[i].job_id = 0;
		jobs[i].state = UNDEF;
		jobs[i].parsed_line = NULL;
	}
}

int addjob(struct job *joblist,int pid,int state,char** arg){
	int i;
	int j;
	if(pid == 0){
		return 0;
	}
	for(i = 0;i < MAXJOB;i++){
		if(joblist[i].pid == 0){
			joblist[i].pid = pid;
			joblist[i].job_id = next_job++;
			if(next_job > MAXJOB){
				next_job = 1;
			}
			
			joblist[i].state = state;
			joblist[i].parsed_line = malloc(sizeof*arg);
			for(j = 0;arg[j] != NULL;j++){
				joblist[i].parsed_line[j] = strdup(arg[j]);
			}
			
			joblist[i].parsed_line[j] = NULL;
			
			if(joblist[i].state == BACKGROUND){
				printf("[%d] %d",joblist[i].job_id,joblist[i].pid);
			  
			}
			return i;
		}
	}
	printf("ERROR");
	return 0;
}

int max_jobid(struct job * joblist){
	 int max = 0;
	 for(int j = 0;j < MAXJOB;j++){
	        if(joblist[j].job_id > max){
	            max = joblist[j].job_id;
	        	}
	        }
	 return max;
}

int deletejob(struct job *joblist,int pid){
	if(pid < 1){
		return 0;
	}
	else{   
		for(int i = 0;i < MAXJOB;i++){
			if(joblist[i].pid == pid){
				joblist[i].pid = 0;
				joblist[i].job_id = 0;
				joblist[i].state = UNDEF;
				joblist[i].parsed_line = NULL;
				next_job = max_jobid(joblist) + 1;
				return 1;
			}
		}
	}
	return 0;
}

int jobid_via_pid(struct job * joblist,int pid){
	if(pid < 1){
		return 0;
	}
	for(int i = 0;i < MAXJOB;i++){
		if(joblist[i].pid == pid){
			return joblist[i].job_id;
		}
	}
	return 0;
}

bool iden_jobid(struct job * joblist,int job_id){
	if(job_id < 0){
		return false;
	}
	for(int i = 0;i < MAXJOB;i++){
		if(joblist[i].job_id == job_id){
			return true;
		}
	}
	return false;
}

void modify_print(int i,char** arg,int key){
	switch (key){
		case 1:
			for(int x = i;arg[x] != NULL;x++){
	 			printf("%s ",arg[x]);
			}
			printf("\n");
			break;
		case 2:
		        for(int x = i;strcmp(arg[x],"&");x++){
	 			printf("%s ",arg[x]);
			}
			printf("\n");
			break;
	        default:
	                printf("ERROR");	
	}
}

void ChildHandler(int sig){
        int status,id;
        int pid;
	while((pid = waitpid(-1,&status,WNOHANG)) > 0){
	        id = jobid_via_pid(jobs,pid) - 1;
	        if(jobs[id].state != FOREGROUND){
	        	printf("[%d]  Done                ",jobs[id].job_id);
	        	modify_print(0,jobs[id].parsed_line,2);
	        }
	        deletejob(jobs,pid);
	}
}

int Detection(char **arg,char* target){
	int i;
	for(i = 0;arg[i] != NULL;i++){
		if(!strcmp(arg[i],target)){
			return i;
		}
	}
	return -1;
}

char* read_line(){
	char* buffer = NULL;
	size_t buff_size = 0;
	getline(&buffer,&buff_size,stdin);
	return buffer;
}


char** parse_line(char* line){
	char** parse_list = malloc(MAXTOKENS*sizeof(char*));
	char* temp;
	int x = 0;
	temp = strtok(line," \t\r\n");
	
	while(temp != NULL){
		parse_list[x] = temp;
		x++;
		if(x >= MAXTOKENS - 1){
			break;
		}
		temp = strtok(NULL," \t\r\n");
		}
		
	parse_list[x] = NULL;
	return parse_list;
	}
	
int excode = 0;

void read_me(char** arg){
	FILE* file = fopen(arg[1],"r");
	char *line;
	const char **args;
	size_t len = 0;
	int stat = 1;
	ssize_t read;
	while(getline(&line,&len,file) != -1){
		args = parse_line(line);
		stat = execute(args);
	}
	excode = stat;
	fclose(file);
	free(line);
	free(args);
}

char** prev = NULL;

int execute(char** arg){
        int inRedirect,outRedirect,appendRedirect,background;
        inRedirect = Detection(arg,"<");
        outRedirect = Detection(arg,">");
        appendRedirect = Detection(arg,">>");
        background = Detection(arg,"&");
        if(arg[0] == NULL){
        	return 1;
        }
	else if(!strcmp(arg[0],"echo") && inRedirect < 0 && outRedirect < 0 && appendRedirect < 0){
	        if(!strcmp(arg[1],"$?")){
	        	printf("%d\n",excode);
	        }
	        else{
		    modify_print(1,arg,1);
	        }
	}
	else if(!strcmp(arg[0],"exit")){
		if(*arg[1] >= '0' && *arg[1] <= '255'){
		        return atoi(arg[1]);
		}
		else{
		   printf("Command not found \n");
		}
	}
	else if(!strcmp(arg[0],"!!") && prev != NULL){
		if(strcmp(prev[0],"!!")){
		    modify_print(0,prev,1);
		    int x = execute(prev);
		}
		else{
		    printf("Previously !!\n");
		    
		}
	}
	else if(!strcmp(arg[0],"./icsh")){
		read_me(arg);
	}
	else if(!strcmp(arg[0],"jobs")){
		listOfjob(jobs);
	}
	else if(!strcmp(arg[0],"fg")){
		int id,status;
		if(arg[1] == NULL){
			printf("ERROR");
		}
		else{
		        id = atoi(&arg[1][1]) - 1;
		        if(iden_jobid(jobs,id) != false){
		        	jobs[id].state = FOREGROUND;
		        	kill(jobs[id].pid,SIGCONT);
		        	modify_print(0,jobs[id].parsed_line,2);
		        	waitpid(jobs[id].pid,&status,WUNTRACED);
		        	jobs[id].state = DONE;
		        }
		        else{
		        	printf("ERROR");
		        }
		}
	}
	else{
	     signalGroup();
	     int status;
	     int pid = fork();
	     char **args;
	     int in,out,append,parse;
	     if(pid < 0){ //Error
		  perror("Fork failed");
		  exit(errno);
	     }
	     else if(!pid){//Child process
		  args = malloc(sizeof*arg);
	          parse = 0;
		  for(int i = 0;arg[i] != NULL;i++){
			  if(!strcmp(arg[i],"<")){
			      ++i;
			      in = open(arg[i],O_RDONLY);
			      dup2(in,0);
			      close(in);
			      continue;
		           }
		           if(!strcmp(arg[i],">")){
			      ++i;
			      out = open(arg[i],O_TRUNC|O_CREAT|O_WRONLY,0666);
			      dup2(out,1);
			      close(out);
			      continue;
		           }
		           if(!strcmp(arg[i],">>")){
			      ++i;
			      append = open(arg[i],O_CREAT|O_RDWR|O_APPEND,0666);
			      dup2(append,1);
			      close(append);
			      continue;
		            }
		           args[parse++] = strdup(arg[i]);
                      }
                      args[parse] = NULL;
                      
                      if(background >= 0){
                      	args[background] = NULL;
                      
                      }
                      if(execvp(args[0],args) < 0){
        	           exit(EXIT_FAILURE);
                      }
             }	
	     else{//Parent process
	     	if(background < 0){
	     		int i = addjob(jobs,pid,FOREGROUND,arg);
	     		setpgid(pid,pid);
	     		signal(SIGTTOU,SIG_IGN);
	     		tcsetpgrp(STDIN_FILENO,pid);
	     		waitpid(pid,&status,WUNTRACED);
	     		tcsetpgrp(STDIN_FILENO,getpid());
	     		
	     		if(WIFEXITED(status)){
	     			jobs[i].state = DONE;
                         }
                         else if(WIFSIGNALED(status)){
                         	jobs[i].state = DONE;
                         }
                         else if(WSTOPSIG(status)){
                         	jobs[i].state = STOPPED;
                         }
	     	}
	     	else{
	     		int i = addjob(jobs,pid,BACKGROUND,arg);
	     		printf("\n");
	     	    }
	    	}		
	}	
	if(memcmp(&prev,&arg,sizeof(arg)) != 0){
	 	 save_me(arg);
	}
	return 1;
}

void save_me(char** arg){
	int i;
	free(prev);
	prev = malloc(MAXTOKENS*sizeof*arg);
	for(i = 0;arg[i] != NULL;i++){
	 	prev[i] = strdup(arg[i]);
	 	}
	prev[i] = NULL;
}

void listOfjob(struct job *joblist){
	for(int i = 0;i < MAXJOB;i++){
		if(joblist[i].pid != 0){
			if(joblist[i].state == DONE){
				deletejob(joblist,joblist[i].pid);
				continue;
			}
			else if(joblist[i].state == FOREGROUND){
			        printf("[%d]   ",joblist[i].job_id);
				printf("Foreground            ");
			}
			else if(joblist[i].state == BACKGROUND){
			        printf("[%d]   ",joblist[i].job_id);
				printf("Running               ");            
			}
			else if(joblist[i].state == STOPPED){
			        printf("[%d]   ",joblist[i].job_id);
				printf("Stopped               ");
			}
		modify_print(0,joblist[i].parsed_line,1);
		}
	}
}

void signalGroup(){
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = GeneralHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
    
    struct sigaction sa2;
    sa2.sa_flags = SA_RESTART;
    sa2.sa_handler = GeneralHandler;
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGTSTP,&sa2,NULL);
}

int main() {
    char *buffer;
    char *prompt;
    char** arg;
    int status = 1;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_sigaction = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
    
    struct sigaction sa2;
    sa2.sa_flags = 0;
    sa2.sa_sigaction = SIG_IGN;
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGTSTP,&sa2,NULL);
    
    struct sigaction sa1;
    sa1.sa_flags = SA_RESTART;
    sa1.sa_sigaction = ChildHandler;
    sigemptyset(&sa1.sa_mask);
    sigaction(SIGCHLD,&sa1,NULL);
    
    initial_jobs(jobs);
    do { 
        printf("icsh $ ");
        buffer = read_line();
        arg = parse_line(buffer);
        status = execute(arg);
        free(arg);
        free(buffer);
    }while (status == 1);
    
    printf("bye \n");
    exit(status);
}
