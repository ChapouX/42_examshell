#include <stdio.h>
#include <ctype.h>

char	*g_s;

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");
}

int	parse_sum(void);

int	parse_factor(void)
{
	int	n;

	if (isdigit(*g_s))
		return (*g_s++ - '0');
	if (*g_s == '(')
	{
		g_s++;
		n = parse_sum();
		if (n < 0)
			return (-1);
		if (*g_s != ')')
			return (unexpected(*g_s), -1);
		g_s++;
		return (n);
	}
	return (unexpected(*g_s), -1);
}

int	parse_product(void)
{
	int	a;
	int	b;

	a = parse_factor();
	if (a < 0)
		return (-1);
	while (*g_s == '*')
	{
		g_s++;
		b = parse_factor();
		if (b < 0)
			return (-1);
		a *= b;
	}
	return (a);
}

int	parse_sum(void)
{
	int	a;
	int	b;

	a = parse_product();
	if (a < 0)
		return (-1);
	while (*g_s == '+')
	{
		g_s++;
		b = parse_product();
		if (b < 0)
			return (-1);
		a += b;
	}
	return (a);
}

int	main(int ac, char **av)
{
	int	n;

	if (ac != 2)
		return (1);
	g_s = av[1];
	if (!*g_s)
		return (unexpected(0), 1);
	n = parse_sum();
	if (n < 0)
		return (1);
	if (*g_s)
		return (unexpected(*g_s), 1);
	printf("%d\n", n);
	return (0);
}