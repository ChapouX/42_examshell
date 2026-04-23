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

**Workflow pour s'en souvenir :**
1. Gérer les erreurs de base (`fd < 0` ou `BUFFER_SIZE <= 0`).
2. Faire un gros `malloc(100000)` pour éviter les `realloc`.
3. Lire caractère par caractère (`read(fd, &c, 1)`) dans une boucle.
4. Stocker chaque caractère et s'arrêter si on rencontre un `\n`.
5. Gérer les erreurs de lecture, ajouter le `\0` final et retourner.

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

**Workflow pour s'en souvenir :**
1. Lire tout `stdin` par blocs avec `read()` et cumuler dans une grande chaîne `all` avec `realloc()`.
2. Parcourir la chaîne `all` caractère par caractère.
3. À chaque position, essayer de correspondre l'argument avec une boucle imbriquée (`j`).
4. Si correspondance complète trouvée, imprimer la bonne quantité d'étoiles `*`, et avancer l'index extérieur (`i += tl`).
5. Sinon, imprimer le caractère tel quel, et avancer (`i++`).

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

**Workflow pour s'en souvenir :**
1. Variables globales `g_n` (taille de l'échiquier) et `g_pos` (tableau où `g_pos[col] = row`).
2. Fonction `safe(col, row)` : vérifier pour toutes les colonnes précédentes (`i < col`) si on est sur la même ligne ou sur une diagonale (`|g_pos[i] - row| == col - i`).
3. Fonction `solve(col)` :
   - Si `col == g_n`, afficher la grille (`print()`).
   - Sinon, pour chaque ligne (`row` de 0 à `g_n-1`), si `safe(col, row)` est vrai, `g_pos[col] = row` puis appel récursif  `solve(col + 1)`.
4. Dans le `main`, on lit l'argument et on lance `solve(0)`.

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

**Workflow pour s'en souvenir :**
1. Tableaux globaux : `g_src` (lettres de départ), `g_res` (permutation en cours), `g_used` (marquage des lettres utilisées) et `g_len` (taille).
2. Dans `main` : stocker `av[1]` dans `g_src` et le trier par ordre alphabétique croissant, pour garantir l'ordre lexicographique.
3. Fonction récursive `perm(pos)` :
   - Condition d'arrêt : si `pos == g_len`, afficher `g_res`.
   - Boucle sur chaque lettre `i` de `g_src`. Si elle n'est pas utilisée (`!g_used[i]`) :
     - La marquer utilisée `g_used[i] = 1`, l'ajouter à `g_res[pos] = g_src[i]`.
     - Appel récursif `perm(pos + 1)`.
     - Backtracking : la marquer libre `g_used[i] = 0`.

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

**Workflow pour s'en souvenir :**
1. Variables globales : taille `g_sz`, liste des nombres convertis `g_set`, et sous-ensemble en cours de construction `g_sub`.
2. `main` convertit chaque argument en entier dans `g_set` et appelle `bt(0, 0, 0, target)`.
3. Fonction `bt(idx, ssub, sum, target)` : backtracking binaire.
   - À chaque index `idx`, si on a parcouru tout le set (`idx == g_sz`) vérifier : `sum == target`. Si oui, afficher `g_sub` (ou une ligne vide s'il est vide).
   - On fait un appel sans inclure `g_set[idx]` : `bt(idx+1, ssub, sum, tgt)`.
   - On fait un appel en l'incluant : `g_sub[ssub] = g_set[idx]`, et on appelle `bt(idx+1, ssub+1, sum + g_set[idx], tgt)`.

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

**Workflow pour s'en souvenir :**
1. Créer une fonction de vérification `bal(s)` qui valide si la chaîne a des parenthèses bien balancées en ignorant les espaces (incrémenter pour `(`, décrémenter pour `)` ; fail direct si `b < 0` ; check final `b == 0`).
2. Créer une fonction récursive `find(s, &max_remove, i, removed)` :
   - But : trouver le minimum de suppressions de parenthèses pour avoir `bal(s) == vrai`.
   - Remplace tour à tour chaque parenthèse par `' '` (comme ça pas besoin de décaler la chaîne ou de faire des `malloc`/`free`). 
3. Créer une fonction récursive `gen(s, max_remove, i, removed)` qui fait exactement pareil que `find`, mais l'affiche.
   - Si `bal(s)` est vrai et qu'on a fait EXACTEMENT `max_remove` remplacements, on `write`.
4. Le `main` appelle d'abord `find` pour obtenir `m`, puis `gen`.

---



```
broken_gnl  → read(fd, &c, 1) en boucle, malloc(100000), c'est plié
filter      → read tout stdin → realloc, scan char par char, write '*'
n_queens    → globales + safe() + solve() récursif
permutations→ tri bubble + backtracking avec used[]
powerset    → bt(inclure/exclure) → if sum==target → print
rip         → find_min() puis gen() même structure, ' ' au lieu de delete
```
