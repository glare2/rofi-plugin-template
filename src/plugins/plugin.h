#include <rofi/helper.h>

#ifndef PLUGIN_H__
#define PLUGIN_H__
typedef struct {
  const char *name;
  void (*_init)();
  void (*_destroy)();
  int (*_token_match)(rofi_int_matcher **tokens, unsigned int index);
  int (*_get_priority)(char *search_str); // return priority 0-100, or -1
  char *(*_get_cmd)(int index);
  char *(*_get_text)(int index);
  char *(*_get_icon)(int index);
  unsigned int (*_get_num_matches)();
  char *message; //rofi message the plugin wants to display
  int priority;
} Plugin;
#endif

