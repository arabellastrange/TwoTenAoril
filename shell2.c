#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

FILE *fp;
FILE *aliasFile;
char *words[50];
char *orgPATH;
char *PATH;
int next_store,history_count, alias_number;
struct command_history{
	char input_string[512];
	int history_number; //integer to track the number of the command
}history[20];

struct saved_aliases {
	char alias[512];
	char actual_command[512];
} aliases[10];

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
void load_aliases();
void save_alias_to_file();
int isAlias(char *alias);
void remove_alias(char *alias);
void print_aliases();


// main function calls input method, saves and restores the user path, opens files for history and aliases for writing 
int main(){
	alias_number = 0;
	next_store=0;
	history_count=0;
	orgPATH = strdup(getenv("PATH"));
	PATH = strdup(orgPATH);
	chdir(getenv("HOME"));
	load_saved_history();
	load_aliases();
	fp = fopen(".hist_list", "w");
	if(!fp) {
		printf("Failed to open file.\n");
		my_exit(1);
	}
	aliasFile = fopen(".aliases","w");
	if(!aliasFile){
		printf("Failed to open file.\n");
		my_exit(1);
	}	
	
	input();

	setenv("PATH", orgPATH, 1);
	printf("PATH : %s\n", getenv("PATH"));
	save_history_to_file();
	save_alias_to_file();
	fclose(aliasFile);	
	fclose(fp);
	return 0;
}

//Gets a line, calls the tokenize and the run function as long as the input is different from 'exit' or when ctrl+D is pressed
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
   		printf(">>");
	}
}

//call the appropriate method depending on user input
void runCommand(){
	char aliasCommand[512],copy[512];
	if(isAlias(words[0]) == 1)
			for(int i=0;i<alias_number;i++)
				if(strcmp(words[0],aliases[i].alias)==0) {
					int word_count;			
					for(word_count=0;word_count<50;word_count++)
						if(words[word_count]==NULL)
							break;						
					const char s[] = "|><&; \t\n";
					char *token;
					int k=0;
					strcpy(copy,aliases[i].actual_command);					
					token = strtok(copy, s);
					for(int i=0;i<word_count-1;i++)
						strcpy(words[i],words[i+1]);
					word_count --;		
   					while( token != NULL ) {
						for(int j = word_count;j>k;j--)
						words[j]=strdup(words[j-1]);
      						words[k] = strdup(token);
						k++;
						word_count++;
      						token = strtok(NULL, s);
   					}
					words[word_count] = NULL;
				}
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
	else if(strcmp(words[0],"history")==0){
		if(words[1]==NULL){
			print_history();
		}
		else {
			printf("history doesn't take any parameters.\n");
		}
	}
	else if(strcmp(words[0], "alias") == 0){
		if(words[1] == NULL){		
			print_aliases();
		}
		else if(words[1]!=NULL && words[2]==NULL) {
			printf("Can't create the alias; command corresponding to alias not given.\n");
		}  
		else if(alias_number<10 || isAlias(words[1])){
				strcpy(aliasCommand, words[2]);
				if(words[3]!=NULL)
				strcat(aliasCommand, " ");
				for(int i = 3; i < 49; i++){
					if(words[i] != NULL){
						strcat(aliasCommand,words[i]);
						if(words[i+1]!=NULL)
							strcat(aliasCommand," ");		
					}
				}
				if(isAlias(words[1])==0){
					strcpy(aliases[alias_number].alias,words[1]);
					strcpy(aliases[alias_number++].actual_command,aliasCommand);
					printf("The alias saved\n");
				}
				else {
					for(int i=0;i<alias_number;i++)
						if(strcmp(aliases[i].alias,words[1])==0)
						{
							printf("The command \"%s\" of the alias \"%s\" has been replaced with the command \"%s\"\n", aliases[i].actual_command,aliases[i].alias,aliasCommand);
						strcpy(aliases[i].actual_command, aliasCommand);						
						}
				}			
			} 
			else {
				printf("Can't save any more aliases.\n");
			}			
	}	
	else if(strcmp(words[0], "unalias") == 0)
		if(alias_number>0) {
		if(words[1]!=NULL)		
			if(words[2]==NULL) {
				remove_alias(words[1]);
			}			
			else {
				printf("Invalid arguments for unalias; only one argument accepted\n");
			}		
		else {
			printf("Unalias command expects one argument.\n");
		}
		}
		else{
			printf("Alias list is empty.\n");
		}
	else {
		fork_execution(words);
	}
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
	int ok = 1;
	for(int i=0;i<strlen(command) && ok==1;i++)
		if(!(command[i]>='0' && command[i]<='9'))
			ok = 0;
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
				else if(ok==1){
					printf("The number is greater than the number of commands previously executed\n");
				}
					else {
						printf("There is no command %s\n",command);
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
	int ok = 1;
	for(int i=0;i<strlen(command) && ok==1;i++)
		if(!(command[i]>='0' && command[i]<='9'))
			ok = 0;
	if(ok==1)
		if(history_count-atoi(command)-1>=0 && atoi(command)<20){
			strcpy(copy,history[(history_count-atoi(command)-1)%20].input_string);
			tokenize(copy);
			runCommand();
		}
		else {
			printf("Number larger than the number of commands stored.\n");
		}
	else{
			printf("Invalid input passed.\n");
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

//saves the content of history struct in a file
void save_history_to_file() {
		for(int i=0;i<20 && i<history_count;i++)	
			fprintf(fp, "%d:%s\n", history[i].history_number, history[i].input_string);
				
}

//load the contents from .hist_list file into the history structure 
void load_saved_history() {
	char command_from_file[512];
	fp = fopen(".hist_list", "r");
	if(!fp) {
		next_store = 0;
		history_count = 0;
	}
	else{
		while(fgets(command_from_file,sizeof command_from_file, fp)!=NULL) {
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

//loads the contents of .aliases file in the aliases structure
void load_aliases(){
	char alias_line[512];
	fp = fopen(".aliases", "r");
	if(!fp) {
		alias_number = 0;
	}
	else{
		while(fgets(alias_line,sizeof alias_line, fp)!=NULL) {
			strcpy(aliases[alias_number].alias,strtok(alias_line,":"));
			strcpy(aliases[alias_number++].actual_command,strtok(NULL,":\n"));			
		}
		fclose(fp);
	}
}

//saves the contents of the alias structure in a file
void save_alias_to_file(){
	for(int i=0;i<alias_number;i++)
		fprintf(aliasFile, "%s:%s\n", aliases[i].alias,aliases[i].actual_command);

}

//checks if a string is an alias or not
int isAlias(char *alias) {
	for (int i=0;i<alias_number;i++)
		if(strcmp(aliases[i].alias,alias)==0)
			return 1;
	return 0;
}

//removes an alias from the list of aliases
void remove_alias(char *alias) {
	int alias_index = 0;

	if(isAlias(alias) == 1) {
		for(int i = 0;i<alias_number;i++)
			if(strcmp(aliases[i].alias,alias)==0){
				alias_index = i;
				break;
			}
		for(int i = alias_index;i<alias_number-1;i++){
			strcpy(aliases[i].alias,aliases[i+1].alias);
			strcpy(aliases[i].actual_command,aliases[i+1].actual_command);
		}
		alias_number--;
		printf("Alias successfully removed.\n");
	} else {
		printf("Not a valid alias\n");
	}
}

//prints the aliases which are already stored
void print_aliases() {
	if(alias_number==0)
		printf("There are no saved aliases.\n");
		for(int i=0;i<alias_number;i++)
			printf("%d %s:%s\n",i+1,aliases[i].alias,aliases[i].actual_command);
	}
