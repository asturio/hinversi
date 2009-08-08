prefix=/usr/local
exec_prefix=${prefix}/games
libdir=${prefix}/lib
includedir=${prefix}/include

CFLAGS=-Wall -ggdb -L. -DGTK_DISABLE_DEPRECATED
PKG_CONFIG_PATH=.

%.o: %.c Makefile
	@gcc $(CFLAGS) -fPIC -c $<

gui: gtkgui.c
	gcc ${CFLAGS} `pkg-config gtk+-2.0 --cflags --libs` -o gui gtkgui.c

clean:
	rm -f gui
