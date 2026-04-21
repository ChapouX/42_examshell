# 🎯 Rank 04 — Solutions Finales (Best-Of)

> **Base** : `solution_copilot.md` (meilleure gestion d'erreurs)
> + nommage lisible de `solution.md` + corrections identifiées.
> Sandbox et argo retirés (hors examen).

## 📋 Cheat Sheet

| # | Exercice | ~Lignes | Mémo clé |
|---|----------|---------|----------|
| 1 | `ft_popen` | 30 | pipe + fork, enfant: dup2 + close 2 bouts + execvp |
| 2 | `picoshell` | 55 | Boucle cmds[], `last_fd` chaîne les pipes, wait à la fin |
| 3 | `vbc` | 60 | sum→product→factor, pointeur global `*s`, erreur = -1 |

---

# Level 1

---

## 1. `ft_popen`

> **Fichier** : `ft_popen.c` — **Autorisé** : `pipe`, `fork`, `dup2`, `execvp`, `close`, `exit`

> [!TIP]
> **HACK** : C'est un `popen()` simplifié. Pipe + fork. L'enfant redirige le bon
> bout du pipe et exec. Le parent ferme l'autre bout et retourne le fd.

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
		{
			if (dup2(fd[1], 1) == -1)
				exit(1);
		}
		else if (dup2(fd[0], 0) == -1)
			exit(1);
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

> [!IMPORTANT]
> **Fix vs solution.md** : vérifie le retour de `dup2()`. Si dup2 échoue dans
> l'enfant, `exit(1)` au lieu de continuer vers execvp avec des fd cassés.

---

## 2. `picoshell`

> **Fichier** : `picoshell.c` — **Autorisé** : `close`, `fork`, `wait`, `exit`, `execvp`, `dup2`, `pipe`

> [!TIP]
> **HACK** : Une seule boucle. `last_fd` garde le bout lecture du pipe précédent.
> Si cmd suivante existe → pipe. L'enfant branche stdin/stdout. Le parent passe au suivant.

```c
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static int	cleanup(int last_fd, int fd[2], int has_pipe)
{
	if (last_fd != -1)
		close(last_fd);
	if (has_pipe)
	{
		close(fd[0]);
		close(fd[1]);
	}
	while (wait(NULL) > 0)
		;
	return (1);
}

int	picoshell(char **cmds[])
{
	int		fd[2];
	int		last_fd;
	int		i;
	pid_t	pid;

	last_fd = -1;
	i = 0;
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (cleanup(last_fd, fd, 0));
		pid = fork();
		if (pid == -1)
			return (cleanup(last_fd, fd, cmds[i + 1] != NULL));
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
		else
			last_fd = -1;
		i++;
	}
	while (wait(NULL) > 0)
		;
	return (0);
}
```

> [!IMPORTANT]
> **Fix vs solution.md** : `cleanup()` attend les enfants déjà forkés avant de
> retourner 1. Sans ça → zombies si pipe/fork échoue au milieu du pipeline.

---

# Level 2

---

## 3. `vbc`

> **Fichier** : `*.c` `*.h` — **Autorisé** : `malloc`, `calloc`, `realloc`, `free`, `printf`, `isdigit`, `write`

> [!TIP]
> **HACK** : Ignore le `given.c` (approche AST). Évaluation directe par descente
> récursive. `sum` appelle `product`, `product` appelle `factor`. Erreur = retour `-1`.

#### `vbc.h`
```c
#ifndef VBC_H
# define VBC_H

# include <stdio.h>
# include <ctype.h>

void	unexpected(char c);
int		ft_factor(void);
int		ft_product(void);
int		ft_sum(void);

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
		if (n < 0)
			return (-1);
		if (*s != ')')
			return (unexpected(*s), -1);
		s++;
		return (n);
	}
	return (unexpected(*s), -1);
}

int	ft_product(void)
{
	int	a;
	int	b;

	a = ft_factor();
	if (a < 0)
		return (-1);
	while (*s == '*')
	{
		s++;
		b = ft_factor();
		if (b < 0)
			return (-1);
		a *= b;
	}
	return (a);
}

int	ft_sum(void)
{
	int	a;
	int	b;

	a = ft_product();
	if (a < 0)
		return (-1);
	while (*s == '+')
	{
		s++;
		b = ft_product();
		if (b < 0)
			return (-1);
		a += b;
	}
	return (a);
}

int	main(int argc, char **argv)
{
	int	n;

	if (argc != 2)
		return (1);
	s = argv[1];
	if (!*s)
		return (unexpected(0), 1);
	n = ft_sum();
	if (n < 0)
		return (1);
	if (*s)
		return (unexpected(*s), 1);
	printf("%d\n", n);
	return (0);
}
```

> [!IMPORTANT]
> **Approche copilot choisie** (pas de `check_input`) car :
> - Moins de lignes totales (~55 vs ~80 avec check_input)
> - Gère TOUS les cas d'erreur pendant le parsing (ex: `(+)` → erreur correcte)
> - `solution.md` avec check_input rate certains cas : `(+)` évalue silencieusement à 0

> [!NOTE]
> **Mémo** : `-1` comme sentinel d'erreur est safe car les valeurs sont 0-9
> et les opérations `+` et `*` → le résultat est toujours ≥ 0.

---

## 🧠 Pièges à retenir

| Piège | Où | Détail |
|-------|-----|--------|
| fd leaks | ft_popen, picoshell | Enfant: fermer les DEUX bouts après dup2. Parent: fermer le bout inutile |
| Zombies | picoshell | Toujours `wait()` pour chaque `fork()`, même en cas d'erreur |
| `dup2` peut échouer | ft_popen | Vérifier le retour, `exit(1)` si échec |
| `-1` = erreur | vbc | Propager systématiquement, tester avant de continuer |
| `given.c` | vbc | Ignore le given.c (approche AST), évaluation directe |
