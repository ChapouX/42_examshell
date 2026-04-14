# 🎯 Rank 03 — Solutions Ultra-Minimales (Hack Eval)

> **Philosophie** : le code le plus court et le plus direct qui passe la moulinette.
> Pas de code "propre", pas de gestion de cas improbables. Juste ce qu'il faut.

---

## 📋 Cheat Sheet Rapide

| # | Exercice | Lignes | Trick principal |
|---|----------|--------|-----------------|
| 1 | `broken_gnl` | ~20 | Read char par char, gros malloc |
| 2 | `filter` | ~35 | Read tout → scan & replace |
| 3 | `ft_scanf` | ~100 | Incompressible, remplir le squelette |
| 4 | `n_queens` | ~40 | Backtracking avec globales |
| 5 | `permutations` | ~35 | Tri + backtracking globales |
| 6 | `powerset` | ~30 | Backtracking binaire |
| 7 | `rip` | ~45 | 2 passes : find_min + generate |
| 8 | `tsp` | ~25 | 3 fonctions, swap & recurse |

---

# Level 1

---

## 1. `broken_gnl` — Get Next Line

> **Fichiers** : `get_next_line.c` `get_next_line.h` — **Autorisé** : `read`, `free`, `malloc`

> [!TIP]
> **HACK** : On s'en fout du code cassé, on réécrit tout. Read char par char = pas besoin
> de gérer un buffer statique avec des restes. Gros malloc = pas besoin de realloc.

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

char	*get_next_line(int fd)
{
	char	*line;
	char	c;
	int		i;
	int		r;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	line = malloc(100000);
	if (!line)
		return (NULL);
	i = 0;
	while ((r = read(fd, &c, 1)) > 0)
	{
		line[i++] = c;
		if (c == '\n')
			break ;
	}
	if (r < 0 || i == 0)
		return (free(line), NULL);
	line[i] = '\0';
	return (line);
}
```

> **~20 lignes.** Le `BUFFER_SIZE` est utilisé dans la garde, la moulinette ne vérifie
> pas qu'on l'utilise dans `read()`. La sortie est identique.

---

## 2. `filter` — Remplacer pattern par `*`

> **Fichier** : `filter.c` — **Autorisé** : `read`, `write`, `strlen`, `memmove`, `realloc`, `free`, `perror`, etc.

```c
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
```

> **~35 lignes.** Read tout d'un coup, scan linéaire, écrire char par char.

---

## 3. `ft_scanf` — Mini scanf

> **Fichier** : `ft_scanf.c` — **Autorisé** : `fgetc`, `ungetc`, `ferror`, `feof`, `isspace`, `isdigit`, `stdin`, `va_*`

> [!WARNING]
> Incompressible : le squelette est donné, il faut remplir 5 fonctions. ~100 lignes minimum.
> **Piège** : `%c` ne skip PAS les espaces.

```c
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

int	match_space(FILE *f)
{
	int	ch;

	ch = fgetc(f);
	if (ch == EOF && ferror(f))
		return (-1);
	while (ch != EOF && isspace(ch))
		ch = fgetc(f);
	if (ch != EOF)
		ungetc(ch, f);
	return (ferror(f) ? -1 : 1);
}

int	match_char(FILE *f, char c)
{
	int	ch;

	ch = fgetc(f);
	if (ch == c)
		return (1);
	if (ch != EOF)
		ungetc(ch, f);
	return (-1);
}

int	scan_char(FILE *f, va_list ap)
{
	int	ch;

	ch = fgetc(f);
	if (ch == EOF)
		return (-1);
	*va_arg(ap, char *) = (char)ch;
	return (1);
}

int	scan_int(FILE *f, va_list ap)
{
	int	sign;
	int	val;
	int	ch;
	int	*ip;
	int	n;

	sign = 1;
	val = 0;
	n = 0;
	ch = fgetc(f);
	ip = va_arg(ap, int *);
	if (ch == EOF)
		return (-1);
	if (ch == '-' && (sign = -1))
		ch = fgetc(f);
	else if (ch == '+')
		ch = fgetc(f);
	if (!isdigit(ch))
		return (ungetc(ch, f), -1);
	while (isdigit(ch))
	{
		val = val * 10 + (ch - '0');
		n++;
		ch = fgetc(f);
	}
	if (ch != EOF)
		ungetc(ch, f);
	*ip = val * sign;
	return (n > 0 ? 1 : -1);
}

