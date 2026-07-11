#include "life.h"

static void	free_board(char **board, int height)
{
	if (!board)
		return ;
	for (int i = 0; i < height; i++)
		free(board[i]);
	free(board);
}

static char	**alloc_board(int width, int height)
{
	char	**board;

	board = malloc(height * sizeof(char *));
	if (!board)
		return (NULL);
	for (int i = 0; i < height; i++)
	{
		board[i] = malloc(width);
		if (!board[i])
			return (free_board(board, i), NULL);
		for (int j = 0; j < width; j++)
			board[i][j] = ' ';
	}
	return (board);
}

static void	draw_pen_commands(t_game *game)
{
	char	command;

	while (read(0, &command, 1) == 1)
	{
		if (command == 'w' && game->pen_row > 0)
			game->pen_row--;
		else if (command == 's' && game->pen_row < game->height - 1)
			game->pen_row++;
		else if (command == 'a' && game->pen_col > 0)
			game->pen_col--;
		else if (command == 'd' && game->pen_col < game->width - 1)
			game->pen_col++;
		else if (command == 'x')
			game->drawing = !game->drawing;
		else
			continue ;
		/* the pen also marks its cell right after an 'x' toggle,
		   not only after a move: matches the reference behavior */
		if (game->drawing)
			game->board[game->pen_row][game->pen_col] = 'O';
	}
}

static int	count_alive_neighbors(t_game *game, int row, int col)
{
	int	count;
	int	neighbor_row;
	int	neighbor_col;

	count = 0;
	for (int d_row = -1; d_row <= 1; d_row++)
	{
		for (int d_col = -1; d_col <= 1; d_col++)
		{
			if (d_row == 0 && d_col == 0)
				continue ;
			neighbor_row = row + d_row;
			neighbor_col = col + d_col;
			if (neighbor_row >= 0 && neighbor_row < game->height
				&& neighbor_col >= 0 && neighbor_col < game->width
				&& game->board[neighbor_row][neighbor_col] == 'O')
				count++;
		}
	}
	return (count);
}

static int	play_one_generation(t_game *game)
{
	char	**next_board;
	int		alive_neighbors;

	next_board = alloc_board(game->width, game->height);
	if (!next_board)
		return (-1);
	for (int row = 0; row < game->height; row++)
	{
		for (int col = 0; col < game->width; col++)
		{
			alive_neighbors = count_alive_neighbors(game, row, col);
			if (game->board[row][col] == 'O'
				&& (alive_neighbors == 2 || alive_neighbors == 3))
				next_board[row][col] = 'O';
			else if (game->board[row][col] == ' ' && alive_neighbors == 3)
				next_board[row][col] = 'O';
		}
	}
	free_board(game->board, game->height);
	game->board = next_board;
	return (0);
}

static void	print_board(t_game *game)
{
	for (int i = 0; i < game->height; i++)
	{
		for (int j = 0; j < game->width; j++)
			putchar(game->board[i][j]);
		putchar('\n');
	}
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 4)
		return (1);
	game.width = atoi(argv[1]);
	game.height = atoi(argv[2]);
	game.iterations = atoi(argv[3]);
	game.pen_row = 0;
	game.pen_col = 0;
	game.drawing = 0;
	game.board = alloc_board(game.width, game.height);
	if (!game.board)
		return (1);
	draw_pen_commands(&game);
	for (int i = 0; i < game.iterations; i++)
		if (play_one_generation(&game) == -1)
			return (free_board(game.board, game.height), 1);
	print_board(&game);
	free_board(game.board, game.height);
	return (0);
}
