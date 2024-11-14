#include "utils.h"
#include <unistd.h>
#include <stdio.h>

int str_len_to_char(char *search_str, char c)
{
  for(int i=0; i<strlen(search_str); i++)
  {
    if ( search_str[i] == c ) return i;
  }
  return -1;
}
bool utils_token_match(char *search_str, char *actual_str)
{
  if ( strlen(search_str) > strlen(actual_str) ) return false;
  return starts_with(search_str, actual_str);
}

char *get_str_from_tokens(rofi_int_matcher **tokens)
{
  if ( tokens == NULL ) return NULL;
  int i = 0;
  char *final_str = NULL;
  if ( tokens[i] != NULL )
  {
    const char *first_token = get_plaintext_from_regex ( tokens[i]->regex );
    final_str = g_strdup(first_token);
    i ++;
  }
  while ( tokens[i] != NULL )
  {
    const char *token = get_plaintext_from_regex ( tokens[i]->regex );
    final_str = g_realloc(final_str,
				 sizeof(char) * (strlen(final_str) + strlen(token) + 2));
    strcat(final_str, " "); // tokens split by spaces
    strcat(final_str, token);
    i ++;
  }
  return final_str;
}

char *get_plaintext_from_regex(GRegex *regex)
{
  const char *pattern = g_regex_get_pattern ( regex );
  GString *result = g_string_new("");
  for ( int i=0; pattern[i] != '\0'; i++)
  {
    if ( pattern[i] == '\\' && pattern[i + 1] != '\0') i ++;
    g_string_append_c( result, pattern[i] );
  }
  return g_string_free( result, FALSE );
}

bool starts_with(const char *str, const char *query)
{
  //if (strlen(query) > strlen(str)) return false;
  return strncmp(str, query, strlen(query)) == 0;
}

int execute_generator(const char *cmd)
{
  int fd = -1;
  GError *error = NULL;
  //second arg is <args>
  g_spawn_async_with_pipes(NULL, NULL, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                           NULL, NULL, &fd, NULL, &error);

  if (error != NULL) {
    printf("Failed to execute: '%s'\nError: '%s'", cmd,
                                error->message);
    g_error_free(error);
    fd = -1;
  }

  return fd;
}

char **get_command_lines(const char *cmd)
{
  //printf("getting command lines for %s", cmd);
  int fd = execute_generator(cmd);
  //printf("fd returns %d", fd);
  if ( fd == -1) return NULL;

  int size = 4;
  int i = 0;
  char **lines = g_malloc0( size * sizeof( char * ) );
  char *curr_line = NULL;
  while ( (curr_line = read_until_newline(fd)) != NULL )
  {
    lines[i] = curr_line;
    i ++;

    if ( i == size )
    {
      size += 4;
      lines = g_realloc( lines, size * sizeof( char * ) );
    }
  }
  lines[i] = NULL;
  close( fd );
  return lines;
}

char *read_until_newline(int fd)
{
  char c = 'a';
  int r = 1;
  int i = 0;
  int size = 32;
  char *line = g_malloc0( size * sizeof( char ) );
  while ( r > 0 )
  {
    r = read(fd, &c, 1);
    if ( r == -1 )
    {
      g_free(line);
      return NULL;
    }
    if ( r == 0 )
    {
      if ( i == 0)
      {
	g_free(line);
	return NULL;
      }
      break;
    }
    if ( c == '\n' )
    {
      line[i] = '\0';
      return line;
    }
    
    line[i] = c;
    i ++;
    
    if ( i == size )
    {
      size += 32;
      line = g_realloc( line, size * sizeof( char ) );
    }
  }
  line[i] = '\0';
  return line;
}
