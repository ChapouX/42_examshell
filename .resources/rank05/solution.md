# 🎯 Rank 05 — Solutions Ultra-Minimales (Hack Eval)

> **Philosophie** : le code le plus court et le plus direct qui passe la moulinette.
> Pas de code "propre", pas de gestion de cas improbables. Juste ce qu'il faut.

> [!WARNING]
> **C++ obligatoire** pour le Level 1 (bigint, vect2, polyset). Pense surcharge
> d'opérateurs et forme canonique orthodoxe.

---

## 📋 Cheat Sheet Rapide

| # | Exercice | Lang | Lignes | Trick principal |
|---|----------|------|--------|-----------------|
| 1 | `bigint` | C++ | ~150 | string inversée + addition chiffre par chiffre |
| 2 | `vect2` | C++ | ~120 | Deux ints, surcharger tous les opérateurs |
| 3 | `polyset` | C++ | ~60 | Héritage multiple, juste `has()` + wrapper set |
| 4 | `bsq` | C | ~180 | DP matrix : `min(haut, gauche, diag) + 1` |
| 5 | `life` | C | ~130 | read + switch wasdx, compter voisins, double buffer |

---

# Level 1

---

## 1. `bigint` — Entier à précision arbitraire

> **Fichiers** : `bigint.hpp` `bigint.cpp` — **C++**

> [!TIP]
> **HACK** : Stocker le nombre dans un `std::string`. L'addition se fait chiffre
> par chiffre en inversant les strings. Le "digitshift" `<<` c'est ajouter des '0'
> à la fin, `>>` c'est tronquer la fin. Les comparaisons : comparer les longueurs
> d'abord, puis comparer lexicographiquement.

#### `bigint.hpp`
```cpp
#ifndef BIGINT
#define BIGINT

#include <sstream>
#include <iostream>
#include <string>

class bigint
{
	private:
		std::string str;
	public:
		bigint();
		bigint(unsigned int num);
		bigint(const bigint& source);

		std::string getStr() const;

		bigint& operator=(const bigint& source);

		bigint operator+(const bigint& other) const;
		bigint& operator+=(const bigint& other);

		bigint& operator++();
		bigint operator++(int);

		bigint operator<<(unsigned int n) const;
		bigint operator>>(unsigned int n) const;
		bigint& operator<<=(unsigned int n);
		bigint& operator>>=(unsigned int n);

		bigint operator<<(const bigint& other) const;
		bigint operator>>(const bigint& other) const;
		bigint& operator<<=(const bigint& other);
		bigint& operator>>=(const bigint& other);

		bool operator==(const bigint& other) const;
		bool operator!=(const bigint& other) const;
		bool operator<(const bigint& other) const;
		bool operator>(const bigint& other) const;
		bool operator<=(const bigint& other) const;
		bool operator>=(const bigint& other) const;
};

std::ostream& operator<<(std::ostream& output, const bigint& obj);

#endif
```

