/**
 * rofi-plugin-template
 *
 * MIT/X11 License
 * Copyright (c) 2017 Qball Cow <qball@gmpclient.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

void icon_map_set(char *key, uint32_t value);
uint32_t icon_map_get(char *key);

static int world_mode_init( Mode *sw );
static unsigned int world_mode_get_num_entries( const Mode *sw );
int get_plugin_index(unsigned int sel_line, int *plugin);
static ModeMode world_mode_result( Mode *sw, int mretv, char **input, unsigned int sel_line);
static void world_mode_destroy( Mode *sw );
static char *world_mode_get_display_value(const Mode *sw, unsigned int sel_line, G_GNUC_UNUSED int *state, G_GNUC_UNUSED GList **attr_list, int get_entry);
static cairo_surface_t *world_mode_get_icon(const Mode *sw, unsigned int sel_line, int height);
int world_priority_compare(const void *a, const void *b);
void world_update_priorities(rofi_int_matcher **tokens);
static int world_mode_token_match( const Mode *sw, rofi_int_matcher **tokens, unsigned int sel_line);
