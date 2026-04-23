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