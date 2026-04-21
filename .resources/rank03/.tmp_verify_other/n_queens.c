#include <unistd.h>
#include <stdlib.h>

int	n;
int	p[100];

void	pn(int x)
{
	char	c;

	if (x > 9)
		pn(x / 10);
	c = x % 10 + '0';
	write(1, &c, 1);
}

int	ok(int c, int r)
{
	int	i;
	int	d;

	i = 0;
	while (i < c)
	{
		if (p[i] == r)
			return (0);
		d = p[i] - r;
		if (d < 0)
			d = -d;
		if (d == c - i)
			return (0);
		i++;
	}
	return (1);
}

void	go(int c)
{
	int	r;

	if (c == n)
	{
		r = 0;
		while (r < n)
		{
			pn(p[r]);
			write(1, r < n - 1 ? " " : "\n", 1);
			r++;
		}
		return ;
	}
	r = 0;
	while (r < n)
	{
		if (ok(c, r))
		{
			p[c] = r;
			go(c + 1);
		}
		r++;
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	n = atoi(av[1]);
	if (n < 1 || n > 100)
		return (0);
	go(0);
	return (0);
}
