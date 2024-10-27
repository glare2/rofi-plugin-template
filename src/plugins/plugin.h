#include <rofi/mode.h>

#ifndef PLUGIN_H__
#define PLUGIN_H__
typedef struct {
  const char *name;
  void (*_init)();
  void (*_destroy)();
  int (*_token_match)(rofi_int_matcher **tokens); // return priority 0-100, or -1
  ModeMode (*_execute)(int index);
  char *(*_get_text)(int index);
  char *(*_get_icon)(int index);
  unsigned int (*_get_num_entries)();
  char *message; //rofi message the plugin wants to display
} Plugin;
#endif

