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

#include <stdint.h>

G_MODULE_EXPORT Mode mode;

typedef struct
{
  char *text;
  char *icon;

  uint32_t icon_fetcher_request;
} Entry;

/**
 * The internal data structure holding the private data of the TEST Mode.
 */
typedef struct
{
  Entry *array;
  unsigned int array_length;
} WorldModePrivateData;

static void push_array(WorldModePrivateData *pd, const char *text, const char *icon)
{
  pd->array = g_realloc ( pd->array, (pd->array_length + 1) * sizeof(Entry) );
  pd->array[ pd->array_length ].text = g_strdup( text );
  pd->array[ pd->array_length ].icon = g_strdup( icon );
  pd->array[ pd->array_length ].icon_fetcher_request = 0;
  pd->array_length ++;
}

static void get_world (  Mode *sw )
{
  /** 
   * Get the entries to display.
   * this gets called on plugin initialization.
   */
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );
  push_array(pd, "Example text that is searchable", "~/Pictures/scr/cursor.png");
}


static int world_mode_init ( Mode *sw )
{
  /**
   * Called on startup when enabled (in modi list)
   */
  if ( mode_get_private_data ( sw ) == NULL ) {
    WorldModePrivateData *pd = g_malloc0 ( sizeof ( *pd ) );
    mode_set_private_data ( sw, (void *) pd );
    // Load content.
    get_world ( sw );
  }
  return TRUE;
}
static unsigned int world_mode_get_num_entries ( const Mode *sw )
{
  const WorldModePrivateData *pd = (const WorldModePrivateData *) mode_get_private_data ( sw );
  return pd->array_length;
}

static ModeMode world_mode_result ( Mode *sw, int mretv, char **input, unsigned int selected_line )
{
  ModeMode           retv  = MODE_EXIT;
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );
  if ( mretv & MENU_NEXT ) {
    retv = NEXT_DIALOG;
  } else if ( mretv & MENU_PREVIOUS ) {
    retv = PREVIOUS_DIALOG;
  } else if ( mretv & MENU_QUICK_SWITCH ) {
    retv = ( mretv & MENU_LOWER_MASK );
  } else if ( ( mretv & MENU_OK ) ) { //ENTER
    retv = MENU_CUSTOM_INPUT; //Keep alive
  } else if ( ( mretv & MENU_ENTRY_DELETE ) == MENU_ENTRY_DELETE ) {
    retv = RELOAD_DIALOG;
  }
  return retv;
}

static void free_array ( WorldModePrivateData *pd )
{
  for ( unsigned int i = 0; i < pd->array_length; i++ )
  {
    Entry *entry = & ( pd->array[i] );
    g_free ( entry->text );
    g_free ( entry->icon );
  }
  g_free ( pd-> array );
  pd->array = NULL;
  pd->array_length = 0;
}

static void world_mode_destroy ( Mode *sw )
{
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );
  if ( pd != NULL ) {
    free_array ( pd );
    g_free ( pd );
    mode_set_private_data ( sw, NULL );
  }
}

/*
static void replace_array(const WorldModePrivateData *pd, const char *text, const char *icon)
{
  free_array( pd );
  
  }*/

static char *_get_display_value ( const Mode *sw, unsigned int selected_line, G_GNUC_UNUSED int *state, G_GNUC_UNUSED GList **attr_list, int get_entry )
{
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );

  // Only return the string if requested, otherwise only set state.
  if ( !get_entry ) return NULL;
  return g_strdup( pd->array[selected_line].text );
}

static cairo_surface_t *_get_icon ( const Mode *sw, unsigned int selected_line, int height )
{
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );
  Entry *entry = & pd->array[selected_line];
  if ( entry->icon != NULL)
  {
    if ( entry->icon_fetcher_request <= 0 )
    {
      entry->icon_fetcher_request = rofi_icon_fetcher_query ( entry->icon, height );
    }
    return rofi_icon_fetcher_get ( entry->icon_fetcher_request );
  }
  return NULL;
}

/**
 * @param sw The mode object.
 * @param tokens The tokens to match against.
 * @param index  The index in this plugin to match against.
 *
 * Match the entry.
 *
 * @param returns try when a match.
 */
static int world_token_match ( const Mode *sw, rofi_int_matcher **tokens, unsigned int index )
{
  WorldModePrivateData *pd = (WorldModePrivateData *) mode_get_private_data ( sw );

  // Call default matching function.
  return helper_token_match ( tokens, pd->array[index].text);
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
    ._token_match       = world_token_match,
    ._get_display_value = _get_display_value,
    ._get_icon          = _get_icon,
    ._get_message       = NULL,
    ._get_completion    = NULL,
    ._preprocess_input  = NULL,
    .private_data       = NULL,
    .free               = NULL,
  };
