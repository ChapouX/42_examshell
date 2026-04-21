#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int	main(int ac, char **av)
{
	char	buf[4096];
	char	*all;
	char	*tmp;
	size_t	tot;
	size_t	tl;
	ssize_t	r;
	size_t	i;
	size_t	j;

	if (ac != 2 || !av[1][0])
		return (1);
	all = NULL;
	tot = 0;
	tl = strlen(av[1]);
	while ((r = read(0, buf, sizeof(buf))) > 0)
	{
		tmp = realloc(all, tot + (size_t)r);
		if (!tmp)
			return (perror("Error: "), free(all), 1);
		all = tmp;
		memmove(all + tot, buf, (size_t)r);
		tot += (size_t)r;
	}
	if (r < 0)
		return (perror("Error: "), free(all), 1);
	i = 0;
	while (i < tot)
	{
		j = 0;
		while (j < tl && i + j < tot && all[i + j] == av[1][j])
			j++;
		if (j == tl)
		{
			j = 0;
			while (j++ < tl)
				write(1, "*", 1);
			i += tl;
		}
		else
			write(1, &all[i++], 1);
	}
	free(all);
	return (0);
}