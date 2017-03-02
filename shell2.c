#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

FILE *fp;
char *words[50];
char *orgPATH;
char *PATH;
int next_store,history_count;
struct command_history{
	char input_string[512];
	int history_number; //integer to track the number of the command
}history[20];
    	
// Function Declarations
void input();
void print();
void fork_execution(char *command[]);
void change_path(char *path);
void change_directory(char *directory);
void get_command(char* command);
void get_command_minus(char* command);
void my_exit(int flag);
void store(char* command);
void print_history();
void tokenize(char *line);
void runCommand();
void save_history_to_file(char *command); 
void load_saved_history();

// main function calls input method, saves and restores the user path
int main(){
	next_store=0;
	history_count=0;	
	load_saved_history();
	fp = fopen("prev_commands.txt", "a");
	//print_history(); 	
	orgPATH = strdup(getenv("PATH"));
	PATH = strdup(orgPATH);	
	input();
	setenv("PATH", orgPATH, 1);
	printf("PATH : %s\n", getenv("PATH"));	
 	fclose(fp);
	return 0;
}

//Gets a line and separates it into tokens which are saved in a pointer array
//It stops when the word 'exit' is given as input or when ctrl+D is pressed
void input(){
	char str[512];
	
	//start programme by setting the defualt directory to the home directory
	chdir(getenv("HOME"));
  	printf(">>");
	while(fgets(str, sizeof(str), stdin) != NULL){
		store(str);
		tokenize(str);
		if(words[0]!=NULL){
			if(strcmp(words[0],"exit") == 0){
				if(words[1] != NULL){
					printf("Exit doesn't take any parameters.\n");
				}
				else{
					break;
				}
			}
		else
			runCommand();			
		}
		//print();
   		printf(">>");
	}
}

//call the appropriate method depending on user input
void runCommand(){
	if(strcmp(words[0],"setpath") == 0){
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
 			if(words[1] != NULL && words[2] == NULL){
 				change_directory(words[1]);
 			}
 			else if(words[1] == NULL){
				change_directory("~"); 				
 			}
 			else{
 				printf("cd only takes zero or one parameter\n");
 			}
 		}
		else if(words[0][0] == '!'){
			if(words[0][1] != '\0' && words[0][1] != '-'){
				char parameter[2];
				strcpy(parameter,words[0]+1);
				get_command(parameter);				
			}
			else if(words[0][1] != '\0' && words[0][1] == '-' && words[0][2] != '\0'){
				char parameter[2];
				strcpy(parameter,words[0]+2);
				get_command_minus(parameter);
 			}
 			else{
 				printf("! needs an input \n");
 			}
		}
		else if(strcmp(words[0],"history")==0 && words[1] == NULL)
			print_history();
		else 
			fork_execution(words);
}

//break user input into tokens at the space (or other specified symbols)
void tokenize(char* line){
	const char s[] = "|><&; \t\n";
	char *token;
	int i = 0;
	token = strtok(line, s);
   	while( token != NULL ) {
      		words[i++] = strdup(token);
      		token = strtok(NULL, s);
   	}
	words[i] = NULL;
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
		chdir(getenv("HOME"));
	}
	else{
		if(chdir(directory)==-1)		
			perror(directory);
	}
}

//call command from history by number
void get_command(char* command){
	if(command[0]!='!'){
		if(atoi(command)-1<history_count){
			printf("%s",history[atoi(command)-1].input_string);
			tokenize(history[atoi(command)-1].input_string);
			runCommand();
		}
		else{
			printf("The number is greater than the number of commands previously executed\n");
		}
	}
	else if(strcmp(command,"!")==0 && command[1]=='\0')
		if(history_count>0){
			printf("%s",history[history_count-1].input_string);
			tokenize(history[history_count-1].input_string);
			runCommand();
		}
		else{
			printf("There's no previous command to be executed.\n");
		}
	else{
		printf("that is not a valid input for !\n");	
	}
}

//call command from histoy relative to current position
void get_command_minus(char* command){
	if(history_count-atoi(command)-1>=0){	
		printf("%s",history[history_count-atoi(command)-1].input_string);	
		tokenize(history[history_count-atoi(command)-1].input_string);
		runCommand();
	}
	else{
		printf("Number larger than the number of commands stored.\n");
	}
}

// restores the original path and exits
void my_exit(int flag){
	setenv("PATH", orgPATH, 1);
	exit(flag);
}

//store the user input in history
void store(char* command){
	if(command[0]!='!'){
		strcpy(history[next_store].input_string,command);
		history[next_store].history_number=history_count++;
		next_store=(next_store+1)%20;
		save_history_to_file(command);
	}
}

//print saved history
void print_history(){
	for(int i=0;i<history_count && i<20;i++){
		printf("%d %s",history[i].history_number+1,history[i].input_string);
	}
}

/* 	Questions: 		
		Does the file ever empty? 		
		Does it loop back to the start of the file and re write the items alrady stored. 	 Problem 		
		This needs to know how many items are in the array. 
*/ 
void save_history_to_file(char *command) { 	
		 		
		// For each item in the history, add it the the file. 		
		fprintf(fp, "%d:%s", history_count, command); 	
 
} 

/* 	
	Load the saved history in the file to the history array. 
	Problems 	
		Gettin the history number and the actual command seperated. 
*/ 
void load_saved_history() { 
	fp = fopen("prev_commands.txt", "r"); 	
	char command_from_file[512];	
	while(fgets(command_from_file,sizeof command_from_file, fp)!=NULL){	
		
		// Seprate the string into the two sections.
 		history[history_count].history_number = atoi(strtok(command_from_file, ":"));
		strcpy(history[history_count%20].input_string,strtok(NULL,":"));
		printf("%d:%s",history[history_count].history_number, history[history_count%20].input_string);
		history_count++;		
	}
	
	fclose(fp); 
}
