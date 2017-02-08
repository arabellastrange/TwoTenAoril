#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

char *words[50];

void input();
void print();
void fork_execution(char *command[3]);

int main(){
	char *command[3];
	command[0] = strdup("/bin/ls");
	//command[1] = strdup("-l");	
	command[1] = NULL;
	fork_execution(command);
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
		if(strcmp(words[0],"exit") == 0){
			if(words[1] != NULL){
				printf("Exit doesn't take any parameters.\n");
				break;
			}
			else{
				break;
			}
		}
		print();
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

void fork_execution(char *command[3]){
	pid_t pid;

	pid = fork();

	if(pid<0) {
		fprintf(stderr, "Fork failed.");
		exit(-1);
	}
	else if (pid ==0) {
		if (execvp(command[0],command) == -1)
			perror ("Error description");
	}
	else {
		wait(NULL);
		printf("Child Complete\n");
	}
}
