#include <stdio.h>

int main() {
   	FILE *fp;

   	fp = fopen("test_commands.txt", "w+");
   	fprintf(fp, "This is testing for fprintf...\n");
   	fclose(fp);

	return 0;
}
