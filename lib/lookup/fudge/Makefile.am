SUFFIXES = .c .h .y .l
ACLOCAL_AMFLAGS = -I m4
AM_YFLAGS = -d

lookupdir = $(libdir)/@project_name@/lookup
lookup_LTLIBRARIES = fudge.la

fudge_la_SOURCES = fudge_bison.y fudge_flex.l
#fudge_la_CFLAGS = -g
fudge_la_LDFLAGS = -module


BUILT_SOURCES = fudge_bison.h

#include_HEADERS = hiena/lookup_module.h