#### `bigint.cpp`
```cpp
#include "bigint.hpp"

bigint::bigint() { str = "0"; }

bigint::bigint(unsigned int num) {
	std::stringstream ss;
	ss << num;
	str = ss.str();
}

bigint::bigint(const bigint& src) { *this = src; }

bigint& bigint::operator=(const bigint& src) {
	if (this != &src)
		str = src.str;
	return *this;
}

std::string bigint::getStr() const { return str; }

static std::string rev(const std::string& s) {
	std::string r;
	for (size_t i = s.length(); i > 0; i--)
		r.push_back(s[i - 1]);
	return r;
}

static unsigned int toUint(const std::string& s) {
	std::stringstream ss(s);
	unsigned int r;
	ss >> r;
	return r;
}

bigint bigint::operator+(const bigint& o) const {
	std::string s1 = rev(str), s2 = rev(o.getStr());
	while (s1.length() < s2.length()) s1.push_back('0');
	while (s2.length() < s1.length()) s2.push_back('0');
	std::string res;
	int carry = 0;
	for (size_t i = 0; i < s1.length(); i++) {
		int sum = (s1[i] - '0') + (s2[i] - '0') + carry;
		carry = sum / 10;
		res.push_back(sum % 10 + '0');
	}
	if (carry)
		res.push_back(carry + '0');
	bigint tmp;
	tmp.str = rev(res);
	return tmp;
}

bigint& bigint::operator+=(const bigint& o) { *this = *this + o; return *this; }
bigint& bigint::operator++() { *this = *this + bigint(1); return *this; }
bigint bigint::operator++(int) { bigint t = *this; ++(*this); return t; }

bigint bigint::operator<<(unsigned int n) const {
	bigint t = *this;
	t.str.insert(t.str.end(), n, '0');
	return t;
}

bigint bigint::operator>>(unsigned int n) const {
	bigint t = *this;
	if (n >= t.str.length()) t.str = "0";
	else t.str.erase(t.str.length() - n, n);
	return t;
}

bigint& bigint::operator<<=(unsigned int n) { *this = *this << n; return *this; }
bigint& bigint::operator>>=(unsigned int n) { *this = *this >> n; return *this; }

bigint bigint::operator<<(const bigint& o) const { return *this << toUint(o.str); }
bigint bigint::operator>>(const bigint& o) const { return *this >> toUint(o.str); }
bigint& bigint::operator<<=(const bigint& o) { *this = *this << toUint(o.str); return *this; }
bigint& bigint::operator>>=(const bigint& o) { *this = *this >> toUint(o.str); return *this; }

bool bigint::operator==(const bigint& o) const { return str == o.str; }
bool bigint::operator!=(const bigint& o) const { return !(*this == o); }
bool bigint::operator<(const bigint& o) const {
	if (str.length() != o.str.length()) return str.length() < o.str.length();
	return str < o.str;
}
bool bigint::operator>(const bigint& o) const { return !(*this < o) && !(*this == o); }
bool bigint::operator<=(const bigint& o) const { return *this < o || *this == o; }
bool bigint::operator>=(const bigint& o) const { return *this > o || *this == o; }

std::ostream& operator<<(std::ostream& out, const bigint& obj) {
	out << obj.getStr();
	return out;
}
```

> **~150 lignes.** L'addition se fait sur les strings inversées avec carry.
> Le `>` n'est PAS `!<` (sinon `==` serait `true` pour `>`). `>` = `!< && !=`.

---

## 2. `vect2` — Vecteur 2D avec opérateurs

> **Fichiers** : `vect2.hpp` `vect2.cpp` — **C++**

> [!TIP]
> **HACK** : Deux `int` x/y, surcharger TOUT : `+`, `-`, `*`, `+=`, `-=`, `*=`,
> `++`/`--` (pre/post), `[]` (const + non-const), `-` unaire, `==`/`!=`,
> `<<` stream, et `int * vect2` (non-member).

#### `vect2.hpp`
```cpp
#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2 {
	private:
		int x, y;
	public:
		vect2();
		vect2(int a, int b);
		vect2(const vect2& src);
		vect2& operator=(const vect2& src);
		~vect2();

		int operator[](int i) const;
		int& operator[](int i);

		vect2 operator-() const;
		vect2 operator+(const vect2& o) const;
		vect2 operator-(const vect2& o) const;
		vect2 operator*(int n) const;

		vect2& operator+=(const vect2& o);
		vect2& operator-=(const vect2& o);
		vect2& operator*=(int n);

		vect2& operator++();
		vect2 operator++(int);
		vect2& operator--();
		vect2 operator--(int);

		bool operator==(const vect2& o) const;
		bool operator!=(const vect2& o) const;
};

vect2 operator*(int n, const vect2& o);
std::ostream& operator<<(std::ostream& os, const vect2& o);

#endif
```

