#include<stdio.h>
#include<string.h>

int main() 
{
	char alias[512];

	strcat(alias, "Hello");
	strcat(alias, ",");
	strcat(alias, "World");

	printf("%s\n", alias);

	return 0;
}
