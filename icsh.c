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

int redirectDetect(char **arg,char* target){
	int i;
	for(i = 0;arg[i] != NULL;i++){
		if(!strcmp(arg[i],target)){
			return i;
		}
	return -1;
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
        inRedirect = redirectDetect(arg,'<');
        outRedirect = redirectDetect(arg,'>');
        appendRedirect = redirectDetect(arg,">>");
        printf("%d",appendRedirect);
        if(arg[0] == NULL){
        	return status;
        }
	else if(!strcmp(arg[0],"echo") &&     
	       ((inRedirect+outRedirect+appendRedirect) <= -3)){
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
		//process(arg);'
		signalGroup();
		int pid = fork();
		int in,out,append;
		if(pid < 0){
		perror("Fork failed");
		exit(errno);
	        }
	        else if(!pid){
		     if(inRedirect >= 0){
		     	in = open(arg[inRedirect + 1],O_RDONLY);
		     	dup2(in,0);
		     	close(in);
		         }
		   
	             if(outRedirect >= 0){
		   	out = open(arg[outRedirect + 1],O_TRUNC|O_CREAT|O_WRONLY,0666);
		   	dup2(out,1);
		   	close(out);
		 	}
		 
		     if(appendRedirect >= 0){
		        append = open(arg[appendRedirect + 1],O_RDWR|O_CREAT|O_WRONLY,0666);
		        dup2(append,1);
		        close(append);
		 
		      }

	        if(execvp(arg[0],arg) < 0){
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



void process(char** arg){
	signalGroup();
	int pid = fork();
	int in,out,append;
	int inRedirect = redirectDetect(arg,"<");
        int outRedirect = redirectDetect(arg,">");
        int appendRedirect = redirectDetect(arg,">>");
	    
	if(pid < 0){
		perror("Fork failed");
		exit(errno);
	}
	else if(!pid){
		if(inRedirect >= 0){
		   in = open(arg[inRedirect + 1],O_RDONLY);
		   dup2(in,0);
		   close(in);
		   }
		   
		 if(outRedirect >= 0){
		   out = open(arg[outRedirect + 1],O_TRUNC|O_CREAT|O_WRONLY,0666);
		   dup2(out,1);
		   close(out);
		 }
		 
		 if(appendRedirect >= 0){
		   append = open(arg[appendRedirect + 1],O_RDWR|O_CREAT|O_WRONLY,0666);
		   dup2(append,1);
		   close(append);
		 
		 }

	        if(execvp(arg[0],arg) < 0){
	        	exit(EXIT_FAILURE);
		   }
	}	
	else{
		waitpid(pid,NULL,0);
	    	}		
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
