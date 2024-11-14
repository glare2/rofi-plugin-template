#include <stdbool.h>
#include <rofi/mode.h>

// https://stackoverflow.com/a/40591483
#define STR_IMPL_(x) #x      //stringify argument
#define STR(x) STR_IMPL_(x)  //indirection to expand argument macros


int str_len_to_char(char *search_str, char c);
void rofi_view_reload(void); //view.h dne
bool utils_token_match(char *search_str, char *actual_str);
char *get_str_from_tokens(rofi_int_matcher **tokens);
char *get_plaintext_from_regex(GRegex *regex);
char *read_until_newline(int fd);
bool starts_with(const char *str, const char *query);
char **get_command_lines(const char *cmd);
