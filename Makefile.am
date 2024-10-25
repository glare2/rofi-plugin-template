ACLOCAL_AMFLAGS=-I m4
plugindir=${libdir}/rofi/

plugin_LTLIBRARIES = world.la

myplugin_la_SOURCES=\
		 src/world.c
		 $(wildcard src/plugins/*.c)

myplugin_la_CFLAGS= @glib_CFLAGS@ @rofi_CFLAGS@
myplugin_la_LIBADD= @glib_LIBS@ @rofi_LIBS@
myplugin_la_LDFLAGS= -module -avoid-version
