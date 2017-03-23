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
void save_history_to_file();
void load_saved_history();
void save_alias_to_file(char *alias);
void read_alias_from_file(char *command);


// main function calls input method, saves and restores the user path
int main(){
	next_store=0;
	history_count=0;
	orgPATH = strdup(getenv("PATH"));
	PATH = strdup(orgPATH);
	chdir(getenv("HOME"));
	load_saved_history();
	fp = fopen(".hist_list", "w");
	if(!fp) {
		printf("Failed to open file.\n");
		my_exit(1);
	}
		//print_history();
		input();
		save_history_to_file();
		setenv("PATH", orgPATH, 1);
		printf("PATH : %s\n", getenv("PATH"));
 		fclose(fp);
		return 0;
}

//Gets a line and separates it into tokens which are saved in a pointer array
//It stops when the word 'exit' is given as input or when ctrl+D is pressed
void input(){
	char str[512],copy[512];

	//start programme by setting the defualt directory to the home directory
	
  	printf(">>");
	while(fgets(str, sizeof(str), stdin) != NULL){
		strcpy(copy,str);
		tokenize(str);
		if(words[0]!=NULL)
			if(strcmp(words[0],"exit") == 0)
				if(words[1] != NULL)
					printf("Exit doesn't take any parameters.\n");
				else
					break;
			else{
				store(strtok(copy,"\n"));
				runCommand();
			}
		//print();
   		printf(">>");
	}
}

//call the appropriate method depending on user input
void runCommand(){
	char aliasCommand[512];
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
		else if(strcmp(words[0],"history")==0)
			if(words[1]==NULL){
				print_history();
			}
			else {
				printf("history doesn't take any parameters.\n");
			}
		else if(strcmp(words[0], "alias") == 0){
			if(words[1] == NULL || words[2] == NULL){
				printf("Must have an alias and a command");			
			}
			else{
				strcpy(aliasCommand, words[1]);
				strcat(aliasCommand, ":");
				for(int i = 2; i < 50; i++){
					if(words[i] != NULL){
						strcat(aliasCommand,words[i]);
						strcat(aliasCommand," ");		
					}
				}

				save_alias_to_file(aliasCommand);
			}		
		}
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
		perror(command[0]);
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
	char copy[512];
	if(command[0]!='!'){
		if((atoi(command)-1<20))
			if(atoi(command)-1>=0)
				if(history_count>20)
				{
					strcpy(copy,history[(atoi(command)-1+history_count)%20].input_string);
					tokenize(copy);
					runCommand();
				}
				else if(atoi(command)-1<history_count)
				{
					strcpy(copy,history[atoi(command)-1].input_string);
					tokenize(copy);
					runCommand();
				}
				else {
					printf("The number is greater than the number of commands previously executed\n");
				}
			else{
				printf("There is no command %s\n",command);
			}
		else{
			printf("The number is greater than the number of commands previously executed\n");
		}
	}
	else if(strcmp(command,"!")==0 && command[1]=='\0')
		if(history_count>0){
		//	printf("%s\n",history[(history_count-1)%20].input_string);
			strcpy(copy,history[(history_count-1)%20].input_string);
			tokenize(copy);
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
	char copy[512];
	if(history_count-atoi(command)-1>0)
		if(command[0]>='0' && command[0]<='9'){
		//	printf("%s",history[(history_count-atoi(command)-1)%20].input_string);
			strcpy(copy,history[(history_count-atoi(command)-1)%20].input_string);
			tokenize(copy);
			runCommand();
		}
		else {
			printf("Invalid input passed.\n");
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
		strcpy(history[next_store%20].input_string,command);
		history[next_store%20].history_number=++history_count;
		next_store=(next_store+1)%20;
	}
}

//print saved history
void print_history(){
	if(history_count>20) 
        	for(int i = history_count%20; i< history_count;i++)
			if(history[i].input_string!=NULL)
				printf("%d %s\n",history[i%20].history_number-history[history_count%20].history_number+1, history[i%20].input_string);
			else{
                         }
	else
		for(int i=0;i<20 && i < history_count;i++){
			if(history[i].input_string!=NULL)
				if(history[i].history_number != 0)
					printf("%d %s\n",history[i].history_number,history[i].input_string);
				else
					printf("%d %s\n",20,history[i].input_string);
	}
}


void save_history_to_file() {
		for(int i=0;i<20 && i<history_count;i++)	
			fprintf(fp, "%d:%s\n", history[i].history_number, history[i].input_string);
				
}

void load_saved_history() {
	char command_from_file[512];
	fp = fopen(".hist_list", "r");
	if(!fp) {
		next_store = 0;
		history_count = 0;
	}
	else{
	while(fgets(command_from_file,sizeof command_from_file, fp)!=NULL){

		// Seprate the string into the two sections.
 		history[next_store%20].history_number = atoi(strtok(command_from_file, ":"));
		strcpy(history[next_store%20].input_string,strtok(NULL,":\n"));
		if(history[next_store%20].history_number==0 && history_count<20)
			history_count = 20;
		if(history[next_store%20].history_number>history_count)				
			history_count=history[next_store%20].history_number;
		next_store++;	
	}
	next_store = history_count%20;
	fclose(fp);
}
}

void save_alias_to_file(char *alias){
	FILE *aliasFile;

	aliasFile = fopen("alias.txt","w");
	fprintf(aliasFile, "%s\n",alias);

	fclose(aliasFile);
}

void read_alias_from_file(char *command){
	FILE *alias_file;
	char buff[512];

	alias_file=fopen("alias.txt","r");

	while(fgets(buff, sizeof buff, fp)!=NULL) {
		fscanf(alias_file, "%s", buff);

		char alias[100];
		char alias_command[512];

		strcpy(alias, strtok(alias, ":"));
		strcpy(command, strtok(NULL, ":"));

		//Alias has been found in the file.
		if(strcmp(command,alias) == 0){
			//save the command to the words array then tok it and run.
		}
	}

}
