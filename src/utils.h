#include <stdbool.h>
#include <rofi/mode.h>

char *get_str_from_tokens(rofi_int_matcher **tokens);
char *read_until_newline(int fd);
bool starts_with(const char *str, const char *query);
char **get_command_lines(const char *cmd);
