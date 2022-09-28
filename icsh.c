/* ICCS227: Project 1: icsh
 * Name: Sumet Saelow
 * StudentID: 6280154
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#define MAX_CMD_BUFFER 255

char last_command[MAX_CMD_BUFFER] = "";
char last_line[MAX_CMD_BUFFER] = "\0";

char* read_line(){
	char* buffer = (char*) malloc(MAX_CMD_BUFFER*sizeof(char));
	fgets(buffer, 255, stdin);
	return buffer;
}

char** parse_line(char* line){
	char** parse_list = malloc(10*sizeof(char*));
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

int execute(char** arg){
	if(!strcmp(arg[0],"echo")){
		int i;
		for(i = 1;arg[i] != NULL;i++){
			printf("%s ",arg[i]);
		}
		printf("\n");
		return 1;
	}
	else if(!strcmp(arg[0],"exit")){
		if( *arg[1] >= '0' && *arg[1] <= '9'){
			printf("bye \n");
		        exit(*arg[1]);
		}
		else{
		   printf("Command not found \n");
		}
	}
	else if(!strcmp(arg[0],"!!")){
		if(strcmp(last_command,"!!")){
		    printf("%s",last_line);
		    int x = execute(parse_line(last_line));
		}
		else{
		    printf("Previously !!\n");
		}
	}
	else{
		printf("Command not found \n");
	}
	return 1;
}

void remember_me(char** arg,char* temp){
	strcpy(last_line,temp);
        strcpy(last_command,arg[0]);
}

int main() {
    char *buffer;
    char **arg;
    int status = 0;
    do {
        printf("icsh $ ");
        buffer = read_line();
        char* temp = (char*) malloc(strlen(buffer) + 1);
        strcpy(temp,buffer);
        arg = parse_line(buffer);
        status = execute(arg);
        
        remember_me(arg,temp);
      
        free(buffer);
        free(arg);
    }while (status);
}
