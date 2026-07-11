#ifndef LIFE_H
#define LIFE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_game
{
	int		width;
	int		height;
	int		iterations;
	int		pen_row;
	int		pen_col;
	int		drawing;
	char	**board;
}	t_game;

#endif
