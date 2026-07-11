#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int		integer;
		char	*string;
	};
} json;

typedef struct pair {
	char	*key;
	json	value;
} pair;

int	peek(FILE *stream)
{
	int	c;

	c = getc(stream);
	if (c != EOF)
		ungetc(c, stream);
	return (c);
}

void	unexpected(FILE *stream)
{
	int	c;

	c = peek(stream);
	if (c == EOF)
		printf("unexpected end of input\n");
	else
		printf("unexpected token '%c'\n", c);
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
		return ((void)getc(stream), 1);
	return (0);
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return (1);
	unexpected(stream);
	return (0);
}

void	free_json(json j)
{
	size_t	i;

	if (j.type == STRING)
		free(j.string);
	if (j.type != MAP)
		return ;
	i = 0;
	while (i < j.map.size)
	{
		free(j.map.data[i].key);
		free_json(j.map.data[i].value);
		i++;
	}
	free(j.map.data);
}

void	serialize(json j)
{
	size_t	i;

	if (j.type == INTEGER)
		printf("%d", j.integer);
	else if (j.type == STRING)
	{
		printf("\"");
		i = 0;
		while (j.string[i])
		{
			if (j.string[i] == '\\' || j.string[i] == '"')
				printf("\\");
			printf("%c", j.string[i]);
			i++;
		}
		printf("\"");
	}
	else
	{
		printf("{");
		i = 0;
		while (i < j.map.size)
		{
			if (i)
				printf(",");
			serialize((json){.type = STRING, .string = j.map.data[i].key});
			printf(":");
			serialize(j.map.data[i].value);
			i++;
		}
		printf("}");
	}
}

static char	*parse_string_data(FILE *stream)
{
	char	*res;
	char	*new_res;
	int		c;
	size_t	len;

	if (!expect(stream, '"'))
		return (NULL);
	res = calloc(1, 1);
	if (!res)
		return (NULL);
	len = 0;
	while (1)
	{
		c = getc(stream);
		if (c == EOF)
			return (free(res), unexpected(stream), NULL);
		if (c == '"')
			break ;
		if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
				return (free(res), unexpected(stream), NULL);
			if (c != '\\' && c != '"')
				return (free(res), unexpected(stream), NULL);
		}
		new_res = realloc(res, len + 2);
		if (!new_res)
			return (free(res), NULL);
		res = new_res;
		res[len++] = (char)c;
		res[len] = '\0';
	}
	return (res);
}

static int	parse_number(json *dst, FILE *stream)
{
	int	sign;
	int	n;
	int	c;

	sign = 1;
	if (accept(stream, '-'))
		sign = -1;
	if (!isdigit(peek(stream)))
		return (unexpected(stream), -1);
	n = 0;
	c = peek(stream);
	while (isdigit(c))
	{
		n = n * 10 + (getc(stream) - '0');
		c = peek(stream);
	}
	dst->type = INTEGER;
	dst->integer = n * sign;
	return (1);
}

static int	parse_value(json *dst, FILE *stream);

static int	parse_map(json *dst, FILE *stream)
{
	char	*key;
	json	value;
	pair	*new_data;

	if (!expect(stream, '{'))
		return (-1);
	dst->type = MAP;
	dst->map.data = NULL;
	dst->map.size = 0;
	if (accept(stream, '}'))
		return (1);
	while (1)
	{
		if (peek(stream) != '"')
			return (free_json(*dst), unexpected(stream), -1);
		key = parse_string_data(stream);
		if (!key)
			return (free_json(*dst), -1);
		if (!expect(stream, ':'))
			return (free(key), free_json(*dst), -1);
		if (parse_value(&value, stream) == -1)
			return (free(key), free_json(*dst), -1);
		new_data = realloc(dst->map.data, sizeof(pair) * (dst->map.size + 1));
		if (!new_data)
			return (free(key), free_json(value), free_json(*dst), -1);
		dst->map.data = new_data;
		dst->map.data[dst->map.size].key = key;
		dst->map.data[dst->map.size].value = value;
		dst->map.size++;
		if (accept(stream, ','))
			continue ;
		if (accept(stream, '}'))
			return (1);
		return (free_json(*dst), unexpected(stream), -1);
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
		dst->string = parse_string_data(stream);
		if (!dst->string)
			return (-1);
		return (1);
	}
	if (c == '-' || isdigit(c))
		return (parse_number(dst, stream));
	return (unexpected(stream), -1);
}

int	argo(json *dst, FILE *stream)
{
	if (!dst || !stream)
		return (-1);
	if (parse_value(dst, stream) == -1)
		return (-1);
	if (peek(stream) != EOF)
		return (unexpected(stream), free_json(*dst), -1);
	return (1);
}

int	main(int ac, char **av)
{
	FILE	*stream;
	json	file;

	if (ac != 2)
		return (1);
	stream = fopen(av[1], "r");
	if (!stream)
		return (1);
	file.type = INTEGER;
	file.integer = 0;
	if (argo(&file, stream) != 1)
		return (free_json(file), fclose(stream), 1);
	serialize(file);
	printf("\n");
	free_json(file);
	fclose(stream);
	return (0);
}