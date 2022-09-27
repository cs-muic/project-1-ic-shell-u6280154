/* ICCS227: Project 1: icsh
 * Name: Sumet Saelow
 * StudentID: 6280154
 */

#include "stdio.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

char* read_line(){
	char* buffer = (char*) malloc(MAX_CMD_BUFFER*sizeof(char));
	fgets(buffer, 255, stdin) 
	return buffer;
}

int main() {
    char *buffer;
    while (1) {
        printf("icsh $ ");
        buffer = read_line();
        printf("you said: %s\n", buffer);
        free(buffer);
    }
}
