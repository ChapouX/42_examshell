#include "life.h"

int init_game(t_game* game, char* argv[])
{
	game->width = atoi(argv[1]);
	game->height = atoi(argv[2]);
	game->iterations = atoi(argv[3]);
	game->alive = 'O';
	game->dead = ' ';
	game->i = 0;
	game->j = 0;
	game->draw = 0;
	game->board = malloc((game->height) * sizeof(char *));
	if(!(game->board))
		return(-1);
	for(int i = 0; i < game->height; i++)
	{
		game->board[i] = malloc((game->width) * sizeof(char));
		if(!(game->board[i]))
			return(free_board(game), -1);
		for(int j = 0; j < game->width; j++)
			game->board[i][j] = game->dead;
	}
	return(0);
}

void fill_board(t_game* game)
{
	char c;

	while(read(STDIN_FILENO, &c, 1) == 1)
	{
		if(c == 'w')
			game->i -= (game->i > 0);
		else if(c == 's')
			game->i += (game->i < game->height - 1);
		else if(c == 'a')
			game->j -= (game->j > 0);
		else if(c == 'd')
			game->j += (game->j < game->width - 1);
		else if(c == 'x')
			game->draw = !(game->draw);
		else
			continue;
		if(game->draw)
			game->board[game->i][game->j] = game->alive;
	}
}

int count_neighbors(t_game* game, int i, int j)
{
	int count = 0;
	for(int di = -1; di < 2; di++)
	{
		for(int dj = -1; dj < 2; dj++)
		{
			if((di == 0) && (dj == 0))
				continue;

			int ni = i + di;
			int nj = j + dj;
			if((ni >= 0) && (nj >=0) && (ni < game->height) && (nj < game->width)) {
				if(game->board[ni][nj] == game->alive)
					count++;
			}
		}
	}
	return(count);
}

int play(t_game* game)
{
	char** temp = malloc((game->height) * sizeof(char *));
	if(!temp)
		return(-1);
	for(int i = 0; i < game->height; i++)
	{
		temp[i] = malloc((game->width) * sizeof(char));
		if(!(temp[i]))
			return(-1);
	}

	for(int i = 0; i < game->height; i++)
	{
		for(int j = 0; j < game->width; j++)
		{
			int n = count_neighbors(game, i, j);
			int alive = (game->board[i][j] == game->alive);

			temp[i][j] = (n == 3 || (n == 2 && alive)) ? game->alive : game->dead;
		}
	}

	free_board(game);
	game->board = temp;
	return(0);
}

void print_board(t_game* game)
{
	for(int i = 0; i < game->height; i++)
	{
		for(int j = 0; j < game->width; j++)
		{
			putchar(game->board[i][j]);
		}
		putchar('\n');
	}
}

void free_board(t_game* game)
{
	if(game->board)
	{
		for(int i = 0; i < game->height; i++)
		{
			if(game->board[i])
				free(game->board[i]);
		}
		free(game->board);
	}
}

int main(int argc, char* argv[])
{
	if(argc != 4)
		return (1);

	t_game game;

	if(init_game(&game, argv) == -1)
		return(1);

	fill_board(&game);

	for(int i = 0; i < game.iterations; i++) {
		if(play(&game) == -1) {
			free_board(&game);
			return(1);
		}
	}
	print_board(&game);
	free_board(&game);

	return (0);
}