#### `vect2.cpp`
```cpp
#include "vect2.hpp"

vect2::vect2() : x(0), y(0) {}
vect2::vect2(int a, int b) : x(a), y(b) {}
vect2::vect2(const vect2& s) { *this = s; }
vect2& vect2::operator=(const vect2& s) {
	if (this != &s) { x = s.x; y = s.y; }
	return *this;
}
vect2::~vect2() {}

int vect2::operator[](int i) const { return i == 0 ? x : y; }
int& vect2::operator[](int i) { return i == 0 ? x : y; }

vect2 vect2::operator-() const { return vect2(-x, -y); }
vect2 vect2::operator+(const vect2& o) const { return vect2(x + o.x, y + o.y); }
vect2 vect2::operator-(const vect2& o) const { return vect2(x - o.x, y - o.y); }
vect2 vect2::operator*(int n) const { return vect2(x * n, y * n); }

vect2& vect2::operator+=(const vect2& o) { x += o.x; y += o.y; return *this; }
vect2& vect2::operator-=(const vect2& o) { x -= o.x; y -= o.y; return *this; }
vect2& vect2::operator*=(int n) { x *= n; y *= n; return *this; }

vect2& vect2::operator++() { x++; y++; return *this; }
vect2 vect2::operator++(int) { vect2 t = *this; ++(*this); return t; }
vect2& vect2::operator--() { x--; y--; return *this; }
vect2 vect2::operator--(int) { vect2 t = *this; --(*this); return t; }

bool vect2::operator==(const vect2& o) const { return x == o.x && y == o.y; }
bool vect2::operator!=(const vect2& o) const { return !(*this == o); }

vect2 operator*(int n, const vect2& o) { return o * n; }
std::ostream& operator<<(std::ostream& os, const vect2& o) {
	os << "{" << o[0] << ", " << o[1] << "}";
	return os;
}
```

> **~120 lignes (hpp+cpp).** Le piège c'est le `operator*(int, vect2)` en non-member
> pour que `3 * v` compile. Et les deux versions de `[]` (const pour lecture, ref pour écriture).

---

## 3. `polyset` — Héritage multiple : searchable bags + set

> **Fichiers à rendre** : `searchable_array_bag.hpp/cpp`, `searchable_tree_bag.hpp/cpp`, `set.hpp/cpp`

> [!TIP]
> **HACK** : Les classes `bag`, `searchable_bag`, `array_bag`, `tree_bag` sont DONNÉES.
> Tu écris juste les wrappers `searchable_*_bag` (hériter des deux parents + implémenter `has()`)
> et le `set` (wrapper sur `searchable_bag&` qui check `has()` avant `insert()`).

> [!IMPORTANT]
> **Héritage virtuel** : `bag` est hérité virtuellement par `array_bag` et `searchable_bag`,
> sinon le diamant cause des ambiguïtés. C'est déjà fait dans le code donné.

#### `searchable_array_bag.hpp`
```cpp
#pragma once

#include "array_bag.hpp"
#include "searchable_bag.hpp"

class searchable_array_bag : public array_bag, public searchable_bag {
	public:
		searchable_array_bag();
		searchable_array_bag(const searchable_array_bag& src);
		searchable_array_bag& operator=(const searchable_array_bag& src);
		~searchable_array_bag();
		bool has(int) const;
};
```

#### `searchable_array_bag.cpp`
```cpp
#include "searchable_array_bag.hpp"

searchable_array_bag::searchable_array_bag() {}
searchable_array_bag::searchable_array_bag(const searchable_array_bag& s)
	: array_bag(s) {}
searchable_array_bag& searchable_array_bag::operator=(const searchable_array_bag& s) {
	if (this != &s) array_bag::operator=(s);
	return *this;
}
searchable_array_bag::~searchable_array_bag() {}
bool searchable_array_bag::has(int val) const {
	for (int i = 0; i < size; i++)
		if (data[i] == val) return true;
	return false;
}
```

#### `searchable_tree_bag.hpp`
```cpp
#pragma once

#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag {
	private:
		bool search(node* n, int val) const;
	public:
		searchable_tree_bag();
		searchable_tree_bag(const searchable_tree_bag& src);
		searchable_tree_bag& operator=(const searchable_tree_bag& src);
		~searchable_tree_bag();
		bool has(int) const;
};
```

#### `searchable_tree_bag.cpp`
```cpp
#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag() {}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& s)
	: tree_bag(s) {}
searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag& s) {
	if (this != &s) tree_bag::operator=(s);
	return *this;
}
searchable_tree_bag::~searchable_tree_bag() {}
bool searchable_tree_bag::search(node* n, int val) const {
	if (!n) return false;
	if (n->value == val) return true;
	if (val < n->value) return search(n->l, val);
	return search(n->r, val);
}
bool searchable_tree_bag::has(int val) const { return search(tree, val); }
```