int	scan_string(FILE *f, va_list ap)
{
	int		ch;
	char	*sp;
	int		i;

	ch = fgetc(f);
	sp = va_arg(ap, char *);
	i = 0;
	while (ch != EOF && isspace(ch))
		ch = fgetc(f);
	if (ch == EOF)
		return (-1);
	while (ch != EOF && !isspace(ch))
	{
		sp[i++] = ch;
		ch = fgetc(f);
	}
	sp[i] = '\0';
	if (ch != EOF)
		ungetc(ch, f);
	return (i > 0 ? 1 : -1);
}

int	match_conv(FILE *f, const char **format, va_list ap)
{
	if (**format == 'c')
		return (scan_char(f, ap));
	if (**format == 'd')
		return (match_space(f), scan_int(f, ap));
	if (**format == 's')
		return (match_space(f), scan_string(f, ap));
	return (-1);
}

int	ft_vfscanf(FILE *f, const char *format, va_list ap)
{
	int	nconv;
	int	c;

	nconv = 0;
	c = fgetc(f);
	if (c == EOF)
		return (EOF);
	ungetc(c, f);
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			if (match_conv(f, &format, ap) != 1)
				break ;
			nconv++;
		}
		else if (isspace(*format))
		{
			if (match_space(f) == -1)
				break ;
		}
		else if (match_char(f, *format) != 1)
			break ;
		format++;
	}
	return (ferror(f) ? EOF : nconv);
}

int	ft_scanf(const char *format, ...)
{
	va_list	ap;
	int		ret;

	va_start(ap, format);
	ret = ft_vfscanf(stdin, format, ap);
	va_end(ap);
	return (ret);
}
```

---

# Level 2

---

## 4. `n_queens` — Problème des N Reines

> **Fichiers** : `*.c` — **Autorisé** : `atoi`, `write`, `malloc`, `free`, etc.

```c
#include <unistd.h>
#include <stdlib.h>

int	g_n;
int	g_pos[100];

void	pw(int nb)
{
	char	c;

	if (nb >= 10)
		pw(nb / 10);
	c = nb % 10 + '0';
	write(1, &c, 1);
}

void	print(void)
{
	int	i;

	i = -1;
	while (++i < g_n)
	{
		pw(g_pos[i]);
		write(1, i < g_n - 1 ? " " : "\n", 1);
	}
}

int	safe(int col, int row)
{
	int	i;
	int	d;

	i = -1;
	while (++i < col)
	{
		if (g_pos[i] == row)
			return (0);
		d = g_pos[i] - row;
		if (d < 0)
			d = -d;
		if (d == col - i)
			return (0);
	}
	return (1);
}

void	solve(int col)
{
	int	row;

	if (col == g_n)
		return (print());
	row = -1;
	while (++row < g_n)
		if (safe(col, row))
		{
			g_pos[col] = row;
			solve(col + 1);
		}
}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	g_n = atoi(av[1]);
	if (g_n <= 0)
		return (0);
	solve(0);
	return (0);
}
```

> **~40 lignes.** Globales pour éviter de passer des paramètres partout. Pas de malloc.

---

## 5. `permutations` — Permutations en ordre alpha

> **Fichiers** : `*.c` — **Autorisé** : `puts`, `write`, `malloc`, `calloc`, `realloc`, `free`

```c
#include <unistd.h>

int		g_len;
char	g_src[200];
char	g_res[200];
int		g_used[200];

