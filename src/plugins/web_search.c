#include "web_search.h"
#include <stdbool.h>
#include <stdio.h>
#include <rofi/helper.h>
#include <rofi/mode.h>

char *web_search_query;

void web_search_init()
{
  web_search_query = g_strdup("");
}

void web_search_destroy()
{
  g_free(web_search_query);
}

int web_search_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  g_free(web_search_query);
  web_search_query = NULL;
  //tokens split by spaces
  int i = 0;
  if ( tokens[i] != NULL )
  {
    const char *first_token = g_regex_get_pattern ( tokens[i]->regex );
    web_search_query = g_strdup(first_token);
    i ++;
  }
  while ( tokens[i] != NULL )
  {
    const char *token = g_regex_get_pattern ( tokens[i]->regex );
    web_search_query = g_realloc(web_search_query,
				 sizeof(char) * (strlen(web_search_query) + strlen(token) + 2));
    strcat(web_search_query, " ");
    strcat(web_search_query, token);
    i ++;
  }
  
  return true;
}

ModeMode web_search_execute(int index)
{
  char *cmd = NULL;
  cmd = g_strconcat ( "xdg-open 'https://www.google.com/search?q=",
		      web_search_query, "'", NULL);
  helper_execute_command(NULL, cmd, false, NULL);
  g_free(cmd);
  return MODE_EXIT;
}

char *web_search_get_text(int index)
{
  return web_search_query;
}

char *web_search_get_icon(int index)
{
  return "~/Pictures/scr/cursor.png";
}

unsigned int web_search_get_num_entries()
{
  return 1;
}

Plugin web_search_plugin =
  {
    .name = "web_search",
    ._init = web_search_init,
    ._destroy = web_search_destroy,
    ._token_match = web_search_token_match,
    ._execute = web_search_execute,
    ._get_text = web_search_get_text,
    ._get_icon = web_search_get_icon,
    ._get_num_entries = web_search_get_num_entries,
    .message = NULL
  };
