#include "utils.h"

size_t ft_strlen(const char *s)
{
    size_t i = 0;
    while (s && s[i])
        i++;
    return i;
}

void ft_putstr_fd(const char *s, int fd)
{
    if (s)
        write(fd, s, ft_strlen(s));
}

int ft_strcmp(const char *a, const char *b)
{
    if (!a || !b)
        return 1; /* consider them different if one is NULL */
    while (*a && *b && (*a == *b))
    {
        a++;
        b++;
    }
    return ((unsigned char)*a - (unsigned char)*b);
}