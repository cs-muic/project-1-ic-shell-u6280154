/* ICCS227: Project 1: icsh
 * Name: Sumet Saelow
 * StudentID: 6280154
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"
#include "signal.h"

#define MAX_CMD_BUFFER 255

char last_command[MAX_CMD_BUFFER] = "";
char last_line[MAX_CMD_BUFFER] = "\0";

void INThandler(int signum){
	fflush(stdout);
}

void TSTPhandler(int signum){
	fflush(stdout);
}

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


char* read_line(){
	char* buffer = NULL;
	size_t buff_size = 0;
	getline(&buffer,&buff_size,stdin);
	return buffer;
}

char** parse_line(char* line){
	char** parse_list = malloc(sizeof(char*));
	char* temp;
	int i = 0;
	temp = strtok(line," \t\r\n");
	while(temp != NULL){
		parse_list[i] = temp;
		i++;
		temp = strtok(NULL," \t\r\n");
		}
	return parse_list;
	}
	
int excode = 0;


char** prev;

int execute(char** arg){
        int status = 1;   
        if(arg[0] == NULL){
        	return status;
        }
	else if(!strcmp(arg[0],"echo")){
	        if(!strcmp(arg[1],"$?")){
	        	printf("%d\n",excode);
	        }
	        else{
	            int i;
	            for(i = 1;arg[i] != NULL;i++){
		         printf("%s ",arg[i]);
		    }
		    printf("\n");
	  }
	}
	else if(!strcmp(arg[0],"exit")){
		if( *arg[1] >= '0' && *arg[1] <= '9'){
		        status = atoi(arg[1]);
		}
		else{
		   printf("Command not found \n");
		}
	}
	else if(!strcmp(arg[0],"!!")){
		if(strcmp(prev[0],"!!")){
		    int i = 0;
		    while(prev[i] != NULL){
		    	printf("%s ",prev[i]);
		    	i++; 
		    }
		    printf("\n");
		    int x = execute(prev);
		}
		else{
		    printf("Previously !!\n");
		}
	}
	
	else if(!strcmp(arg[0],"./icsh")){
		FILE* file = fopen(arg[1],"r");
		char *line;
		char **args;
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
	else{
	    int pid;
	    int status;
	    if((pid=fork()) < 0){
		perror("Fork failed");
		exit(errno);
	    	}
	    if(!pid){
	    

               
		if(execvp(arg[0],arg) < 0){
			exit(EXIT_FAILURE);
		}
	    }
	    if(pid){
		waitpid(pid,NULL,0);
	    }	
	 }
	 
	 if(memcmp(&prev,&arg,sizeof(arg)) != 0){
	 	prev = arg;
	 }
	 
	return status;
}

void remember_me(char** arg,char* temp){
	strcpy(last_line,temp);
        strcpy(last_command,arg[0]);
}


void handler_TSTP(int signum){
	printf("\nActivate");
	exit(signum);
}

int main() {
    char *buffer = NULL;
    char **arg;
    int status = 1;
    
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = INThandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
    
    struct sigaction sa1;
    sa1.sa_flags = 0;
    sa1.sa_sigaction = ChildHandler;
    sigemptyset(&sa1.sa_mask);
    sigaction(SIGCHLD,&sa1,NULL);
    
    struct sigaction sa2;
    sa2.sa_flags = SA_RESTART;
    sa2.sa_handler = TSTPhandler;
    sigemptyset(&sa2.sa_mask);
    sigaction(SIGTSTP,&sa2,NULL);
    
    
    do { 
        printf("icsh $ ");
        buffer = read_line();
        arg = parse_line(buffer);
        status = execute(arg);
    }while (status == 1);
    printf("bye \n");
    free(buffer);
    free(arg);
    exit(status);
}
