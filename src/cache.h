#include <rofi/helper.h>
#include <stdio.h>

typedef struct {
  char *text;
  char *icon;
  char *cmd;
} CacheEntry;

typedef struct {
  CacheEntry **(*_init)(unsigned int *length);
  int (*_get_priority)(char *search_str);
  int (*_token_match)(rofi_int_matcher **tokens, unsigned int index);
  CacheEntry **array;
  unsigned int array_length;
  CacheEntry **data;
  unsigned int length;
  unsigned int max_length;
} Cache;

void cache_init(Cache *cache);
void cache_destroy(Cache *cache);
void cache_token_match(Cache *cache, Plugin *plugin, rofi_int_matcher **tokens);

#define DEFINE_PLUGIN_FUNCTION(name, func_name)	\
  .##func_name = name##_autogen##func_name,

#define DEFINE_CACHE_FUNCTION(name, func_name) \
  .##func_name = name##func_name,

// pass name as plugin prefix (i.e. apps)
// pass err_str as str (i.e. "No Apps Found")
#define DEFINE_CACHE_GETTERS(name, err_str) \
  char *name##_autogen_get_cmd(int index) { \
    if( name##_cache.data[index] != NULL && name##_cache.data[index]->cmd != NULL) { \
      return name##_cache.data[index]->cmd; \
    } \
    return NULL; \
  } \
  char *name##_autogen_get_text(int index) { \
    if( name##_cache.data[index] != NULL && name##_cache.data[index]->text != NULL) { \
      return name##_cache.data[index]->text; \
    } \
    return err_str; \
  } \
  char *name##_autogen_get_icon(int index) { \
    if( name##_cache.data[index] != NULL && name##_cache.data[index]->icon != NULL) { \
      return name##_cache.data[index]->icon; \
    } \
    return "n/a"; \
  } \
   unsigned int name##_autogen_get_num_matches() { \
     return name##_cache.length; \
   } \
   int name##_autogen_get_priority( char *search_str ) { \
     return name##_cache._get_priority( search_str );	 \
   } \
   void name##_autogen_init() { \
     cache_init( & (name##_cache) );		\
   } \
   void name##_autogen_destroy() { \
     cache_destroy( & (name##_cache) );		\
   } \
   int name##_autogen_token_match(rofi_int_matcher **tokens, unsigned int index) { \
     return name##_cache._token_match(tokens, index); \
   }

//pass plugin* reference
#define INIT_PLUGIN(name_ref, plg_name, cap_val)	\
  Plugin name_ref##_plugin = \
    { \
      ._init = name_ref##_autogen_init, \
      ._destroy = name_ref##_autogen_destroy, \
      ._token_match = name_ref##_autogen_token_match, \
      ._get_priority = name_ref##_autogen_get_priority, \
      ._get_cmd = name_ref##_autogen_get_cmd, \
      ._get_text = name_ref##_autogen_get_text, \
      ._get_icon = name_ref##_autogen_get_icon, \
      ._get_num_matches = name_ref##_autogen_get_num_matches, \
      .name = plg_name,					  \
      .message = NULL,					  \
    }; \
  Cache name_ref##_cache = \
    { \
      ._init = name_ref##_init, \
      ._get_priority = name_ref##_get_priority, \
      ._token_match = name_ref##_token_match, \
      .array = NULL,				\
      .array_length = 0, \
      .data = NULL, \
      .length = 0, \
      .max_length = cap_val \
    }

