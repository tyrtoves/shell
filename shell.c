#include<stdio.h>
#include<unistd.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>

int MAXARG = 10;
int MAXSIZE = 100;

int main(int argc, char* argv[]) {
	char cur_dir[ PATH_MAX + 1 ];
	
	while (1) {
		char c;
		char* buf = malloc(MAXSIZE);
		int i = 0, j = 0;
		char* argv[MAXARG];
		int filedes[2];

		if (pipe(filedes) < 0) {
		    printf("PIPE");
		    exit(0);
		}
		for (i = 0; i < MAXARG; ++i){
			char* argv[i] = malloc(MAXSIZE);	
		}
		char* buf = malloc(MAXSIZE);
		char param_memory = 0;
		i = 0;
		while (1) {
			c = fgetc(stdin);
			if (c != '\n') {
				if (c != ' ') {
					buf[i++] = c;
					if (i == MAXSIZE - 1) {
						buf = realloc(buf, 2*MAXSIZE);
						param_memory = 1;
						MAXSIZE = MAXSIZE * 2;
					}
				}
				else {
					if (param_memory) {
						argv[j] = realloc(argv[j], 2*MAXSIZE);
					}
					buf[i] = '\0';
					argv[j] = buf;
					j++;
					param_memory = 0;
					i = 0;
				}
			} 
			else {
				break;
			}
		}
		if ( !strcmp(argv[0], "cd") ) {
			strncat( cur_dir, argv[0], PATH_MAX );
			strncat( cur_dir, "/", PATH_MAX );
			continue;
		}
		
		printf("%s", cur_dir);
		if (execvp(argv[0], argv) == -1) {
			perror("EXEC");
			return;
		}
		// распараллеливаем
		int PID = fork();	
		if (PID < 0) { 
			perror("FORK");
			exit(0);	
		}
		if (PID == 0){ // потомок выполняет
			close(filedes[0]);
			dup2(filedes[1], 1);
			if (execvp(argv[0], argv) == -1) {
				perror("EXEC");
				return;
			}
			close (filedes[1]);
			exit(0);
		}
		else { // родитель читает дальше
			close(filedes[1]);
		    int pos = 0;
		    char buf[500];
			while ( ( pos = read(filedes[0], buf, 500) ) != 0) {
				write(1, buf, pos);
			}
			continue;
		}	
	}
	return 0;
}