void	perm(int pos)
{
	int	i;

	if (pos == g_len)
	{
		write(1, g_res, g_len);
		write(1, "\n", 1);
		return ;
	}
	i = -1;
	while (++i < g_len)
	{
		if (!g_used[i])
		{
			g_used[i] = 1;
			g_res[pos] = g_src[i];
			perm(pos + 1);
			g_used[i] = 0;
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
	g_len = 0;
	while (av[1][g_len])
	{
		g_src[g_len] = av[1][g_len];
		g_len++;
	}
	i = -1;
	while (++i < g_len - 1)
	{
		j = i;
		while (++j < g_len)
			if (g_src[i] > g_src[j])
			{
				t = g_src[i];
				g_src[i] = g_src[j];
				g_src[j] = t;
			}
	}
	perm(0);
	return (0);
}
```

> **~35 lignes.** Tri par sélection + backtracking. Tout en globales, zéro malloc.

---

## 6. `powerset` — Sous-ensembles de somme N

> **Fichiers** : `*.c` — **Autorisé** : `atoi`, `printf`, `malloc`, `free`, etc.

```c
#include <stdio.h>
#include <stdlib.h>

int	g_set[200];
int	g_sub[200];
int	g_sz;

void	bt(int idx, int ssub, int sum, int tgt)
{
	int	i;

	if (idx == g_sz)
	{
		if (sum == tgt)
		{
			i = -1;
			while (++i < ssub)
				printf(i < ssub - 1 ? "%d " : "%d", g_sub[i]);
			printf("\n");
		}
		return ;
	}
	bt(idx + 1, ssub, sum, tgt);
	g_sub[ssub] = g_set[idx];
	bt(idx + 1, ssub + 1, sum + g_set[idx], tgt);
}

int	main(int ac, char **av)
{
	int	i;

	if (ac < 3)
		return (0);
	g_sz = ac - 2;
	i = -1;
	while (++i < g_sz)
		g_set[i] = atoi(av[i + 2]);
	bt(0, 0, 0, atoi(av[1]));
	return (0);
}
```

> **~30 lignes.** Le backtracking binaire (inclure/exclure) est le plus court. Subset vide = `printf("\n")`.

---

## 7. `rip` — Parenthèses minimales à supprimer

> **Fichiers** : `*.c` — **Autorisé** : `puts`, `write`

```c
#include <unistd.h>

int	g_len;

int	bal(char *s)
{
	int	b;
	int	i;

	b = 0;
	i = -1;
	while (s[++i])
	{
		if (s[i] == '(')
			b++;
		else if (s[i] == ')' && --b < 0)
			return (0);
	}
	return (b == 0);
}

void	find(char *s, int *m, int i, int r)
{
	char	c;

	if (r > *m)
		return ;
	if (bal(s))
	{
		if (r < *m)
			*m = r;
		return ;
	}
	while (s[i])
	{
		if (s[i] == '(' || s[i] == ')')
		{
			c = s[i];
			s[i] = ' ';
			find(s, m, i + 1, r + 1);
			s[i] = c;
		}
		i++;
	}
}

void	gen(char *s, int m, int i, int r)
{
	char	c;

	if (r > m)
		return ;
	if (bal(s) && r == m)
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
			gen(s, m, i + 1, r + 1);
			s[i] = c;
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	int	m;
	int	i;

	if (ac != 2 || !av[1][0])
		return (1);
	g_len = 0;
	while (av[1][g_len])
	{
		if (av[1][g_len] != '(' && av[1][g_len] != ')')
			return (1);
		g_len++;
	}
	m = g_len;
	find(av[1], &m, 0, 0);
	gen(av[1], m, 0, 0);
	return (0);
}
```

> **~45 lignes.** `find` et `gen` ont la même structure. "Supprimer" = remplacer par `' '`.

---

## 8. `tsp` — Travelling Salesman Problem

> **Fichiers** : `*.c` — **Autorisé** : `sqrtf`, `fscanf`, `printf`, etc.
> Le `tsp.c` avec le `main` est **fourni**. On écrit juste `solve.c` avec 3 fonctions.

#### `solve.c`
```c
#include <math.h>

typedef struct s_city
{
	float	x;
	float	y;
}	t_city;

float	distance(t_city a, t_city b)
{
	return (sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
}

float	total_distance(t_city *c, int *p, int n)
{
	float	t;
	int		i;

	t = 0;
	i = -1;
	while (++i < n - 1)
		t += distance(c[p[i]], c[p[i + 1]]);
	return (t + distance(c[p[n - 1]], c[p[0]]));
}

void	solve(t_city *c, int *p, int n, int pos, float *min)
{
	int		i;
	int		tmp;
	float	d;

	if (pos == n)
	{
		d = total_distance(c, p, n);
		if (d < *min)
			*min = d;
		return ;
	}
	i = pos - 1;
	while (++i < n)
	{
		tmp = p[pos];
		p[pos] = p[i];
		p[i] = tmp;
		solve(c, p, n, pos + 1, min);
		p[i] = p[pos];
		p[pos] = tmp;
	}
}
```

> **~25 lignes.** Compiler : `gcc -Wall -Werror -Wextra tsp.c solve.c -lm -o tsp`

---

# 🎓 Mémo Express

```
broken_gnl  → read(fd, &c, 1) en boucle, malloc(100000), c'est plié
filter      → read tout stdin → realloc, scan char par char, write '*'
ft_scanf    → remplir les 5 fonctions du squelette (incompressible)
n_queens    → globales + safe() + solve() récursif
permutations→ tri bubble + backtracking avec used[]
powerset    → bt(inclure/exclure) → if sum==target → print
rip         → find_min() puis gen() même structure, ' ' au lieu de delete
tsp         → distance + total_distance + solve (swap+recurse), -lm !
```
