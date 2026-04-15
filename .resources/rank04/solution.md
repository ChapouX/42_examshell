# 🎯 Rank 04 — Solutions Ultra-Minimales (Hack Eval)

> **Philosophie** : le code le plus court et le plus direct qui passe la moulinette.
> Pas de code "propre", pas de gestion de cas improbables. Juste ce qu'il faut.

---

## 📋 Cheat Sheet Rapide

| # | Exercice | Lignes | Trick principal |
|---|----------|--------|-----------------|
| 1 | `ft_popen` | ~35 | pipe + fork + dup2, renvoyer le bon bout |
| 2 | `picoshell` | ~40 | Boucle pipe+fork, chaîner last_fd |
| 3 | `sandbox` | ~45 | fork + alarm + waitpid, EINTR = timeout |
| 4 | `argo` | ~80 | Fonctions données + get_str + parse_map |
| 5 | `vbc` | ~60 | Grammaire récursive : sum→product→factor |

---

# Level 1

---

## 1. `ft_popen` — Popen maison

> **Fichier** : `ft_popen.c` — **Autorisé** : `pipe`, `fork`, `dup2`, `execvp`, `close`, `exit`

> [!TIP]
> **HACK** : C'est un `popen()` simplifié. Pipe + fork. L'enfant redirige le bon
> bout du pipe (stdout pour 'r', stdin pour 'w') et exec. Le parent ferme l'autre bout
> et retourne le fd. Ne pas oublier de fermer les deux bouts dans l'enfant après dup2.

```c
#include <unistd.h>
#include <stdlib.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (close(fd[0]), close(fd[1]), -1);
	if (pid == 0)
	{
		if (type == 'r')
			dup2(fd[1], 1);
		else
			dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(1);
	}
	if (type == 'r')
		return (close(fd[1]), fd[0]);
	return (close(fd[0]), fd[1]);
}
```

> **~35 lignes.** Le seul piège c'est de ne pas leak les fd : l'enfant ferme
> les DEUX bouts après dup2, le parent ferme le bout inutile.

---

## 2. `picoshell` — Pipeline de commandes

> **Fichier** : `picoshell.c` — **Autorisé** : `close`, `fork`, `wait`, `exit`, `execvp`, `dup2`, `pipe`

> [!TIP]
> **HACK** : Une seule boucle. `last_fd` garde le bout lecture du pipe précédent.
> Si cmd suivante existe → pipe. L'enfant branche stdin sur `last_fd` et stdout
> sur le nouveau pipe. Le parent ferme le bout écriture et passe au suivant.

```c
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int	picoshell(char **cmds[])
{
	int		fd[2];
	int		last_fd;
	pid_t	pid;
	int		i;

	last_fd = -1;
	i = -1;
	while (cmds[++i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (1);
		pid = fork();
		if (pid == -1)
		{
			if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			return (1);
		}
		if (pid == 0)
		{
			if (last_fd != -1)
			{
				dup2(last_fd, 0);
				close(last_fd);
			}
			if (cmds[i + 1])
			{
				close(fd[0]);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (last_fd != -1)
			close(last_fd);
		if (cmds[i + 1])
		{
			close(fd[1]);
			last_fd = fd[0];
		}
	}
	while (wait(NULL) > 0)
		;
	return (0);
}
```

> **~40 lignes.** Même pattern que minishell mais sans parsing. La variable
> `last_fd` est la clé : elle chaîne les pipes entre chaque commande.

---

# Level 2

---

## 5. `vbc` — Calculatrice à précédence

> **Fichiers** : `vbc.c` `vbc.h` — **Autorisé** : `malloc`, `calloc`, `realloc`, `free`, `printf`, `isdigit`, `write`

> [!TIP]
> **HACK** : Grammaire récursive descendante sans arbre. `sum` appelle `product`,
> `product` appelle `factor`, `factor` gère `(expr)` et les chiffres. Un pointeur
> global `char *s` avance dans la string. Pré-validation de l'input avant le parsing.

> [!IMPORTANT]
> **Deux approches** : la simple (évaluation directe, ~60 lignes) ou celle du
> given.c (arbre AST). La version directe est bien plus courte pour l'exam.

#### `vbc.h`
```c
#ifndef VBC_H
# define VBC_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>

void	unexpected(char c);
int		ft_factor(void);
int		ft_product(void);
int		ft_sum(void);
int		check_input(char *str);

#endif
```

#### `vbc.c`
```c
#include "vbc.h"

char	*s;

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");
}

int	ft_factor(void)
{
	int	n;

	if (isdigit(*s))
		return (*s++ - '0');
	if (*s == '(')
	{
		s++;
		n = ft_sum();
		s++;
		return (n);
	}
	return (0);
}

int	ft_product(void)
{
	int	a;

	a = ft_factor();
	while (*s == '*')
	{
		s++;
		a *= ft_factor();
	}
	return (a);
}

int	ft_sum(void)
{
	int	a;

	a = ft_product();
	while (*s == '+')
	{
		s++;
		a += ft_product();
	}
	return (a);
}

int	check_input(char *str)
{
	int	par;
	int	i;

	par = 0;
	i = -1;
	while (str[++i])
	{
		if (str[i] == '(')
			par++;
		else if (str[i] == ')')
			par--;
		else if (!isdigit(str[i]) && str[i] != '+'
			&& str[i] != '*')
			return (unexpected(str[i]), 1);
		if (isdigit(str[i]) && isdigit(str[i + 1]))
			return (unexpected(str[i + 1]), 1);
		if (par < 0)
			return (unexpected(')'), 1);
	}
	if (par > 0)
		return (unexpected('('), 1);
	if (i > 0 && (str[i - 1] == '+' || str[i - 1] == '*'))
		return (unexpected(0), 1);
	if (i == 0)
		return (unexpected(0), 1);
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	if (check_input(argv[1]))
		return (1);
	s = argv[1];
	printf("%d\n", ft_sum());
	return (0);
}
```

> **~60 lignes utiles.** L'évaluation récursive directe évite tout malloc/arbre.
> La grammaire respecte la précédence : `sum` (addition) → `product` (multiplication)
> → `factor` (chiffre ou parenthèses). `check_input` valide avant de parser.

---

## 📝 Récap' Mémo Express

```
ft_popen    → pipe + fork, enfant: dup2 + close les 2 bouts + execvp
picoshell   → boucle cmds[], last_fd chaîne les pipes, wait(NULL) à la fin
sandbox     → fork, enfant: f()+exit(0), parent: alarm+waitpid, EINTR=timeout
argo        → given.c déjà fourni, écrire get_str + parse_map + argo (récursif)
vbc         → sum→product→factor, pointeur global *s, check_input avant
```

## ⚡ Tips d'Exam

> [!IMPORTANT]
> - **Level 1** (ft_popen / picoshell / sandbox) : tous basés sur `pipe+fork+exec`.
>   Si tu maîtrises minishell, c'est du déjà-vu.
> - **Level 2** (argo / vbc) : parsing récursif. La structure est la même :
>   une fonction par niveau de la grammaire.
> - **fd leaks** : le piège #1. Toujours fermer les DEUX bouts du pipe dans l'enfant
>   après dup2, et le bout inutile dans le parent.
> - **Zombies** : toujours `wait()` / `waitpid()` pour chaque `fork()`.
> - **argo** : recopie le `given.c` en entier d'abord, puis ajoute tes fonctions.
> - **vbc** : `check_input` ne fait PAS partie du parsing. C'est une pré-validation
>   pour les cas d'erreur (parenthèses, opérateurs seuls, chiffres consécutifs).
