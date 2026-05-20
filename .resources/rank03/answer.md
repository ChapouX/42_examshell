# 🎯 Rank 03 — Solutions Finales (Best-Of)

---

# Level 1

---

## 1. `broken_gnl`

#### `get_next_line.h`
```c
#ifndef GNL
# define GNL

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif

char	*get_next_line(int fd);

#endif
```
#### `get_next_line.c`
```c
#include "get_next_line.h"

static char *join(char *line, char *buf, int n)
{
	char *out;
	int i = 0;
	int j = 0;
	
	while (line && line[i])
		i++;
	out = malloc(i + n + 1);
	if (!out)
		return(free(line), NULL);
	while (j < i)
	{
		out[j] = line[j];
		j++;
	}
	j = 0;
	while (j < n)
	{
		out[i + j] = buf[j];
		j++;
	}
	out[i + n] = '\0';
	free(line);
	return (out);
}

char *get_next_line(int fd)
{
	static char buf[BUFFER_SIZE];
	char *line = NULL;
	static int pos = 0;
	static int got = 0;
	int i = 0;
	int b_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (1)
	{
		if (pos >= got)
		{
			b_read = read(fd, buf, BUFFER_SIZE);
			if (b_read <= 0)
			{
				if (b_read < 0)
				{
					free(line);
					return (NULL);
				}
				return (line);
			}
			got = (int)b_read;
			pos = 0;
		}
		i = pos;
		while (pos < got && buf[pos] != '\n')
			pos++;
		if (pos > i)
		{
			line = join(line, buf + i, pos - i);
			if (!line)
				return (NULL);
		}
		if (pos < got && buf[pos] == '\n')
		{
			line = join(line, "\n", 1);
			if (!line)
				return (NULL);
			pos++;
			break;
		}
	}
	return (line);
}
```

---

## 2. `filter`

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static int is_match(char *acc, char *pat, int pat_len)
{
	int	i = 0;

	while (i < pat_len)
	{
		if (acc[i] == pat[i])
			i++;
		else
			return(0);
	}
	return (1);
}

int	main(int ac, char **av)
{
	char buf[4096];
	char *acc = NULL;
	char *tmp = NULL;
	int pos = 0;
	int b_read = 0;
	int i;
	int p_len;
	int len = 0;

	if (ac != 2 || !av[1][0])
		return (1);
	p_len = strlen(av[1]);
	while ((b_read = read(0, buf, sizeof(buf))) != 0)
	{
		i = 0;
		if (b_read < 0)
			return(perror("Error"), free(acc), 1);
		tmp = realloc(acc, len + b_read);
		if (!tmp)
			return(perror("Error"), free(acc), 1);
		acc = tmp;
		while (i < b_read)
		{
			acc[len + i] = buf[i];
			i++;
		}
		len += b_read;
	}
	i = 0;
	while (pos <= len - p_len)
	{
		if (is_match(acc + pos, av[1], p_len))
		{
			i = 0;
			while (i < p_len)
			{
				write(1, "*", 1);
				i++;
				pos++;
			}
		}
		else
		{
			write(1, &acc[pos], 1);
			pos++;
		}
	}
	while(pos < len)
	{
		write(1, &acc[pos], 1);
		pos++;
	}
	free(acc);
	return (0);
}
```

---

# Level 2

---

## 1. `n_queens`

```c
#include <unistd.h>
#include <stdlib.h>

int	g_n;
int	g_row[100];

void	putnbr(int nb)
{
	char	c;

	if (nb >= 10)
		putnbr(nb / 10);
	c = nb % 10 + '0';
	write(1, &c, 1);
}

void	print_solution(void)
{
	int	col;

	col = 0;
	while (col < g_n)
	{
		putnbr(g_row[col]);
		if (col < g_n - 1)
			write(1, " ", 1);
		col++;
	}
	write(1, "\n", 1);
}

int	is_safe(int col, int row)
{
	int	prev;
	int	diff;

	prev = 0;
	while (prev < col)
	{
		if (g_row[prev] == row)
			return (0);
		diff = g_row[prev] - row;
		if (diff < 0)
			diff = -diff;
		if (diff == col - prev)
			return (0);
		prev++;
	}
	return (1);
}

