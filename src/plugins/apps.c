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

void apps_init()
{
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
	  char *app_cmd = g_malloc0( (strlen(app_info[i]) - 5) * sizeof( char ) );
	  strncpy(app_cmd, app_info[i] + 5, strlen(app_info[i]) - 5);
	  curr_app->cmd = app_cmd;
	}
	else if ( starts_with( app_info[i], "Icon=" ))
	{
	  char *app_icon = g_malloc0( (strlen(app_info[i]) - 5) * sizeof( char ) );
	  strncpy(app_icon, app_info[i] + 5, strlen(app_info[i]) - 5);
	  curr_app->icon = app_icon;
	}
	else if ( starts_with( app_info[i], "Name=" ))
	{
	  char *app_name = g_malloc0( (strlen(app_info[i]) - 5) * sizeof( char ) );
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
    apps_length ++;
    if ( apps_length == apps_capacity )
    {
      apps_capacity += 16;
      apps_array = g_realloc( apps_array, apps_capacity * sizeof( App * ) );
    }
  }

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

int apps_token_match(rofi_int_matcher **tokens)
{
  //helper_token_match
  
  return true;
}

ModeMode apps_execute(int index)
{
  if( apps_array[0] != NULL && apps_array[0]->cmd != NULL)
  {
    helper_execute_command(NULL, apps_array[0]->cmd, false, NULL);
  }
  return MODE_EXIT;
}

char *apps_get_text(int index)
{
  if( apps_array[0] != NULL && apps_array[0]->name != NULL)
  {
    return apps_array[0]->name;
  }
  return "n/a";
}

char *apps_get_icon(int index)
{
  if( apps_array[0] != NULL && apps_array[0]->icon != NULL)
  {
    return apps_array[0]->icon;
  }
  return "n/a";
}

unsigned int apps_get_num_entries()
{
  return 1;
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
    .message = NULL
  };
