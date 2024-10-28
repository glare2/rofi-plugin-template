#include "utils.h"
#include <unistd.h>
#include <rofi/helper.h>
#include <stdio.h>
bool starts_with(const char *str, const char *query)
{
  //if (strlen(query) > strlen(str)) return false;
  return strncmp(str, query, strlen(query)) == 0;
}

char **get_command_lines(const char *cmd){
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
