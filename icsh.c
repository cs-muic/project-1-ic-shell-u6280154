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
#define FORE 1
#define BACK 2
#define STOP 3
#define DONE 4
#define MAXJOB 20

char last_command[MAX_CMD_BUFFER] = "";
char last_line[MAX_CMD_BUFFER] = "\0";

void INThandler(int signum){
        printf("");
}

void TSTPhandler(int signum){
        printf("");
}

struct job{
	int pid;
	int job_id;
	int state;
	char line[MAXLEN];
}

int next_job = 1;

struct job jobs[MAXJOB];





void ChildHandler(int sig,siginfo_t *sip,void *notused){
        int status;
	fflush(stdout);
	status = 0;
	if(sip->si_pid == waitpid(sip->si_pid,&status,WNOHANG)){
		if(WIFEXITED(status)||WTERMSIG(status))
			printf("\n");
		
		else
			printf("\n");
		
	}
	else{
		printf("\n");
	}
}

int redirectDetect1(char **arg){
	int i;
	for(i = 0;arg[i] != NULL;i++){
		if(!strcmp(arg[i],"<")){
			return i;
		}
	}
	return -1;
}

int redirectDetect2(char **arg){
	int i;
	for(i = 0;arg[i] != NULL;i++){
		if(!strcmp(arg[i],">")){
			return i;
		}
	}
	return -1;
}

int redirectDetect3(char **arg){
	int i;
	for(i = 0;arg[i] != NULL;i++){
		if(!strcmp(arg[i],">>")){
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

char** prev;

void modify_print(int i,char** arg){
         int x;
	 for(x = i;arg[x] != NULL;x++){
	 	printf("%s ",arg[x]);
	}
	printf("\n");
}

int execute(char** arg){
        int status = 1;
        int inRedirect,outRedirect,appendRedirect;
        inRedirect = redirectDetect1(arg);
        outRedirect = redirectDetect2(arg);
        appendRedirect = redirectDetect3(arg);
        if(arg[0] == NULL){
        	return status;
        }
	else if(!strcmp(arg[0],"echo") && inRedirect < 0 && outRedirect < 0 && appendRedirect < 0){
	        if(!strcmp(arg[1],"$?")){
	        	printf("%d\n",excode);
	        }
	        else{
		    modify_print(1,arg);
	        }
	}
	else if(!strcmp(arg[0],"exit")){
		if(*arg[1] >= '0' && *arg[1] <= '255'){
		        status = atoi(arg[1]);
		}
		else{
		   printf("Command not found \n");
		}
	}
	else if(!strcmp(arg[0],"!!")){
		if(strcmp(prev[0],"!!")){
		    modify_print(0,prev);
		    int x = execute(prev);
		}
		else{
		    printf("Previously !!\n");
		}
	}
	else if(!strcmp(arg[0],"./icsh")){
		read_me(arg);
	}
	else{
	     signalGroup();
	     int pid = fork();
	     char **args;
	     int in,out,append,parse;
	     if(pid < 0){
		  perror("Fork failed");
		  exit(errno);
	     }
	     else if(!pid){
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
                      if(execvp(args[0],args) < 0){
        	           exit(EXIT_FAILURE);
                      }
             }	
	     else{
		   waitpid(pid,NULL,0);
	    	}		
	}	
	
	if(memcmp(&prev,&arg,sizeof(arg)) != 0){
	 	 save_me(arg);
	}
	 	
	return status;
}



void save_me(char** arg){
	int i;
	prev = malloc(i*sizeof*arg);
	for(i = 0;arg[i] != NULL;i++){
	 	prev[i] = strdup(arg[i]);
	 	}
	prev[i] = NULL;
}

void signalGroup(){
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = INThandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
    
    struct sigaction sa2;
    sa2.sa_flags = SA_RESTART;
    sa2.sa_handler = TSTPhandler;
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGTSTP,&sa2,NULL);
}

int main() {
    char *buffer;
    char *prompt;
    char** arg;
    int status = 1;
    
    struct sigaction sa1;
    sa1.sa_flags = 0;
    sa1.sa_sigaction = ChildHandler;
    sigemptyset(&sa1.sa_mask);
    sigaction(SIGCHLD,&sa1,NULL);
    
    do { 
        signal(SIGINT,SIG_IGN);
        signal(SIGTSTP,SIG_IGN);
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
