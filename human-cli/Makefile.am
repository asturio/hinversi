AM_CPPFLAGS = -I$(srcdir)/../lib -DLOCALEDIR=\"$(localedir)\"
AM_CFLAGS=-Wall -O3 -fomit-frame-pointer -funroll-loops
bin_PROGRAMS = hinversi-cli
hinversi_cli_SOURCES = human-cli.h main.h human-cli.c main.c
hinversi_cli_LDADD = ../lib/libhinversi.la 
hinversi_cli_LDADD += $(LIBINTL)
