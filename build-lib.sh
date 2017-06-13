#!/bin/sh

#LIBTOOL="libtool --tag=CC --silent"
#$LIBTOOL --mode=compile cc -c xslt.c
#$LIBTOOL --mode=link cc -rpath /usr/local/lib/lua/5.1 -o libxml.la xslt.lo
#mv .libs/libxml.so.0.0.0 xml.so

CFLAGS="$(pkg-config --cflags libxml-2.0 libxslt lua) -Wall -Werror -g -pg -Wno-unused-variable -Wno-unused-function"
LIBS="$(pkg-config --libs libxml-2.0 libxslt lua)"
gcc $CFLAGS -O2 -fpic -c -o xslt.o xslt.c
gcc $LIBS -O -shared -fpic -o xml.so xslt.o
