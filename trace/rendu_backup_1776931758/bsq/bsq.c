#include "bsq.h"

static void	free_grid(char **g, int n)
{
	int	i;

	i = -1;
	if (g)
	{
		while (++i < n)
			free(g[i]);
		free(g);
	}
}

static int	load(FILE *f, t_map *m)
{
	char	*line;
	size_t	len;
	int		r;
	int		i;

	line = NULL;
	len = 0;
	if (fscanf(f, "%d %c %c %c", &m->h, &m->empty, &m->obs, &m->full) != 4
		|| m->h <= 0 || m->empty == m->obs || m->empty == m->full
		|| m->obs == m->full)
		return (-1);
	m->grid = malloc((m->h + 1) * sizeof(char *));
	if (!m->grid)
		return (-1);
	m->grid[m->h] = NULL;
	getline(&line, &len, f);
	i = -1;
	while (++i < m->h)
	{
		r = getline(&line, &len, f);
		if (r == -1 || line[r - 1] != '\n')
			return (free(line), free_grid(m->grid, i), -1);
		line[--r] = '\0';
		if (i == 0)
			m->w = r;
		else if (r != m->w)
			return (free(line), free_grid(m->grid, i), -1);
		m->grid[i] = malloc(r + 1);
		if (!m->grid[i])
			return (free(line), free_grid(m->grid, i), -1);
		int j = -1;
		while (++j < r)
		{
			if (line[j] != m->empty && line[j] != m->obs)
				return (free(line), free_grid(m->grid, i + 1), -1);
			m->grid[i][j] = line[j];
		}
		m->grid[i][r] = '\0';
	}
	return (free(line), 0);
}

static int	mn(int a, int b, int c)
{
	if (a < b)
		return (a < c ? a : c);
	return (b < c ? b : c);
}

static void	solve(t_map *m)
{
	int	dp[m->h][m->w];
	int	best;
	int	bi;
	int	bj;
	int	i;
	int	j;

	best = 0;
	bi = 0;
	bj = 0;
	i = -1;
	while (++i < m->h)
	{
		j = -1;
		while (++j < m->w)
		{
			if (m->grid[i][j] == m->obs)
				dp[i][j] = 0;
			else if (i == 0 || j == 0)
				dp[i][j] = 1;
			else
				dp[i][j] = mn(dp[i - 1][j], dp[i - 1][j - 1], dp[i][j - 1]) + 1;
			if (dp[i][j] > best)
			{
				best = dp[i][j];
				bi = i - best + 1;
				bj = j - best + 1;
			}
		}
	}
	i = bi - 1;
	while (++i < bi + best)
	{
		j = bj - 1;
		while (++j < bj + best)
			m->grid[i][j] = m->full;
	}
}

int	execute_bsq(FILE *f)
{
	t_map	m;

	if (load(f, &m) == -1)
		return (-1);
	solve(&m);
	int i = -1;
	while (++i < m.h)
		fprintf(stdout, "%s\n", m.grid[i]);
	free_grid(m.grid, m.h);
	return (0);
}

int	main(int ac, char **av)
{
	if (ac == 1)
	{
		if (execute_bsq(stdin) == -1)
			fprintf(stderr, "map error\n");
	}
	else
	{
		int i = 0;
		while (++i < ac)
		{
			FILE *f = fopen(av[i], "r");
			if (!f || execute_bsq(f) == -1)
				fprintf(stderr, "map error\n");
			if (f)
				fclose(f);
			if (i < ac - 1)
				fprintf(stdout, "\n");
		}
	}
	return (0);
}