void	solve(int col)
{
	int	row;

	if (col == g_n)
	{
		print_solution();
		return ;
	}
	row = 0;
	while (row < g_n)
	{
		if (is_safe(col, row))
		{
			g_row[col] = row;
			solve(col + 1);
		}
		row++;
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	g_n = atoi(av[1]);
	if (g_n <= 0 || g_n > 100)
		return (0);
	solve(0);
	return (0);
}
```

---

## 2. `permutations`

```c
#include <unistd.h>

int		g_len;
char	g_in[200];
char	g_out[200];
int		g_used[200];

void	perm(int pos)
{
	int	i;

	if (pos == g_len)
	{
		write(1, g_out, g_len);
		write(1, "\n", 1);
		return ;
	}
	i = 0;
	while (i < g_len)
	{
		if (!g_used[i])
		{
			g_used[i] = 1;
			g_out[pos] = g_in[i];
			perm(pos + 1);
			g_used[i] = 0;
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	int		i;
	int		j;
	char	tmp;

	if (ac != 2 || !av[1][0])
		return (1);
	g_len = 0;
	while (av[1][g_len])
	{
		if (g_len >= 199)
			return (1);
		g_in[g_len] = av[1][g_len];
		g_len++;
	}
	i = 0;
	while (i < g_len - 1)
	{
		j = i + 1;
		while (j < g_len)
		{
			if (g_in[i] > g_in[j])
			{
				tmp = g_in[i];
				g_in[i] = g_in[j];
				g_in[j] = tmp;
			}
			j++;
		}
		i++;
	}
	perm(0);
	return (0);
}
```

---

## 3. `powerset`

```c
#include <stdio.h>
#include <stdlib.h>

int	g_val[200];
int	g_cur[200];
int	g_nb;

void	bt(int idx, int cur_len, int sum, int goal)
{
	int	i;

	if (idx == g_nb)
	{
		if (sum == goal)
		{
			i = 0;
			while (i < cur_len)
			{
				if (i > 0)
					printf(" ");
				printf("%d", g_cur[i]);
				i++;
			}
			printf("\n");
		}
		return ;
	}
	bt(idx + 1, cur_len, sum, goal);
	g_cur[cur_len] = g_val[idx];
	bt(idx + 1, cur_len + 1, sum + g_val[idx], goal);
}

int	main(int ac, char **av)
{
	int	i;

	if (ac < 3)
		return (0);
	g_nb = ac - 2;
	i = 0;
	while (i < g_nb)
	{
		g_val[i] = atoi(av[i + 2]);
		i++;
	}
	bt(0, 0, 0, atoi(av[1]));
	return (0);
}
```

---

## 4. `rip`

```c
#include <unistd.h>

int	g_len;

int	is_balanced(char *s)
{
	int	depth;
	int	i;

	depth = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == '(')
			depth++;
		else if (s[i] == ')')
		{
			depth--;
			if (depth < 0)
				return (0);
		}
		i++;
	}
	return (depth == 0);
}

void	find_min(char *s, int *best, int i, int removed)
{
	char	saved;

	if (removed > *best)
		return ;
	if (is_balanced(s))
	{
		if (removed < *best)
			*best = removed;
		return ;
	}
	while (s[i])
	{
		if (s[i] == '(' || s[i] == ')')
		{
			saved = s[i];
			s[i] = ' ';
			find_min(s, best, i + 1, removed + 1);
			s[i] = saved;
		}
		i++;
	}
}

void	print_solutions(char *s, int goal, int i, int removed)
{
	char	saved;

	if (removed > goal)
		return ;
	if (is_balanced(s) && removed == goal)
	{
		write(1, s, g_len);
		write(1, "\n", 1);
		return ;
	}
	while (s[i])
	{
		if (s[i] == '(' || s[i] == ')')
		{
			saved = s[i];
			s[i] = ' ';
			print_solutions(s, goal, i + 1, removed + 1);
			s[i] = saved;
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	int	min;

	if (ac != 2 || !av[1][0])
		return (1);
	g_len = 0;
	while (av[1][g_len])
	{
		if (av[1][g_len] != '(' && av[1][g_len] != ')')
			return (1);
		g_len++;
	}
	min = g_len;
	find_min(av[1], &min, 0, 0);
	print_solutions(av[1], min, 0, 0);
	return (0);
}
```
---
