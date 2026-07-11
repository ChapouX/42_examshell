#include "bsq.h"

static void	free_grid(char **grid, int rows)
{
	if (!grid)
		return ;
	for (int i = 0; i < rows; i++)
		free(grid[i]);
	free(grid);
}

/* single exit point for every parsing error: free the getline buffer and
   whatever rows of the grid were already allocated, then report failure */
static int	fail(char *line, char **grid, int allocated_rows)
{
	free(line);
	free_grid(grid, allocated_rows);
	return (-1);
}

static int	load(FILE *file, t_map *map)
{
	char	*line;
	size_t	line_capacity;
	int		line_len;

	line = NULL;
	line_capacity = 0;
	if (fscanf(file, "%d %c %c %c", &map->height, &map->empty, &map->obstacle,
			&map->full) != 4 || map->height <= 0 || map->empty == map->obstacle
		|| map->empty == map->full || map->obstacle == map->full)
		return (-1);
	map->grid = malloc((map->height + 1) * sizeof(char *));
	if (!map->grid)
		return (-1);
	map->grid[map->height] = NULL;
	getline(&line, &line_capacity, file);
	for (int row = 0; row < map->height; row++)
	{
		line_len = getline(&line, &line_capacity, file);
		if (line_len == -1 || line[line_len - 1] != '\n')
			return (fail(line, map->grid, row));
		line[--line_len] = '\0';
		if (row == 0)
		{
			if (line_len <= 0)
				return (fail(line, map->grid, row));
			map->width = line_len;
		}
		else if (line_len != map->width)
			return (fail(line, map->grid, row));
		map->grid[row] = malloc(line_len + 1);
		if (!map->grid[row])
			return (fail(line, map->grid, row));
		for (int col = 0; col < line_len; col++)
		{
			if (line[col] != map->empty && line[col] != map->obstacle)
				return (fail(line, map->grid, row + 1));
			map->grid[row][col] = line[col];
		}
		map->grid[row][line_len] = '\0';
	}
	return (free(line), 0);
}

static int	min3(int up, int diag, int left)
{
	if (up < diag)
		return (up < left ? up : left);
	return (diag < left ? diag : left);
}

static void	solve(t_map *map)
{
	int	square_size[map->height][map->width];
	int	best_size;
	int	best_row;
	int	best_col;

	best_size = 0;
	best_row = 0;
	best_col = 0;
	for (int row = 0; row < map->height; row++)
	{
		for (int col = 0; col < map->width; col++)
		{
			if (map->grid[row][col] == map->obstacle)
				square_size[row][col] = 0;
			else if (row == 0 || col == 0)
				square_size[row][col] = 1;
			else
				square_size[row][col] = min3(square_size[row - 1][col],
						square_size[row - 1][col - 1],
						square_size[row][col - 1]) + 1;
			if (square_size[row][col] > best_size)
			{
				best_size = square_size[row][col];
				best_row = row - best_size + 1;
				best_col = col - best_size + 1;
			}
		}
	}
	for (int row = best_row; row < best_row + best_size; row++)
		for (int col = best_col; col < best_col + best_size; col++)
			map->grid[row][col] = map->full;
}

int	execute_bsq(FILE *file)
{
	t_map	map;

	if (load(file, &map) == -1)
		return (-1);
	solve(&map);
	for (int row = 0; row < map.height; row++)
		fprintf(stdout, "%s\n", map.grid[row]);
	free_grid(map.grid, map.height);
	return (0);
}

int	main(int argc, char **argv)
{
	FILE	*file;

	if (argc == 1)
	{
		if (execute_bsq(stdin) == -1)
			fprintf(stderr, "map error\n");
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			file = fopen(argv[i], "r");
			if (!file || execute_bsq(file) == -1)
				fprintf(stderr, "map error\n");
			if (file)
				fclose(file);
			if (i < argc - 1)
				fprintf(stdout, "\n");
		}
	}
	return (0);
}
