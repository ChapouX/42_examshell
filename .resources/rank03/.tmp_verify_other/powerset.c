#include <stdio.h>
#include <stdlib.h>

int	a[200];
int	b[200];
int	n;
int	t;

void	f(int i, int k, int s)
{
	int	j;

	if (i == n)
	{
		if (s == t)
		{
			j = 0;
			while (j < k)
			{
				if (j)
					printf(" ");
				printf("%d", b[j]);
				j++;
			}
			printf("\n");
		}
		return ;
	}
	f(i + 1, k, s);
	b[k] = a[i];
	f(i + 1, k + 1, s + a[i]);
}

int	main(int ac, char **av)
{
	int	i;

	if (ac < 3)
		return (1);
	t = atoi(av[1]);
	n = ac - 2;
	i = -1;
	while (++i < n)
		a[i] = atoi(av[i + 2]);
	f(0, 0, 0);
	return (0);
}
