#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static int	cleanup(int last_fd, int fd[2], int has_pipe)
{
	if (last_fd != -1)
		close(last_fd);
	if (has_pipe)
	{
		close(fd[0]);
		close(fd[1]);
	}
	while (wait(NULL) > 0)
		;
	return (1);
}

int	picoshell(char **cmds[])
{
	int		fd[2];
	int		last_fd;
	int		i;
	int		status;
	int		ret;
	pid_t	pid;

	last_fd = -1;
	i = 0;
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (cleanup(last_fd, fd, 0));
		pid = fork();
		if (pid == -1)
			return (cleanup(last_fd, fd, cmds[i + 1] != NULL));
		if (pid == 0)
		{
			if (last_fd != -1 && dup2(last_fd, 0) == -1)
				exit(1);
			if (cmds[i + 1] && dup2(fd[1], 1) == -1)
				exit(1);
			if (last_fd != -1)
				close(last_fd);
			if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (last_fd != -1)
			close(last_fd);
		if (cmds[i + 1])
		{
			close(fd[1]);
			last_fd = fd[0];
		}
		else
			last_fd = -1;
		i++;
	}
	ret = 0;
	while (wait(&status) > 0)
	{
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			ret = 1;
	}
	return (ret);
}