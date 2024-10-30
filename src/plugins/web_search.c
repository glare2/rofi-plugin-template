#include "web_search.h"
#include <stdbool.h>
#include <stdio.h>
#include <rofi/helper.h>
#include <rofi/mode.h>
#include "../utils.h"

char *web_search_query;

void web_search_init()
{
  web_search_query = g_strdup("");
}

void web_search_destroy()
{
  g_free(web_search_query);
}

int web_search_get_priority(rofi_int_matcher **tokens)
{
  char *search_str = get_str_from_tokens( tokens );
  // higher char count, spaces, punctuation, sentence structure
  int search_len = strlen( search_str );
  if ( search_len < 10 ) return 0;
  int priority = search_len * 3;
  if ( priority < 100 ) return priority;
  return 100;
}

int web_search_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  web_search_plugin.priority = web_search_get_priority( tokens );
  g_free(web_search_query);
  web_search_query = get_str_from_tokens(tokens);
  
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
    ._get_priority = web_search_get_priority,
    .message = NULL,
    .priority = 0
  };
