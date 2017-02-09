#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

char *words[50];


// Function Declarations
void input();
void print();
void fork_execution(char *command[]);

int main(){
	input();
	return 0;
}

//Gets a line and separates it into tokens which are saved in a pointer array
//It stops when the word 'exit' is given as input or when ctrl+D is pressed
void input(){
	char str[512];
	const char s[] = "|><&; \t\n";
	char *token;
	int i;
  	printf(">>");
	while(fgets(str, sizeof(str), stdin) != NULL){
		i = 0;
		token = strtok(str, s);
   		while( token != NULL ) {
      			words[i++] = strdup(token);
      			token = strtok(NULL, s);
   		}
		words[i] = NULL;
		if(words[0]!=NULL){		
			if(strcmp(words[0],"exit") == 0){
				if(words[1] != NULL){
					printf("Exit doesn't take any parameters.\n");
					break;
				}
				else{
					break;
				}
			} 
			fork_execution(words);
		}
		//print();
   		printf(">>");
	}
}


//Print the content of the pointer array
void print(){
	int i = 0;
	printf("The list of saved tokens is: \n");
	while(words[i]!= NULL){
		printf("\"%s\"\n",words[i++]);
	}
}

//Creates a new process and runs it
void fork_execution(char *command[]){
	pid_t pid;

	pid = fork();
	if(pid<0) {
		fprintf(stderr, "Fork failed.");
		exit(-1);
	}
	else if (pid ==0) {
		execvp(command[0],command);
		perror (command[0]);
		exit(0);
	}
	else {
		wait(NULL);
	}
}