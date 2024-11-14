#include "plugin.h"
#include <stdbool.h>
#include <stdio.h>
#include "../cache.h"
#include "../utils.h"

#define APPS_ENTRY_COUNT 5
// /var/lib/snapd/desktop/applications
#define APPS_DIR "/usr/share/applications"
#define APPS_DEFAULT_PRIORITY 40
#define APP_CMD_KEY "Exec="
#define APP_ICON_KEY "Icon="
#define APP_NAME_KEY "Name="
#define APPS_MEM_BLOCK 16

/*
  Plugin name "apps" -- variables must be named
    Plugin apps_plugin;
    Cache apps_cache;
    CacheEntry **apps_init(unsigned int *len_out);
    int apps_get_priority(char *search_str);
    
 */

Plugin apps_plugin;
Cache apps_cache;

CacheEntry **apps_array;
unsigned int apps_length;
unsigned int apps_capacity;

CacheEntry **apps_init(unsigned int *len_out, int *cache_len_out)
{
  *cache_len_out = APPS_ENTRY_COUNT;
  apps_length = 0;
  apps_capacity = APPS_MEM_BLOCK;
  apps_array = g_malloc0( apps_capacity * sizeof( CacheEntry * ) );
  char *app_list_cmd = g_strconcat( "ls ", APPS_DIR, NULL );
  char **app_files = get_command_lines(app_list_cmd);
  g_free( app_list_cmd );
  while ( app_files[ apps_length ] != NULL )
  {
    char *info_cmd = g_strconcat( "awk '/^", APP_CMD_KEY,
				  "|^", APP_ICON_KEY,
				  "|^", APP_NAME_KEY,
				  "/' ", APPS_DIR, "/",
				 app_files[ apps_length ],
				 NULL );
    char **app_info = get_command_lines(info_cmd);
    g_free( info_cmd );
    int i = 0;
    apps_array[apps_length] = g_malloc0( sizeof(CacheEntry) );
    CacheEntry *curr_app = apps_array[apps_length];
    curr_app->cmd = NULL;
    curr_app->icon = NULL;
    curr_app->text = NULL;
    int max_app_key_len = strlen(APP_CMD_KEY);
    if (strlen(APP_NAME_KEY) > max_app_key_len)
    {
      max_app_key_len = strlen(APP_NAME_KEY);
    }
    if (strlen(APP_ICON_KEY) > max_app_key_len)
    {
      max_app_key_len = strlen(APP_ICON_KEY);
    }
    while ( app_info[i] != NULL )
    {
      if ( strlen( app_info[i] ) > max_app_key_len)
      {
	if ( starts_with( app_info[i], APP_CMD_KEY ) && curr_app->cmd == NULL )
	{
	  char *app_cmd = NULL;
	  int cmd_len = str_len_to_char(app_info[i] + strlen(APP_CMD_KEY), ' ');
	  if ( cmd_len == 0 )
	  {
	    printf("error assigning cmd!\n(L) %s\n(F) %s\n", app_info[i], app_files[apps_length]);
	  }
	  else if ( cmd_len > 0 ) // cut off any args
	  { 
	    app_cmd = g_malloc0( cmd_len * sizeof( char ) );
	    strncpy(app_cmd, app_info[i] + strlen(APP_CMD_KEY), cmd_len);
	  }
	  else // no args given to cut off
	  {
	    app_cmd = g_malloc0( (strlen(app_info[i]) - strlen(APP_CMD_KEY) + 1) * sizeof( char ) );
	    strncpy(app_cmd, app_info[i] + strlen(APP_CMD_KEY), strlen(app_info[i]) - strlen(APP_CMD_KEY));
	  }
	  curr_app->cmd = app_cmd;
	}
	else if ( starts_with( app_info[i], APP_ICON_KEY ) && curr_app->icon == NULL )
	{
	  char *app_icon = g_malloc0( (strlen(app_info[i]) - strlen(APP_ICON_KEY) + 1) * sizeof( char ) );
	  strncpy(app_icon, app_info[i] + strlen(APP_ICON_KEY), strlen(app_info[i]) - strlen(APP_ICON_KEY));
	  curr_app->icon = app_icon;
	}
	else if ( starts_with( app_info[i], APP_NAME_KEY ) && curr_app->text == NULL)
	{
	  char *app_name = g_malloc0( (strlen(app_info[i]) - strlen(APP_NAME_KEY) + 1) * sizeof( char ) );
	  strncpy(app_name, app_info[i] + strlen(APP_NAME_KEY), strlen(app_info[i]) - strlen(APP_NAME_KEY));
	  curr_app->text = app_name;
	}
      }
      g_free( app_info[i] );
      i ++;
      
    }
    g_free( app_info );
    g_free( app_files[ apps_length ] );
    
    apps_length ++;
    if ( apps_length == apps_capacity )
    {
      apps_capacity += APPS_MEM_BLOCK;
      apps_array = g_realloc( apps_array, apps_capacity * sizeof( CacheEntry * ) );
    }
  }
  g_free( app_files );
  apps_array[ apps_length ] = NULL;

  *len_out = apps_length;
  return apps_array;
}

void apps_destroy()
{
  // nothing to destroy
}

int apps_get_priority(char *search_str)
{
  if( search_str == NULL ) return APPS_DEFAULT_PRIORITY;
  if ( apps_cache.data[0] == NULL ) return 0;
  int priority = strlen( search_str ) * 10;
  if ( priority < 100 ) return priority;
  return 100;
}

int apps_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  if ( index == 0 ) cache_token_match( &apps_cache, &apps_plugin, tokens);
  return true;
}

char *apps_get_error_text(int index)
{
  return "No Applications Found";
}

INIT_PLUGIN(apps);

