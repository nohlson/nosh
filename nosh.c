#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOSH_RL_BUFSIZE 1024
#define NOSH_TOK_BUFSIZE 54
#define NOSH_TOK_DELIM " \t\r\n\a"


//forward declarations of builtin shell commands:
int nosh_cd(char ** args);
int nosh_help(char ** args);
int nosh_exit(char ** args);


//buildtin commands


char * builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&nosh_cd,
	&nosh_help,
	&nosh_exit
};

int nosh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}


//implementations of builtins

int nosh_cd(char ** args) {
	if (args[1] == NULL) {
		fprintf(stderr, "nosh: specify directory\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("nosh");
		}
	}
	return 1;
}


int nosh_help(char ** args) {
	printf("Nosh v. 0.9\n");
	return 1;
}

int nosh_exit(char ** args) {
	return 0;
}


int nosh_execute(char ** args) {
	int i;
	if (args[0] == NULL) {
		//no command
		return 1;
	}

	for (i = 0; i < nosh_num_builtins(); i++) {
		if (!strcmp(args[0], builtin_str[i])) {
			return (*builtin_func[i])(args);
		}
	}

	return nosh_launch(args);
}

int nosh_launch(char ** args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		//child process
		if (execvp(args[0], args) == -1) {
			perror("nosh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		//error forking
		perror("nosh");
	} else {
		//parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}


char ** nosh_split_line(char * line){
	int bufsize = NOSH_TOK_BUFSIZE;
	char ** tokens = malloc(bufsize * sizeof(char*));
	char * token;
	int i = 0;

	if (!tokens) {
		fprintf(stderr, "nosh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, NOSH_TOK_DELIM);
	while (token != NULL) {
		tokens[i] = token;
		i++;

		if (i >= bufsize) {
			bufsize = bufsize * 2;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens) {
				fprintf(stderr, "nosh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, NOSH_TOK_DELIM);

	}
	tokens[i] = NULL;
	return tokens;
}




char * nosh_read_line(void) {
	int bufsize = NOSH_RL_BUFSIZE;
	int i = 0;
	char * buffer = (char *)malloc(bufsize * sizeof(char));
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
		args = nosh_split_line(line);

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
