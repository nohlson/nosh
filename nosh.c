#include <stdio.h>
#include <string.h>

#define NOSH_RL_BUFSIZE 1024
#define NOSH_TOK_BUFSIZE




char * nosh_read_line(void) {
	int bufsize = NOSH_RL_BUFSIZE;
	int i = 0;
	char * lineBuffer = (char *)malloc(bufsize * sizeof(char));
	int c;

	if (!buffer) {
		fprintf(stderr, "nosh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		//read char
		c = getchar();

		//if EOF replace with a null character
		if (c == EOF || c == '\n') {
			buffer[i] = '\0';
			return buffer;
		} else {
			buffer[i] = c;
		}
		i++;
		
		//realloc buffer if outside of buffer
		if (i >= bufsize) {
			bufsize = bufsize * 2;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "nosh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


//main loop of shell
void nosh_loop(void) {
	char * line;
	char ** args;
	int status;

	do {
		printf("<> ");
		line = nosh_read_line();
		args = nosh_split_line();
		status = nosh_execute(args);

		free(line);
		free(args);
	} while (status);


}




int main(int argc, char ** argv) {
	//load config files
	
	//run command loop
	nosh_loop();

	return EXIT_SUCCESS;
}
