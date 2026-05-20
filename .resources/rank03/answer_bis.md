# 🎯 Rank 03 — Level 2 — Corrections "à ma façon"

---

## 3. `n_queens`

> **Fichier** : `*.c` — **Autorisé** : `atoi`, `write`, `malloc`, `free`, etc.

```c
#include <unistd.h>
#include <stdlib.h>

int	g_n;
int	g_row[100];

static void	putnbr(int nb)
{
	char	c;

	if (nb >= 10)
		putnbr(nb / 10);
	c = nb % 10 + '0';
	write(1, &c, 1);
}

static void	print_solution(void)
{
	int	i = 0;

	while (i < g_n)
	{
		putnbr(g_row[i]);
		if (i < g_n - 1)
			write(1, " ", 1);
		i++;
	}
	write(1, "\n", 1);
}

static int	is_safe(int col, int row)
{
	int	i = 0;
	int	diff;

	while (i < col)
	{
		if (g_row[i] == row)
			return (0);
		diff = g_row[i] - row;
		if (diff < 0)
			diff = -diff;
		if (diff == col - i)
			return (0);
		i++;
	}
	return (1);
}

static void	solve(int col)
{
	int	row = 0;

	if (col == g_n)
	{
		print_solution();
		return ;
	}
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

## 4. `permutations`

> **Fichier** : `*.c` — **Autorisé** : `puts`, `write`, `malloc`, `calloc`, `realloc`, `free`

```c
#include <unistd.h>

int		g_len;
char	g_in[200];
char	g_out[200];
int		g_used[200];

static void	perm(int pos)
{
	int	i = 0;

	if (pos == g_len)
	{
		write(1, g_out, g_len);
		write(1, "\n", 1);
		return ;
	}
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
	int		i = 0;
	int		j;
	char	t;

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
	while (i < g_len - 1)
	{
		j = i + 1;
		while (j < g_len)
		{
			if (g_in[i] > g_in[j])
			{
				t = g_in[i];
				g_in[i] = g_in[j];
				g_in[j] = t;
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

## 5. `powerset`

> **Fichier** : `*.c` — **Autorisé** : `atoi`, `printf`, `malloc`, `free`, etc.

```c
#include <stdio.h>
#include <stdlib.h>

int	g_val[200];
int	g_cur[200];
int	g_nb;

static void	bt(int idx, int cur_len, int sum, int goal)
{
	int	i = 0;

	if (idx == g_nb)
	{
		if (sum == goal)
		{
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
	int	i = 0;

	if (ac < 3)
		return (0);
	g_nb = ac - 2;
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

## 6. `rip`

> **Fichier** : `*.c` — **Autorisé** : `puts`, `write`

```c
#include <unistd.h>

int	g_len;

static int	is_balanced(char *s)
{
	int	depth = 0;
	int	i = 0;

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

static void	find_min(char *s, int *best, int i, int rem)
{
	char	c;

	if (rem > *best)
		return ;
	if (is_balanced(s))
	{
		if (rem < *best)
			*best = rem;
		return ;
	}
	while (s[i])
	{
		if (s[i] == '(' || s[i] == ')')
		{
			c = s[i];
			s[i] = ' ';
			find_min(s, best, i + 1, rem + 1);
			s[i] = c;
		}
		i++;
	}
}

static void	print_solutions(char *s, int max_rem, int i, int rem)
{
	char	c;

	if (rem > max_rem)
		return ;
	if (is_balanced(s) && rem == max_rem)
	{
		write(1, s, g_len);
		write(1, "\n", 1);
		return ;
	}
	while (s[i])
	{
		if (s[i] == '(' || s[i] == ')')
		{
			c = s[i];
			s[i] = ' ';
			print_solutions(s, max_rem, i + 1, rem + 1);
			s[i] = c;
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

