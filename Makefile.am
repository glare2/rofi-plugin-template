ACLOCAL_AMFLAGS=-I m4
plugindir=${libdir}/rofi/

plugin_LTLIBRARIES = world.la

world_la_SOURCES=\
		 src/world.c
		 $(plugin_sources)

world_la_CFLAGS= @glib_CFLAGS@ @rofi_CFLAGS@
world_la_LIBADD= @glib_LIBS@ @rofi_LIBS@
world_la_LDFLAGS= -module -avoid-version
