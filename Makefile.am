ACLOCAL_AMFLAGS=-I m4
plugindir=${libdir}/rofi/

plugin_LTLIBRARIES = world.la

world_la_SOURCES=\
		src/map.c \
		src/utils.c \
		src/plugins/web_search.c \
		src/plugins/apps.c \
		src/world.c

world_la_CFLAGS= @glib_CFLAGS@ @rofi_CFLAGS@
world_la_LIBADD= @glib_LIBS@ @rofi_LIBS@
world_la_LDFLAGS= -module -avoid-version
