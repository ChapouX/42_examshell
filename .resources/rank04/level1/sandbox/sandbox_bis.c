#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

static void	on_alarm(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	pid_t			pid;
	int				status;

	if (!f)
		return (-1);
	sa.sa_handler = on_alarm;
	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) == -1)
		return (-1);
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno == EINTR)
		{
			alarm(0);
			if (kill(pid, SIGKILL) == -1 && errno != ESRCH)
				return (-1);
			if (waitpid(pid, NULL, 0) == -1 && errno != ECHILD)
				return (-1);
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0)
		{
			if (verbose)
				printf("Nice function!\n");
			return (1);
		}
		if (verbose)
			printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
		return (0);
	}
	if (WIFSIGNALED(status))
	{
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);
}