#### `set.hpp`
```cpp
#pragma once

#include "searchable_bag.hpp"

class set {
	private:
		searchable_bag& bag;
	public:
		set() = delete;
		set(const set&) = delete;
		set& operator=(const set&) = delete;
		set(searchable_bag& b);
		~set();
		bool has(int) const;
		void insert(int);
		void insert(int*, int);
		void print() const;
		void clear();
		const searchable_bag& get_bag();
};
```

#### `set.cpp`
```cpp
#include "set.hpp"

set::set(searchable_bag& b) : bag(b) {}
set::~set() {}
bool set::has(int v) const { return bag.has(v); }
void set::insert(int v) { if (!has(v)) bag.insert(v); }
void set::insert(int* arr, int sz) { for (int i = 0; i < sz; i++) insert(arr[i]); }
void set::print() const { bag.print(); }
void set::clear() { bag.clear(); }
const searchable_bag& set::get_bag() { return bag; }
```

> **~60 lignes à écrire** (6 fichiers). Les `searchable_*_bag` sont des wrappers
> quasi-vides : constructeurs forwarding + `has()`. Le `set` c'est juste `has()` avant `insert()`.

---

# Level 2

---

## 4. `bsq` — Plus grand carré sur une map

> **Fichiers** : `*.c` `*.h` — **Autorisé** : `malloc`, `calloc`, `realloc`, `free`, `fopen`, `fclose`, `getline`, `fscanf`, `fputs`, `fprintf`, `stderr`, `stdout`, `stdin`, `errno`

> [!TIP]
> **HACK** : Programmation dynamique classique. Matrice de même taille que la map.
> Chaque case = `min(haut, gauche, diag) + 1` si pas obstacle. Traquer le max
> au fur et à mesure. Flashback du piscine C.

#### `bsq.h`
```c
#ifndef BSQ_H
#define BSQ_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

typedef struct s_map
{
	char	**grid;
	int		w;
	int		h;
	char	empty;
	char	obs;
	char	full;
}	t_map;

int	execute_bsq(FILE *f);

#endif
```

#### `bsq.c`
```c
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
```

> **~180 lignes.** La DP matrix est l'algo classique. `fscanf` pour la première ligne,
> `getline` pour le reste. Validation : longueur constante, caractères valides, pas de doublons.

---

## 5. `life` — Jeu de la Vie

> **Fichiers** : `*.c` `*.h` — **Autorisé** : `atoi`, `read`, `putchar`, `malloc`, `calloc`, `realloc`, `free`

> [!TIP]
> **HACK** : `read(0, &c, 1)` pour les commandes wasdx. Grille `char**` allouée
> en malloc. Le jeu de la vie c'est un double buffer : on copie la grille, on applique
> les règles, on swap. Les cellules hors limites sont mortes.

#### `life.h`
```c
#ifndef LIFE_H
#define LIFE_H

#include <stdlib.h>
#include <unistd.h>

typedef struct s_game
{
	int		w;
	int		h;
	int		iter;
	int		ci;
	int		cj;
	int		draw;
	char	**board;
}	t_game;

#endif
```

