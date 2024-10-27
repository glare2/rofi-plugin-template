#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gio/gio.h>
#include "map.h"


Pair *create_pair(const char *key, uint32_t value)
{
  Pair *pair = g_malloc(sizeof(Pair));
  pair->key = g_strdup(key);
  pair->value = value;
  return pair;
}

void free_pair(Pair *pair)
{
  g_free(pair->key);
  //g_free(pair->value);
  g_free(pair);
}

void map_resize(Map *map)
{
  if (map->size >= map->capacity)
  {
    map->capacity += 4;
    map->entries = g_realloc(map->entries, map->capacity * sizeof(Pair *));
  }
}

void map_init(Map *map)
{
  map->size = 0;
  map->capacity = 4;
  map->entries = g_malloc(map->capacity * sizeof(Pair *));
}

void map_free(Map *map)
{
  for (int i=0; i<map->size; i++)
  {
    free_pair(map->entries[i]);
  }
  g_free(map->entries);
}

void map_set(Map *map, const char *key, uint32_t value)
{
  map_resize(map); // resize for potential insertion
  int left = 0, right = map->size;
  while (left < right)
  {
    int mid = (left + right) / 2;
    int cmp = strcmp(map->entries[mid]->key, key);
    if (cmp == 0)
    {
      //g_free(map->entries[mid]->value);
      map->entries[mid]->value = value;
      return;
    }
    else if (cmp < 0)
    {
      left = mid + 1;
    }
    else
    {
      right = mid;
    }
  }

  //key dne, shift and insert
  int insert_index = left;
  for (int i = map->size; i > insert_index; i--)
  {
    map->entries[i] = map->entries[i - 1];
  }
  map->entries[insert_index] = create_pair(key, value);
  map->size ++;
}

uint32_t map_get(Map *map, const char *key)
{
  int left = 0, right = map->size;
  while (left < right)
  {
    int mid = (left + right) / 2;
    int cmp = strcmp(map->entries[mid]->key, key);
    if (cmp == 0)
    {
      return map->entries[mid]->value;
    }
    else if (cmp < 0)
    {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return 0; //dne
}
