/**
 * rofi-plugin-template
 *
 * MIT/X11 License
 * Copyright (c) 2017 Qball Cow <qball@gmpclient.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gmodule.h>

#include <rofi/mode.h>
#include <rofi/helper.h>
#include <rofi/mode-private.h>
#include <rofi/rofi-icon-fetcher.h>

#include <stdbool.h>
#include <stdint.h>
#include "map.h"

#include "plugins/plugin.h"
#include "plugins/web_search.h"
#include "plugins/apps.h"

Plugin *world_plugins[] = {
  &web_search_plugin,
  &apps_plugin,
  NULL };

int world_plugins_length;

G_MODULE_EXPORT Mode mode;

Map icon_map =
{
  .entries = NULL,
  .get = icon_map_get,
  .set = icon_map_set
};

void icon_map_set(char *key, uint32_t value)
{
  map_set(&icon_map, key, value);
}

uint32_t icon_map_get(char *key)
{
  return map_get(&icon_map, key);
}

static int world_mode_init ( Mode *sw )
{
  world_plugins_length = sizeof(world_plugins) / sizeof(world_plugins[0]) - 1;
  map_init(&icon_map); 
  int i = 0;
  while ( world_plugins[i] != NULL )
  {
    world_plugins[i]->_init();
    i ++;
  }

  return true;
}

static unsigned int world_mode_get_num_entries ( const Mode *sw )
{
  int i = 0;
  int n = 0;
  while ( world_plugins[i] != NULL )
  {
    n += world_plugins[i]->_get_num_entries();
    i ++;
  }
  return n;
}

static ModeMode world_mode_result ( Mode *sw, int mretv, char **input, unsigned int selected_line )
{
  ModeMode retv = MODE_EXIT;

  // convert selected_line to usable index
  unsigned int i = 0;
  unsigned int n = 0;
  while ( world_plugins[i] != NULL && selected_line >= n + world_plugins[i]->_get_num_entries() )
  {
    n += world_plugins[i]->_get_num_entries();
    i ++;
  }
  if ( world_plugins[i] == NULL ) return retv;
  int plugin_index = selected_line - n;
  
  if ( mretv & MENU_NEXT ) {
    retv = NEXT_DIALOG;
  } else if ( mretv & MENU_PREVIOUS ) {
    retv = PREVIOUS_DIALOG;
  } else if ( mretv & MENU_QUICK_SWITCH ) {
    retv = ( mretv & MENU_LOWER_MASK );
  } else if ( ( mretv & MENU_OK ) ) { //ENTER
    return world_plugins[i]->_execute( plugin_index );
  } else if ( ( mretv & MENU_ENTRY_DELETE ) == MENU_ENTRY_DELETE ) {
    retv = RELOAD_DIALOG;
  }
  return retv;
}

static void world_mode_destroy ( Mode *sw )
{
  map_free(&icon_map);
  int i = 0;
  while ( world_plugins[i] != NULL )
  {
    world_plugins[i]->_destroy();
    //g_free ( world_plugins[i] );
    i ++;
  }
  //g_free ( world_plugins );
}

static char *world_mode_get_display_value ( const Mode *sw, unsigned int selected_line, G_GNUC_UNUSED int *state, G_GNUC_UNUSED GList **attr_list, int get_entry )
{
  // Only return the string if requested, otherwise only set state.
  if ( !get_entry ) return NULL;
  // convert selected_line to usable index
  unsigned int i = 0;
  unsigned int n = 0;
  while ( world_plugins[i] != NULL && selected_line >= n + world_plugins[i]->_get_num_entries() )
  {
    n += world_plugins[i]->_get_num_entries();
    i ++;
  }
  
  if ( world_plugins[i] == NULL ) return g_strdup( "n/a" );
  return g_strdup( world_plugins[i]->_get_text ( selected_line - n ) );
}

static cairo_surface_t *world_mode_get_icon ( const Mode *sw, unsigned int selected_line, int height )
{
  // convert selected_line to usable index
  unsigned int i = 0;
  unsigned int n = 0;
  while ( world_plugins[i] != NULL && selected_line >= n + world_plugins[i]->_get_num_entries() )
  {
    n += world_plugins[i]->_get_num_entries();
    i ++;
  }
  if ( world_plugins[i] == NULL ) return NULL;

  char *icon_path = world_plugins[i]->_get_icon ( selected_line - n ); 
  if ( icon_path != NULL)
  {
    uint32_t icon_fetcher_request = icon_map.get(icon_path);
    if ( icon_fetcher_request <= 0 )
    {
      icon_fetcher_request = rofi_icon_fetcher_query ( icon_path, height );
      icon_map.set(icon_path, icon_fetcher_request);
    }
    return rofi_icon_fetcher_get ( icon_fetcher_request );
  }
  return NULL;
}

int world_priority_compare(const void *a, const void *b)
{
  Plugin *plugin_a = *(Plugin **) a;
  Plugin *plugin_b = *(Plugin **) b;
  return ( plugin_b->priority - plugin_a->priority );
}

static int world_mode_token_match ( const Mode *sw, rofi_int_matcher **tokens, unsigned int selected_line )
{

  /*
    Get the plugin priorities
    Perfom sort on the plugin array using the priority array
    Now the plugins will display in order of priority
   */

  if ( selected_line == 0 )
  {
    // update priorities
    for ( int j=0; j<world_plugins_length; j++)
    {
      world_plugins[j]->priority = world_plugins[j]->_get_priority( tokens );
      printf("new priority of %s: %d\n", world_plugins[j]->name, world_plugins[j]->priority);
    }
    // sort plugins, so highest priority is on top
    qsort(world_plugins, world_plugins_length, sizeof(Plugin *), world_priority_compare);
  }
  // convert selected_line to usable index
  unsigned int i = 0;
  unsigned int n = 0;
  while ( world_plugins[i] != NULL && selected_line >= n + world_plugins[i]->_get_num_entries() )
  {
    n += world_plugins[i]->_get_num_entries();
    i ++;
  }
  int plugin_index = selected_line - n;
  // Call default matching function.
  //return helper_token_match ( tokens, pd->array[index].text);
  
  if ( world_plugins[i] != NULL ) return world_plugins[i]->_token_match( tokens, plugin_index );
  return false;
}


Mode mode =
  {
    .abi_version        = ABI_VERSION,
    .name               = "world",
    .cfg_name_key       = "display-world",
    ._init              = world_mode_init,
    ._get_num_entries   = world_mode_get_num_entries,
    ._result            = world_mode_result,
    ._destroy           = world_mode_destroy,
    ._token_match       = world_mode_token_match,
    ._get_display_value = world_mode_get_display_value,
    ._get_icon          = world_mode_get_icon,
    ._get_message       = NULL,
    ._get_completion    = NULL,
    ._preprocess_input  = NULL,
    .private_data       = NULL,
    .free               = NULL,
  };
