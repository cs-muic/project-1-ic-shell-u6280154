/* ICCS227: Project 1: icsh
 * Name: Sumet Saelow
 * StudentID: 6280154
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX_CMD_BUFFER 255

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
		printf("bye \n");
		exit(arg[1]);
	}
	else{
		printf("Command not found");
	}
	return 1;
}

int main() {
    char *buffer;
    char **arg;
    int status = 0;
    do {
        printf("icsh $ ");
        buffer = read_line();
        arg = parse_line(buffer);
        status = execute(arg);
        free(buffer);
        free(arg);
    }while (status);
}
