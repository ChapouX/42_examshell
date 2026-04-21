#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int	main(int ac, char **av)
{
	char	b[4096];
	char	*s;
	char	*t;
	int		n;
	int		r;
	int		i;
	int		j;
	int		l;

	if (ac != 2 || !av[1][0])
		return (1);
	s = 0;
	n = 0;
	l = strlen(av[1]);
	while ((r = read(0, b, 4096)) > 0)
	{
		t = realloc(s, n + r + 1);
		if (!t)
			return (perror("Error: "), free(s), 1);
		s = t;
		memmove(s + n, b, r);
		n += r;
	}
	if (r < 0)
		return (perror("Error: "), free(s), 1);
	if (!s)
		return (0);
	s[n] = 0;
	i = 0;
	while (s[i])
	{
		j = 0;
		while (av[1][j] && s[i + j] == av[1][j])
			j++;
		if (j == l)
		{
			while (j--)
				write(1, "*", 1);
			i += l;
		}
		else
			write(1, &s[i++], 1);
	}
	return (free(s), 0);
}
