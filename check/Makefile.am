AM_CPPFLAGS = -I$(srcdir)/../lib
if HAVE_CHECK
TESTS = check_othello
noinst_PROGRAMS = check_othello
check_othello_SOURCES = check_othello.c
check_othello_INCLUDES = @CHECK_CFLAGS@
check_othello_CFLAGS = @CHECK_CFLAGS@
check_othello_LDADD = @CHECK_LIBS@ ../lib/libhinversi.la

clean-local:
	rm -f check-othello.log
endif
