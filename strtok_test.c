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
	char command[] = "1:cd Desktop/Projects";
	char *history_number 	= strtok(command, ":");
	char *history_input 	= strtok(NULL, ":");
	
	printf("%s\n", history_number);
	printf("%s\n", history_input);
}
