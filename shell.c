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
		char param_output = 0;
		
		char output_file[MAXSIZE];
		i = 0;
		int argc = 0;
		while (1) { // читаем запрос
			c = fgetc(stdin);
			if (c != '\n') { //запроc закончился?
				if (c == '>') { // если вывод в файл -> отслеживаем отдельно
					c = fgetc(stdin);
					param_output = 1;
					while(1) {
						c = fgetc(stdin);
						if (c != ' ') {
							buf[i++] = c;
							if (i == MAXSIZE - 1) {
								buf = realloc(buf, 2*MAXSIZE);
								param_memory = 1;
								MAXSIZE = MAXSIZE * 2;
							}
						}
						else {
							buf[i] = '\0';
							if (output_file) 
								output_file = realloc(output_file, 2*MAXSIZE);
							strcpy(output_file, buf);
							i = 0;
							param_memory = 0;
							break;
						}
					}
					break;
				}
				else if (c != ' ') { // по аргументам разделяем
					buf[i++] = c;
					if (i == MAXSIZE - 1) {
						buf = realloc(buf, 2*MAXSIZE);
						param_memory = 1;
						MAXSIZE = MAXSIZE * 2;
					}
				}
				else { // заносим аргументы в массив
					if (param_memory) {
						argv[j] = realloc(argv[j], 2*MAXSIZE);
					}
					buf[i] = '\0';
					strcpy(argv[j], buf);
					j++;
					param_memory = 0;
					i = 0;
					count++;
				}
			} 
			else { // на следующую строку -> следующий аргумент
				break;
			}
		}
		if ( !strcmp(argv[0], "cd") ) { // отдельно отслеживаем переход в директорию
			strncat( cur_dir, argv[1], PATH_MAX );
			strncat( cur_dir, "/", PATH_MAX );
			ch_dir(cur_dir);
			continue;
		}
		printf("%s", cur_dir);
		if (strcmp(argv[argc], '&')) { // если запускаем в фоне
			
			if (param_output) {
				int fd = open(output_file, O_CREATE | O_TRUNC, 0777);
				dup2(fd, 1);
				if (execvp(argv[0], argv) == -1) { // запускаем процесс
					perror("EXEC");
					return;
				}
				close(fd);
			}
			else {
				if (execvp(argv[0], argv) == -1) { // запускаем процесс
					perror("EXEC");
					return;
				}
			}
		}
		else { // распараллеливаем если нужно
		
			int PID = fork();	
			if (PID < 0) { 
				perror("FORK");
				exit(0);	
			}
			if (PID == 0){ // потомок выполняет
				if (param_output) {
					int fd = open(output_file, O_CREATE | O_TRUNC, 0777);
					dup2(fd, 1);
					if (execvp(argv[0], argv) == -1) { // запускаем процесс
						perror("EXEC");
						return;
					}
				}
				else {
					if (execvp(argv[0], argv) == -1) { // запускаем процесс
						perror("EXEC");
						return;
					}
				}
				close(fd);
				exit(0);
			}
			else { // родитель читает дальше
				continue;
			}	
		}
	}
	return 0;
}
