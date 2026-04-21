#include <unistd.h>

int		n;
int		u[200];
char	s[200];
char	o[200];

void	p(int i)
{
	int	k;

	if (i == n)
	{
		write(1, o, n);
		write(1, "\n", 1);
		return ;
	}
	k = -1;
	while (++k < n)
	{
		if (!u[k])
		{
			u[k] = 1;
			o[i] = s[k];
			p(i + 1);
			u[k] = 0;
		}
	}
}

int	main(int ac, char **av)
{
	int		i;
	int		j;
	char	t;

	if (ac != 2 || !av[1][0])
		return (1);
	while (av[1][n])
	{
		if (n == 199)
			return (1);
		s[n] = av[1][n];
		n++;
	}
	i = -1;
	while (++i < n - 1)
	{
		j = i;
		while (++j < n)
			if (s[i] > s[j])
			{
				t = s[i];
				s[i] = s[j];
				s[j] = t;
			}
	}
	p(0);
	return (0);
}
