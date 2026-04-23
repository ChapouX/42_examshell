#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int	main(int ac, char **av)
{
	char	buf[4096];
	char	*all;
	int		tot;
	int		r;
	int		i;
	int		j;
	int		tl;

	if (ac != 2 || !av[1][0])
		return (1);
	tl = strlen(av[1]);
	all = NULL;
	tot = 0;
	while ((r = read(0, buf, 4096)) > 0)
	{
		all = realloc(all, tot + r + 1);
		if (!all)
			return (perror("Error: "), 1);
		memmove(all + tot, buf, r);
		tot += r;
	}
	if (r < 0)
		return (perror("Error: "), free(all), 1);
	if (!all)
		return (0);
	all[tot] = '\0';
	i = 0;
	while (all[i])
	{
		j = 0;
		while (av[1][j] && all[i + j] == av[1][j])
			j++;
		if (j == tl)
		{
			j = -1;
			while (++j < tl)
				write(1, "*", 1);
			i += tl;
		}
		else
			write(1, &all[i++], 1);
	}
	return (free(all), 0);
}