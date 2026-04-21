# 🎯 Rank 04 — Solutions Finales (Best-Of)

> **Base** : `solution_copilot.md` (seul à couvrir les 5 exos, meilleure gestion d'erreurs)
> + nommage lisible de `solution.md` + corrections identifiées.

## 📋 Cheat Sheet

| # | Exercice | ~Lignes | Mémo clé |
|---|----------|---------|----------|
| 1 | `ft_popen` | 30 | pipe + fork, enfant: dup2 + close 2 bouts + execvp |
| 2 | `picoshell` | 55 | Boucle cmds[], `last_fd` chaîne les pipes, wait à la fin |
| 3 | `sandbox` | 70 | fork + alarm + waitpid, EINTR = timeout, WUNTRACED |
| 4 | `argo` | 80 | given.c fourni, écrire get_str + parse_map + parse_value |
| 5 | `vbc` | 60 | sum→product→factor, pointeur global `*s`, erreur = -1 |

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

## 3. `sandbox`

> **Fichier** : `sandbox.c` — **Autorisé** : `fork`, `waitpid`, `exit`, `alarm`, `sigaction`, `kill`, `printf`, `strsignal`, etc.

> [!WARNING]
> **Absent de solution.md.** Code pris de copilot, seule source disponible.

```c
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void	alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	pid_t			pid;
	int				st;

	if (!f)
		return (-1);
	sa.sa_handler = alarm_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	if (!timeout)
	{
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		if (verbose)
			printf("Bad function: timed out after %u seconds\n", timeout);
		return (0);
	}
	alarm(timeout);
	if (waitpid(pid, &st, WUNTRACED) == -1)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
	if (WIFSTOPPED(st))
	{
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		if (verbose)
			printf("Bad function: %s\n", strsignal(WSTOPSIG(st)));
		return (0);
	}
	if (WIFSIGNALED(st))
	{
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(st)));
		return (0);
	}
	if (WIFEXITED(st) && WEXITSTATUS(st) == 0)
	{
		if (verbose)
			printf("Nice function!\n");
		return (1);
	}
	if (WIFEXITED(st))
	{
		if (verbose)
			printf("Bad function: exited with code %d\n", WEXITSTATUS(st));
		return (0);
	}
	return (-1);
}
```

> [!NOTE]
> **Points clés** :
> - `timeout=0` → kill immédiat (le REF sandbox.c ne gère pas ce cas → bug)
> - `WUNTRACED` dans waitpid → détecte les processus stoppés (SIGSTOP)
> - `alarm(0)` après waitpid → annule l'alarme si l'enfant finit avant le timeout
> - `EINTR` = le SIGALRM a interrompu waitpid → c'est le timeout

---

# Level 2

---

## 4. `argo`

> **Fichier** : `argo.c` — **Autorisé** : `getc`, `ungetc`, `printf`, `malloc`, `realloc`, `free`, `isdigit`, etc.

> [!WARNING]
> **Absent de solution.md.** Le `given.c` fournit : types (json/pair), `peek`, `unexpected`,
> `accept`, `expect`, `free_json`, `serialize`, `main`. Tu écris **seulement** les fonctions
> de parsing ci-dessous.

> [!TIP]
> **HACK** : recopie le `given.c` en entier, puis ajoute ces fonctions avant le `main`.

```c
static char	*get_str(FILE *stream)
{
	char	*s;
	char	*tmp;
	size_t	len;
	size_t	cap;
	int		c;

	cap = 16;
	len = 0;
	s = malloc(cap);
	if (!s)
		return (NULL);
	(void)getc(stream);
	while ((c = getc(stream)) != EOF)
	{
		if (c == '"')
		{
			s[len] = 0;
			return (s);
		}
		if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
				return (free(s), unexpected(stream), NULL);
		}
		if (len + 1 >= cap)
		{
			cap *= 2;
			tmp = realloc(s, cap);
			if (!tmp)
				return (free(s), NULL);
			s = tmp;
		}
		s[len++] = c;
	}
	free(s);
	unexpected(stream);
	return (NULL);
}

static int	parse_value(json *dst, FILE *stream);

static int	parse_int(json *dst, FILE *stream)
{
	int	n;
	int	sign;

	sign = 1;
	n = 0;
	if (peek(stream) == '-')
	{
		(void)getc(stream);
		sign = -1;
	}
	if (!isdigit(peek(stream)))
		return (unexpected(stream), -1);
	while (isdigit(peek(stream)))
		n = n * 10 + (getc(stream) - '0');
	dst->type = INTEGER;
	dst->integer = n * sign;
	return (1);
}

static int	parse_map(json *dst, FILE *stream)
{
	pair	*tmp;
	pair	*cur;

	dst->type = MAP;
	dst->map.data = NULL;
	dst->map.size = 0;
	(void)getc(stream);
	if (peek(stream) == '}')
		return ((void)getc(stream), 1);
	while (1)
	{
		if (peek(stream) != '"')
			return (unexpected(stream), -1);
		tmp = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		if (!tmp)
			return (-1);
		dst->map.data = tmp;
		cur = &dst->map.data[dst->map.size];
		cur->key = get_str(stream);
		if (!cur->key)
			return (-1);
		cur->value = (json){0};
		dst->map.size++;
		if (!expect(stream, ':'))
			return (-1);
		if (parse_value(&cur->value, stream) == -1)
			return (-1);
		if (peek(stream) == '}')
			return ((void)getc(stream), 1);
		if (!accept(stream, ','))
			return (unexpected(stream), -1);
	}
}

static int	parse_value(json *dst, FILE *stream)
{
	int	c;

	c = peek(stream);
	if (c == EOF)
		return (unexpected(stream), -1);
	if (c == '{')
		return (parse_map(dst, stream));
	if (c == '"')
	{
		dst->type = STRING;
		dst->string = get_str(stream);
		if (!dst->string)
			return (-1);
		return (1);
	}
	if (isdigit(c) || c == '-')
		return (parse_int(dst, stream));
	unexpected(stream);
	return (-1);
}

int	argo(json *dst, FILE *stream)
{
	if (!dst || !stream)
		return (-1);
	*dst = (json){0};
	if (parse_value(dst, stream) == -1)
		return (free_json(*dst), -1);
	if (peek(stream) != EOF)
		return (unexpected(stream), free_json(*dst), -1);
	return (1);
}
```

> [!IMPORTANT]
> **Bugs du REF corrigés** :
> - `get_str` utilise realloc dynamique (REF: `calloc(4096)` = overflow possible)
> - `realloc` avec pointeur temp `tmp` (REF: écrase le pointeur directement)
> - `parse_map` consomme le `}` final (REF: oublie → garbage en sortie)
> - `argo` vérifie qu'il ne reste rien après le JSON (REF: accepte du trailing)

---

## 5. `vbc`

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
| Zombies | picoshell, sandbox | Toujours `wait()`/`waitpid()` pour chaque `fork()`, même en cas d'erreur |
| `dup2` peut échouer | ft_popen | Vérifier le retour, `exit(1)` si échec |
| `timeout=0` | sandbox | Cas spécial: kill immédiat, pas d'alarm |
| `alarm(0)` | sandbox | Annuler l'alarme après waitpid réussi |
| `WUNTRACED` | sandbox | Nécessaire pour détecter SIGSTOP |
| `}` non consommé | argo parse_map | Toujours `getc` pour consommer le token fermant |
| `realloc` sans tmp | argo get_str/map | Toujours `tmp = realloc(old, ...)` |
| `-1` = erreur | vbc | Propager systématiquement, tester avant de continuer |
| `given.c` | argo, vbc | Recopier en entier d'abord, ajouter tes fonctions ensuite |
