AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS=-Wall -O3 -fomit-frame-pointer -funroll-loops
lib_LTLIBRARIES = libhinversi.la
libhinversi_la_SOURCES = abClient.c abClient.h aiManager.c aiManager.h \
			 naiv-client.c naiv-client.h othello.c othello.h \
			 ringbuf.c ringbuf.h simpleClient.c simpleClient.h \
			 matador.c matador.h gettext.h macros.h
pkginclude_HEADERS = abClient.h aiManager.h naiv-client.h othello.h \
			  ringbuf.h simpleClient.h macros.h matador.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libhinversi.pc

