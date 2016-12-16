SUFFIXES = .c .h .y .l
ACLOCAL_AMFLAGS = -I m4
AM_YFLAGS = -d

scannerdir = $(libdir)/@project_name@/scanners
scanner_LTLIBRARIES = ox.la

ox_la_SOURCES = ox_bison.y ox_flex.l
ox_la_LDFLAGS = -module

BUILT_SOURCES = ox_bison.h
