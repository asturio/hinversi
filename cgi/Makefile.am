AM_CPPFLAGS = -I$(srcdir)/../lib -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS=-Wall -O3 -fomit-frame-pointer -funroll-loops
bin_PROGRAMS = hinversi-cgi
hinversi_cgi_SOURCES = main.c
hinversi_cgi_LDADD = ../lib/libhinversi.la 
hinversi_cgi_LDADD += $(LIBINTL)
