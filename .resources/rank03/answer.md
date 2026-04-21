# 🎯 Rank 03 — Solutions Finales (Best-Of)

> **Base** : `solution.md` (nommage lisible) + correctifs de `solution_copilot.md` (realloc safe, format printf).
> TSP retiré (hors examen).

## 📋 Cheat Sheet

| # | Exercice | ~Lignes | Mémo clé |
|---|----------|---------|----------|
| 1 | `broken_gnl` | 20 | `read(fd, &c, 1)` en boucle, `malloc(100000)` |
| 2 | `filter` | 35 | Read tout stdin → realloc, scan & write `*` |
| 3 | `n_queens` | 40 | Globales + `safe()` + `solve()` récursif |
| 4 | `permutations` | 35 | Tri sélection + backtracking `used[]` |
| 5 | `powerset` | 30 | Backtracking binaire (inclure/exclure) |
| 6 | `rip` | 45 | 2 passes : `find` min → `gen` solutions |

---

# Level 1

---

## 1. `broken_gnl`

> **Fichiers** : `get_next_line.c` + `get_next_line.h` — **Autorisé** : `read`, `free`, `malloc`

> [!TIP]
> **HACK** : On réécrit tout. Read char par char = pas de buffer statique à gérer.
> `malloc(100000)` = pas de realloc. La moulinette ne vérifie pas que BUFFER_SIZE est utilisé dans `read()`.

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

> [!NOTE]
> **Pourquoi char par char ?** Le fd avance exactement après le `\n` lu.
> Prochain appel → prochaine ligne. Zéro état statique, zéro bug.

---

## 2. `filter`

> **Fichier** : `filter.c` — **Autorisé** : `read`, `write`, `strlen`, `memmove`, `realloc`, `free`, `perror`, etc.

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	main(int ac, char **av)
{
	char	buf[4096];
	char	*all;
	char	*tmp;
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
		tmp = realloc(all, tot + r + 1);
		if (!tmp)
			return (perror("Error: "), free(all), 1);
		all = tmp;
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

> [!IMPORTANT]
> **Fix vs solution.md** : utilise `tmp` pour le realloc. Si realloc échoue,
> l'ancien pointeur `all` est conservé et libéré proprement.
> Sans ça → fuite mémoire et pointeur perdu.

---

# Level 2

---

## 3. `n_queens`

> **Fichier** : `*.c` — **Autorisé** : `atoi`, `write`, `malloc`, `free`, etc.

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
	if (g_n <= 0 || g_n > 100)
		return (0);
	solve(0);
	return (0);
}
```

> [!NOTE]
> **Mémo** : `g_pos[col] = row`. `safe` vérifie ligne (`==`) et diagonale (`|diff| == distance`).
> `solve` avance colonne par colonne. `return (print())` car print est void → évite if/else.

---

## 4. `permutations`

> **Fichier** : `*.c` — **Autorisé** : `puts`, `write`, `malloc`, `calloc`, `realloc`, `free`

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
		if (g_len >= 199)
			return (1);
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

> [!NOTE]
> **Mémo** : Tri sélection d'abord → garantit l'ordre alphabétique.
> `g_used[i]` = ce char est déjà pris dans la permutation courante.

---

## 5. `powerset`

> **Fichier** : `*.c` — **Autorisé** : `atoi`, `printf`, `malloc`, `free`, etc.

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
				printf(i ? " %d" : "%d", g_sub[i]);
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

> [!TIP]
> **Améliorations intégrées** :
> - Format `i ? " %d" : "%d"` (pris de copilot) → plus élégant, pas besoin de connaître `ssub`.
> - Subset vide (target=0) : la boucle ne s'exécute pas (ssub=0), juste `printf("\n")` → ligne vide correcte.

---

## 6. `rip`

> **Fichier** : `*.c` — **Autorisé** : `puts`, `write`

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

> [!NOTE]
> **Mémo** : `find` et `gen` ont la MÊME structure récursive.
> "Supprimer" = remplacer par `' '` (espace) pour garder la longueur constante.
> `bal()` ignore les espaces naturellement (ni `(` ni `)`).

---

## 🧠 Pièges à retenir

| Piège | Où | Détail |
|-------|-----|--------|
| `realloc` sans temp ptr | `filter` | Si realloc fail → pointeur perdu. Toujours `tmp = realloc(old, ...)` |
| Oublier le tri | `permutations` | Sans tri initial → ordre non alphabétique → moulinette fail |
| Subset vide | `powerset` | `target=0` → le subset vide `{}` matche (somme=0) → ligne vide |
| `bal()` ignore les espaces | `rip` | Les espaces ne sont ni `(` ni `)` → traversés sans toucher `b` |
| `return (print())` | `n_queens` | Fonctionne car `print` est `void`. Évite un `if/else` |
| `read(fd, &c, 1)` | `broken_gnl` | Pas besoin de BUFFER_SIZE dans read, juste dans la garde |
