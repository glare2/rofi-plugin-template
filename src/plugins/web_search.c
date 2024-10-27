#include "web_search.h"
#include <stdbool.h>

void web_search_init()
{

}

void web_search_destroy()
{

}

int web_search_token_match(rofi_int_matcher **tokens)
{
  return true;
}

ModeMode web_search_execute(int index)
{

}

char *web_search_get_text(int index)
{
  return "Web Search?";
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