#### `life.c`
```c
#include "life.h"

static void	free_board(char **b, int h)
{
	int	i;

	i = -1;
	if (b)
	{
		while (++i < h)
			free(b[i]);
		free(b);
	}
}

static char	**alloc_board(int w, int h)
{
	char	**b;
	int		i;
	int		j;

	b = malloc(h * sizeof(char *));
	if (!b)
		return (NULL);
	i = -1;
	while (++i < h)
	{
		b[i] = malloc(w);
		if (!b[i])
			return (free_board(b, i), NULL);
		j = -1;
		while (++j < w)
			b[i][j] = ' ';
	}
	return (b);
}

static void	fill(t_game *g)
{
	char	c;

	while (read(0, &c, 1) == 1)
	{
		if (c == 'w' && g->ci > 0)
			g->ci--;
		else if (c == 's' && g->ci < g->h - 1)
			g->ci++;
		else if (c == 'a' && g->cj > 0)
			g->cj--;
		else if (c == 'd' && g->cj < g->w - 1)
			g->cj++;
		else if (c == 'x')
			g->draw = !g->draw;
		if (g->draw && c != 'x')
			g->board[g->ci][g->cj] = 'O';
	}
}

static int	neighbors(t_game *g, int r, int c)
{
	int	n;
	int	di;
	int	dj;
	int	ni;
	int	nj;

	n = 0;
	di = -2;
	while (++di < 2)
	{
		dj = -2;
		while (++dj < 2)
		{
			if (di == 0 && dj == 0)
				continue ;
			ni = r + di;
			nj = c + dj;
			if (ni >= 0 && ni < g->h && nj >= 0 && nj < g->w
				&& g->board[ni][nj] == 'O')
				n++;
		}
	}
	return (n);
}

static int	step(t_game *g)
{
	char	**nb;
	int		i;
	int		j;
	int		n;

	nb = alloc_board(g->w, g->h);
	if (!nb)
		return (-1);
	i = -1;
	while (++i < g->h)
	{
		j = -1;
		while (++j < g->w)
		{
			n = neighbors(g, i, j);
			if (g->board[i][j] == 'O' && (n == 2 || n == 3))
				nb[i][j] = 'O';
			else if (g->board[i][j] == ' ' && n == 3)
				nb[i][j] = 'O';
		}
	}
	free_board(g->board, g->h);
	g->board = nb;
	return (0);
}

static void	print_board(t_game *g)
{
	int	i;
	int	j;

	i = -1;
	while (++i < g->h)
	{
		j = -1;
		while (++j < g->w)
			putchar(g->board[i][j]);
		putchar('\n');
	}
}

int	main(int ac, char **av)
{
	t_game	g;
	int		i;

	if (ac != 4)
		return (1);
	g.w = atoi(av[1]);
	g.h = atoi(av[2]);
	g.iter = atoi(av[3]);
	g.ci = 0;
	g.cj = 0;
	g.draw = 0;
	g.board = alloc_board(g.w, g.h);
	if (!g.board)
		return (1);
	fill(&g);
	i = -1;
	while (++i < g.iter)
		if (step(&g) == -1)
			return (free_board(g.board, g.h), 1);
	print_board(&g);
	free_board(g.board, g.h);
	return (0);
}
```

> **~130 lignes.** `fill()` = boucle read avec switch wasdx. `step()` = double buffer.
> Règles de vie : vivant avec 2-3 voisins → survit, mort avec 3 → naît, sinon meurt.

---

## 📝 Récap' Mémo Express

```
bigint    → string + addition inversée + carry, << = append '0', >> = truncate
vect2     → deux ints, surcharger TOUS les opérateurs, int*vect2 non-member
polyset   → héritage donné, écrire has() (scan array / BST search) + set wrapper
bsq       → fscanf header, getline grille, DP: min(haut,gauche,diag)+1, remplir le carré
life      → read wasdx, board char**, step = double buffer, 2-3 survit, 3 naît
```

## ⚡ Tips d'Exam

> [!IMPORTANT]
> - **Level 1 = C++** : forme canonique orthodoxe (constructeur, copie, `=`, destructeur).
>   Ne pas oublier de surcharger les versions `const` vs non-`const` de `[]`.
> - **Level 2 = C** : les deux exos sont des grands classiques de piscine.
> - **bigint** : L'opérateur `>` ≠ `!<` (sinon `a == b` retourne `true` pour `a > b`).
>   Faire `> = !< && !=`.
> - **polyset** : Ne PAS réécrire `insert/print/clear` — elles sont héritées.
>   `set` delete les constructeurs par défaut/copie/assignation.
> - **bsq** : `getline` inclut le `\n`, il faut le retirer. Valider que les lignes
>   ont toutes la même longueur. VLA (`int dp[h][w]`) est le hack rapide.
> - **life** : Le pen se déplace PUIS dessine. `x` toggle le mode dessin.
>   Bien utiliser un nouveau board à chaque step (sinon les voisins sont faux).
