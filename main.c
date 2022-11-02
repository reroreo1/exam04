#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct s_cmd{
	int fd[2];
	int tmp_fd;
	char *argv[1024];
	int index;
	bool prevp;
	bool nextp;
	bool end;
} t_cmd;

t_cmd cmd;

int ft_exec() {

	pipe(cmd.fd);

	int pid = fork();
	if (pid < 0) {
		printf("error fork\n");
	} else if (pid == 0) {

		if (cmd.prevp) {
			// prev
			dup2(cmd.tmp_fd, 0);
			close(cmd.tmp_fd);
		} else if (cmd.nextp) {
			dup2(cmd.fd[1], 1);
			close(cmd.fd[1]);
		} else {
			close(cmd.fd[1]);
			close(cmd.fd[0]);
		}
		// execve
		execve(cmd.argv[0], cmd.argv, 0x00);
	}
	// save read end of pipe to fd_tmp;
	cmd.tmp_fd = dup(cmd.fd[0]);
	close(cmd.fd[0]);
	close(cmd.fd[1]);
	// close pipe ends
	return pid;
}


bool check_arg(char *str) {

	if (cmd.nextp == 1) {
		cmd.nextp = 0;
		cmd.prevp = 1;
	}

	if (strcmp(str, "|") == 0) {
		cmd.nextp = 1;
		return 1;
	}
	else if (strcmp(str, ";") == 0) {

		cmd.end = 1;
		return 1;
	}
	cmd.argv[cmd.index++] = str; // need to reset after exec
	cmd.argv[cmd.index] = 0x00;

	return 0;
}


int main(int argc, char **av){

	cmd.fd[0] = cmd.fd[1] = 0;
	cmd.tmp_fd = 0;
	cmd.prevp = cmd.nextp = cmd.end = 0;
	cmd.index = 0;
	
	int pid = -1;
	int i = 1;
	while(av[i]){

		if (check_arg(av[i]) || av[i + 1] == NULL) {
			// check if cd 
			pid = ft_exec();
			cmd.index = 0;
		}

		if (cmd.end == 1) {
			
			// wait for childs
			waitpid(pid, 0x00, 0);
			while (waitpid(-1 , 0x00, 0) != -1);
			// reset
			cmd.prevp = 0;
			cmd.nextp = 0;
			cmd.end = 0;
		}
		i++;
	}
	return 0;
}