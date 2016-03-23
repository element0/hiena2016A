bin_PROGRAMS = hienafs

hienafs_SOURCES = src/hienafs.c include/gob.c
hienafs_CPPFLAGS = $(FUSE_CFLAGS) -Iinclude
hienafs_LDFLAGS = $(FUSE_LIBS)

SUBDIRS = lib src

ACLOCAL_AMFLAGS = -I m4

