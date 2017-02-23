#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

char *words[50];
char *orgPATH;
char *PATH;

// Function Declarations
void input();
void print();
void fork_execution(char *command[]);
void change_path(char *path);
void change_directory(char *directory);
void my_exit(int flag);

// main function calls input method, saves and restores the user path
int main(){
	orgPATH = strdup(getenv("PATH"));
	PATH = strdup(orgPATH);
	input();
	printf("PATH : %s\n", PATH);
	setenv("PATH", orgPATH, 1);
	return 0;
}

//Gets a line and separates it into tokens which are saved in a pointer array
//It stops when the word 'exit' is given as input or when ctrl+D is pressed
void input(){
	char str[512];
	const char s[] = "|><&; \t\n";
	char *token;
	int i;

	//start programme by setting the defualt directory to the home directory
	chdir(getenv("HOME"));

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
				}
				else{

					break;
				}
			}

			else if(strcmp(words[0],"setpath") == 0){
				if(words[1] != NULL && words[2]==NULL){
					change_path(words[1]);
				}
				else if(words[1] == NULL){
					printf("setpath needs a parameter.\n");
				}
				else{
					printf("setpath only takes one parameter \n");
				}
			}

			else if(strcmp(words[0],"getpath") == 0){
				if(words[1] == NULL){
					printf("PATH : %s\n",getenv("PATH"));
				}
				else{
					printf("getpath doesn't take a parameter.\n");
				}
			}

 			else if(strcmp(words[0], "cd") == 0){
 				if(words[1] != NULL && words[2]==NULL){
 					change_directory(words[1]);
 				}
 				else if(words[1] == NULL){
 					printf("cd needs a parameter\n");
 				}
 				else{
 					printf("cd only takes one parameter\n");
 				}
 			}

			else
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
		my_exit(-1);
	}
	else if (pid ==0) {
		execvp(command[0],command);
		perror (command[0]);
		my_exit(0);
	}
	else {
		wait(NULL);
	}
}

//takes in a path as input from user and sets the shell's path to that input
void change_path(char *path){
	setenv("PATH", path , 1);
	PATH=strdup(getenv("PATH"));
   	printf("PATH : %s\n", getenv("PATH"));
}

//takes in directory as input and sets the programme directory to it
void change_directory(char *directory){
	if(strcmp(directory, "~") == 0){
		//go home
		chdir(getenv("HOME"));
	}
	else{
		chdir(directory);
	}
}

// restores the original path and exits
void my_exit(int flag){
	setenv("PATH", orgPATH, 1);
	exit(flag);
}
