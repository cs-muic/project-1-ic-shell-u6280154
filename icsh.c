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

#define MAX_CMD_BUFFER 255
#define MAXTOKENS 10

char last_command[MAX_CMD_BUFFER] = "";
char last_line[MAX_CMD_BUFFER] = "\0";

void INThandler(int signum){
        printf("");
}

void TSTPhandler(int signum){
        printf("");
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
	else{
	    signalGroup();
	    int pid;
	    int status;
	    if((pid=fork()) < 0){
		perror("Fork failed");
		exit(errno);
	    	}
	    else if(!pid){
		if(execvp(arg[0],arg) < 0){
			exit(EXIT_FAILURE);
		}
	    }
	    else if(pid){
		waitpid(pid,NULL,0);
	    }	
	 }
	 
	 if(memcmp(&prev,&arg,sizeof(arg)) != 0){
	 	int i;
	 	prev = malloc(i*sizeof*arg);
	 	for(i = 0;arg[i] != NULL;i++){
	 		prev[i] = strdup(arg[i]);
	 	}
	 	prev[i] = NULL;
	 }
	  
	return status;
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
