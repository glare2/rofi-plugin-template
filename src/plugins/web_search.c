#include "plugin.h"
#include <stdbool.h>
#include <stdio.h>
#include "../utils.h"

#define OPEN_LINK_CMD "xdg-open"
#define SEARCH_QUERY_PREFIX "https://www.google.com/search?q="
#define SEARCH_DEFAULT_PRIORITY 0
#define SEARCH_ICON "~/Pictures/icons/cursor.png"

Plugin web_search_plugin;
char *web_search_query;

void web_search_init()
{
  web_search_query = g_strdup("");
}

void web_search_destroy()
{
  g_free(web_search_query);
}

int web_search_get_priority(char *search_str)
{
  if ( search_str == NULL ) return SEARCH_DEFAULT_PRIORITY;
  // higher char count, spaces, punctuation, sentence structure
  int search_len = strlen( search_str );
  if ( search_len < 5 ) return 5;
  if ( search_len < 8 ) return 15;
  int priority = search_len * 3;
  if ( priority < 100 ) return priority;
  return 100;
}

int web_search_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  char *search_str = get_str_from_tokens( tokens );
  web_search_plugin.priority = web_search_get_priority( search_str );
  g_free( web_search_query );
  web_search_query = g_strdup( search_str );
  g_free( search_str );
  
  return true;
}

char *web_search_get_cmd(int index)
{
  char *cmd = NULL;
  cmd = g_strconcat ( OPEN_LINK_CMD, " '",  SEARCH_QUERY_PREFIX,
		      web_search_query, "'", NULL);
  return cmd;
}

char *web_search_get_text(int index)
{
  return web_search_query;
}

char *web_search_get_icon(int index)
{
  return SEARCH_ICON;
}

unsigned int web_search_get_num_matches()
{
  return 1;
}

Plugin web_search_plugin =
{
  .name = "web_search",
  ._init = web_search_init,
  ._destroy = web_search_destroy,
  ._token_match = web_search_token_match,
  ._get_cmd = web_search_get_cmd,
  ._get_text = web_search_get_text,
  ._get_icon = web_search_get_icon,
  ._get_num_matches = web_search_get_num_matches,
  ._get_priority = web_search_get_priority,
  .message = NULL,
};
