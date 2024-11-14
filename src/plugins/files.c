#include "plugin.h"
#include <stdbool.h>
#include <stdio.h>
#include "../cache.h"
#include "../utils.h"

#define FILES_ENTRY_COUNT 5
#define FILES_MIN_QUERY_LENGTH 4 // how many chars to type
#define FILES_DEFAULT_PRIORITY 0
#define FILES_FIND_CMD "plocate"
#define FILES_FIND_ARGS "--limit %d"
#define FILES_OPEN_CMD "xdg-open"

Plugin files_plugin;
Cache files_cache;
char *files_prev_query;
//^ files query to not re-run the same cmd 

//add destroy hook for the str
CacheEntry **files_init(unsigned int *len_out, int *cache_len_out)
{
  *cache_len_out = FILES_ENTRY_COUNT;
  files_prev_query = g_strdup("");
  *len_out = 0;
  CacheEntry **entries = g_malloc( FILES_ENTRY_COUNT * sizeof(CacheEntry *) );
  for ( int i=0; i < FILES_ENTRY_COUNT; i++ )
  {
    entries[i] = NULL;
  }
  return entries;
}

void files_destroy()
{
  g_free( files_prev_query );
}

int files_get_priority(char *search_str)
{
  //printf("files get pr -- clen= %d \n", files_cache.length);
  if ( search_str == NULL ) return FILES_DEFAULT_PRIORITY;
  // lower char count, slashes, file extensions
  int search_len = strlen( search_str );
  if ( search_len < FILES_MIN_QUERY_LENGTH ) return 0;
  // one file = highest, max files = lowest
  int priority = (files_cache.length * -1 + FILES_ENTRY_COUNT + 1) * 15 + 25;
  if ( priority < 100 ) return priority;
  return 100;
}

char *files_get_file_icon(char *file_path)
{
  //eventually get extension, then show icon from that.
  return g_strdup("~/Pictures/icons/cursor.png");
}

int files_token_match(rofi_int_matcher **tokens, unsigned int index)
{
  if ( tokens == NULL ) return true; // no update
  //printf("searching..\n");
  // check if str long enough to be worth querying
  char *search_str = get_str_from_tokens( tokens );
  //printf(" ss = %s\n", search_str);
  int max_strlen = strlen(files_prev_query);
  if ( strlen(search_str) > max_strlen) max_strlen = strlen(search_str);
  if ( strncmp(files_prev_query, search_str, max_strlen) == 0 )
  {
    //printf("repeat match, exited\n");
    g_free( search_str );
    return true;
  }
  g_free( files_prev_query );
  files_prev_query = search_str;
  //printf("len checl\n");
  if ( strlen( search_str ) < FILES_MIN_QUERY_LENGTH )
  {
    //files_cache.length = 1;
    return true;
  }
  cache_destroy( &files_cache ); //ERR HERE
  //printf("build locate\n");
  // locate files from tokens
  char *file_locate_args = g_strdup_printf( FILES_FIND_ARGS,
					    FILES_ENTRY_COUNT );
  char *file_locate_cmd = g_strconcat( FILES_FIND_CMD, " ",
				       file_locate_args, " ",
				       search_str,
				       NULL );
  g_free( file_locate_args );
  //printf("execing %s\n", file_locate_cmd);
  char **located_files = get_command_lines(file_locate_cmd);
  g_free( file_locate_cmd );
  files_cache.data = g_malloc0( FILES_ENTRY_COUNT * sizeof( CacheEntry *) );
  files_cache.array = g_malloc0( FILES_ENTRY_COUNT * sizeof( CacheEntry *) );
  int i = 0;
  while ( located_files[i] != NULL )
  {
    files_cache.array[i] = g_malloc0( sizeof(CacheEntry) );
    CacheEntry *curr_file = files_cache.array[i];
    curr_file->cmd = g_strconcat( FILES_OPEN_CMD, " ",
				  located_files[i],
				  NULL );
    curr_file->icon = files_get_file_icon( located_files[i] );
    curr_file->text = located_files[i];
    files_cache.data[i] = files_cache.array[i];

    //g_free( located_files[i] );
    i ++;
  }
  files_cache.length = i;
  files_cache.array_length = i;
  if ( files_cache.length == 0 ) files_cache.length = 1;
  //printf("files cache num entries: %d\n", files_cache.length);
  g_free( located_files );
  
  return true;
}

char *files_get_error_text(int index)
{
  // add msg for "not enough chars entered"
  return "No Files Found";
}

//in the below autogen, No Files Found is getting replaced
//with garbage, which may be related to crashing
INIT_PLUGIN(files);
