#include<stdio.h>
#include<string.h>

char *words[50];

void input();
void print();

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
	while(words[i]! = NULL){
		printf("\"%s\"\n",words[i++]);
	}
}

/*CD: We will need to load an external programme using the exec(function) 
presumbaly by loading them from a file along a predefined path?
The programme identifies the parameters in a given text and uses them in a method
If programme is invaild then return Error messages (how to spot bad programme??)*/
