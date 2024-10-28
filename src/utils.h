#include <stdbool.h>

char *read_until_newline(int fd);
bool starts_with(const char *str, const char *query);
char **get_command_lines(const char *cmd);
