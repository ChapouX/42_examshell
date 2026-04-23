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