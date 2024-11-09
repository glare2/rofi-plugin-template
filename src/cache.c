#include "plugins/plugin.h"
#include "cache.h"
#include "utils.h"
#include <stdbool.h>

void cache_init(Cache *cache)
{
  cache->array = cache->_init( &cache->array_length );
  cache->data = g_malloc0( cache->max_length * sizeof( CacheEntry *) );
  if ( cache->array_length < cache->max_length )
  {
    cache->length = cache->array_length;
    if ( cache->length < 1 ) cache->length = 1; // CANNOT HIT ZERO!!
  }
  else
  {
    cache->length = cache->max_length;
  }
  for (int i=0; i < cache->length && i < cache->array_length; i++)
  {
    cache->data[i] = cache->array[i];
  }
}

void cache_destroy(Cache *cache)
{
  for(int i=0; i < cache->array_length; i++)
  {
    CacheEntry *curr_entry = cache->array[i];
    if ( curr_entry->text != NULL )
    {
      g_free( curr_entry->text );
      curr_entry->text = NULL;
    }
    if ( curr_entry->icon != NULL )
    {
      g_free( curr_entry->icon );
      curr_entry->icon = NULL;
    }
    if ( curr_entry->cmd != NULL )
    {
      g_free( curr_entry->cmd );
      curr_entry->cmd = NULL;
    }
    g_free( curr_entry );
    curr_entry = NULL;
  }
  g_free( cache->array );
  g_free( cache->data );
}

// for a static cache->array, call this function when index == 0
// this will update the cache's priority & update cache->data
// using rofi's helper_token_match
void cache_token_match(Cache *cache, Plugin *plugin, rofi_int_matcher **tokens)
{
  char *search_str = get_str_from_tokens( tokens );
  plugin->priority = cache->_get_priority( search_str );
  // cache apps to the cache len
  int count = 0;
  // O(n)
  for ( int i=0; i < cache->array_length; i++ )
  {
    int m = false;
    if ( cache->array[i]->text != NULL ) m = helper_token_match(tokens, cache->array[i]->text);
    if ( m == true )
    {
      cache->data[ count ] = cache->array[i];
      count ++;
      if ( count == cache->max_length ) break;
    }
  }
  cache->length = count;
  if ( cache->length == 0 ) cache->length = 1; // CANNOT HIT ZERO!!
  if ( count < cache->max_length )
  {
    for ( int i = count; i < cache->max_length; i++ )
    {
      cache->data[ i ] = NULL;
    }
  }
  g_free( search_str );
}
