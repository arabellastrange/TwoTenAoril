#include<stdio.h>
#include<string.h>

void split_string();

int main()
{
	split_string();
	return 0;
}

void split_string()
{
	// If this is passed in it has to be an array, it can't be a pointer.
	char command[] = "1:cd Desktop/Projects";

	char *history_number 	= strtok(command, ":");
	char *history_input 	= strtok(NULL, ":"); // The NULL tells strtok to continue where it left off.
	
	printf("%s\n", history_number);
	printf("%s\n", history_input);
}
