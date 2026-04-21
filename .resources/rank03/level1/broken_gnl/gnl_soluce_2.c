#include "get_next_line.h"

static char	*ap(char *s, char *b, ssize_t n)
{
	char	*r;
	ssize_t	i;
	ssize_t	j;

	i = 0;
	while (s && s[i])
		i++;
	r = malloc(i + n + 1);
	if (!r)
		return (free(s), NULL);
	j = 0;
	while (j < i)
	{
		r[j] = s[j];
		j++;
	}
	i = 0;
	while (i < n)
	{
		r[j + i] = b[i];
		i++;
	}
	r[j + i] = 0;
	free(s);
	return (r);
}

char	*get_next_line(int fd)
{
	static char		b[BUFFER_SIZE > 0 ? BUFFER_SIZE : 1];
	static ssize_t	p;
	static ssize_t	n;
	char			*l;
	ssize_t			i;
	ssize_t			s;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	l = NULL;
	while (1)
	{
		if (p >= n)
		{
			n = read(fd, b, BUFFER_SIZE);
			p = 0;
			if (n <= 0)
				break ;
		}
		i = p;
		while (p < n && b[p] != '\n')
			p++;
		s = p - i + (p < n && b[p] == '\n');
		if (s)
		{
			l = ap(l, b + i, s);
			if (!l)
				return (NULL);
		}
		if (p < n && b[p] == '\n')
			return (p++, l);
	}
	if (n < 0)
		return (free(l), NULL);
	return (l);
}