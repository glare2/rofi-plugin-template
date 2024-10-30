#define APPS_ENTRY_COUNT 5

#include "apps.h"
#include <stdbool.h>
#include <stdio.h>
#include <rofi/helper.h>
#include <rofi/mode.h>
#include "../utils.h"

typedef struct {
  char *name;
  char *icon;
  char *cmd;
} App;

App **apps_array;
unsigned int apps_length;
unsigned int apps_capacity;
App *apps_cache [APPS_ENTRY_COUNT];
int apps_cache_length;

void apps_init()
{
  apps_cache_length = 0;
  apps_length = 0;
  apps_capacity = 16;
  apps_array = g_malloc0( apps_capacity * sizeof( App * ) );
  char **app_files = get_command_lines("ls /usr/share/applications");
  while ( app_files[ apps_length ] != NULL )
  {
    //printf("Getting app info for %s", app_files[apps_length]);
    char *info_cmd = g_strconcat( "awk '/^Exec=|^Icon=|^Name=/' ",
				 "/usr/share/applications/",
				 app_files[ apps_length ],
				 NULL );
    char **app_info = get_command_lines(info_cmd);
    int i = 0;
    apps_array[apps_length] = g_malloc0( sizeof(App) );
    App *curr_app = apps_array[apps_length];
    curr_app->cmd = NULL;
    curr_app->icon = NULL;
    curr_app->name = NULL;
    while ( app_info[i] != NULL )
    {
      if ( strlen( app_info[i] ) > 5)
      {
	//printf("Checking %s\n", app_info[i]);
	if ( starts_with( app_info[i], "Exec=" ))
	{
	  char *app_cmd = g_malloc0( (strlen(app_info[i]) - 4) * sizeof( char ) );
	  strncpy(app_cmd, app_info[i] + 5, strlen(app_info[i]) - 5);
	  curr_app->cmd = app_cmd;
	}
	else if ( starts_with( app_info[i], "Icon=" ))
	{
	  char *app_icon = g_malloc0( (strlen(app_info[i]) - 4) * sizeof( char ) );
	  strncpy(app_icon, app_info[i] + 5, strlen(app_info[i]) - 5);
	  curr_app->icon = app_icon;
	}
	else if ( starts_with( app_info[i], "Name=" ))
	{
	  char *app_name = g_malloc0( (strlen(app_info[i]) - 4) * sizeof( char ) );
	  strncpy(app_name, app_info[i] + 5, strlen(app_info[i]) - 5);
	  //printf("Writing app_name: %s\n", app_name);
	  curr_app->name = app_name;
	}
      }
      g_free( app_info[i] );
      i ++;
      
    }
    g_free( info_cmd );
    g_free( app_info );
    g_free( app_files[ apps_length ] );
    if ( apps_cache_length < APPS_ENTRY_COUNT
	 && apps_array[apps_length]->name != NULL )
    {
      apps_cache[apps_cache_length] = apps_array[apps_length];
      apps_cache_length ++;
    }
    apps_length ++;
    if ( apps_length == apps_capacity )
    {
      apps_capacity += 16;
      apps_array = g_realloc( apps_array, apps_capacity * sizeof( App * ) );
    }
  }
  g_free( app_files );
  apps_array[ apps_length ] = NULL;
}

void apps_destroy()
{
  int i = 0;
  while ( apps_array[i] != NULL)
  {
    App *curr_app = apps_array[i];
    if ( curr_app->name != NULL )
    {
      g_free(curr_app->name);
      curr_app->name = NULL;
    }
    if ( curr_app->icon != NULL )
    {
      g_free(curr_app->icon);
      curr_app->icon = NULL;
    }
    if ( curr_app->cmd != NULL ) 
    {
      g_free(curr_app->cmd);
      curr_app->cmd = NULL;
    }
    g_free(curr_app);
    curr_app = NULL;
    i ++;
  }
  g_free( apps_array );
}

int apps_get_priority(rofi_int_matcher **tokens)
{
  char *search_str = get_str_from_tokens( tokens );
  if ( apps_cache[0] == NULL ) return 0;
  int priority = strlen( search_str ) * 10;
  if ( priority < 100 ) return priority;
  return 100;
}

int apps_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  //printf("token match %d\n", index);
  if ( index == 0 )
  {
    apps_plugin.priority = apps_get_priority( tokens );
    //printf("caching apps\n");
    //cache apps to the cache_array of len 5
    int i = 0;
    int count = 0;
    // O(n)
    while ( apps_array[i] != NULL )
    {
      int m = false;
      if ( apps_array[i]->name != NULL ) m = helper_token_match(tokens, apps_array[i]->name);
      //printf("cache read for %s returns %d\n", apps_array[i]->name, m);
      if ( m == true )
      {
	//printf("add to cache\n");
	apps_cache[ count ] = apps_array[i];
	count ++;
	if ( count == APPS_ENTRY_COUNT ) break;
      }
      i ++;
    }
    apps_cache_length = count;
    if ( apps_cache_length == 0) apps_cache_length = 1; // CANNOT HIT ZERO!!
    //printf("set apps_cache to %d\n", count);
    if ( count < APPS_ENTRY_COUNT )
    { 
      for ( int j = count; j < APPS_ENTRY_COUNT; j++ )
      {	  
	apps_cache[ j ] = NULL;
      }
    }
  }
  
  return true;
}

ModeMode apps_execute(int index)
{
  if( apps_cache[index] != NULL && apps_cache[index]->cmd != NULL)
  {
    helper_execute_command(NULL, apps_cache[index]->cmd, false, NULL);
  }
  return MODE_EXIT;
}

char *apps_get_text(int index)
{
  if( apps_cache[index] != NULL && apps_cache[index]->name != NULL)
  {
    /*
    if ( index == APPS_ENTRY_COUNT - 1 )
    {
      char *label = g_malloc0( (strlen(apps_cache[index]->name) + 32) * sizeof(char));
      sprintf(label, "%s (+%d more programs)",
	      apps_cache[index]->name,
	      apps_length - APPS_ENTRY_COUNT );
      return label;
      }*/
    return apps_cache[index]->name;
  }
  return "No Applications Found";
}

char *apps_get_icon(int index)
{
  if( apps_cache[index] != NULL && apps_cache[index]->icon != NULL)
  {
    return apps_cache[index]->icon;
  }
  return "n/a";
}

unsigned int apps_get_num_entries()
{
  return apps_cache_length;
}

Plugin apps_plugin =
  {
    .name = "apps",
    ._init = apps_init,
    ._destroy = apps_destroy,
    ._token_match = apps_token_match,
    ._execute = apps_execute,
    ._get_text = apps_get_text,
    ._get_icon = apps_get_icon,
    ._get_num_entries = apps_get_num_entries,
    ._get_priority = apps_get_priority,
    .message = NULL,
    .priority = 0
  };
