typedef struct {
  char *key;
  uint32_t value;
} Pair;

typedef struct {
  Pair **entries;
  unsigned int size;
  unsigned int capacity;
  void (*set)(char *key, uint32_t value);
  uint32_t (*get)(char *key);
} Map;

void icon_map_set(char *key, uint32_t value);
uint32_t icon_map_get(char *key);
Pair *create_pair(const char *key, uint32_t value);
void free_pair(Pair *pair);
void map_resize(Map *map);
void map_init(Map *map);
void map_free(Map *map);
uint32_t map_get(Map *map, const char *key);
void map_set(Map *map, const char *key, uint32_t value);
