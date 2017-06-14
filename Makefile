LUA_VER ?= 5.2
CFLAGS = $(shell pkg-config --cflags libxml-2.0 libxslt lua$(LUA_VER)) -Wall -Werror -g -Wno-unused-variable -Wno-unused-function -O0
# Option -pg causes exit on SIGPROF under Ubuntu
LIBS = $(shell pkg-config --libs libxml-2.0 libxslt lua$(LUA_VER))

all: xslt

run: all
	./xslt test.xml test.xsl

check: all
	valgrind --suppressions=suppressions --leak-check=yes --show-leak-kinds=all --track-origins=yes --error-exitcode=119 ./xslt test.xml test.xsl

travischeck: all
	# Not supported on Travis CI: --show-leak-kinds=all 
	# Travis doesn't like the suppressions file:
	# ==6150== FATAL: in suppressions file "suppressions" near line 4:
	# ==6150==    location should be "...", or should start with "fun:" or "obj:"
	valgrind --leak-check=yes --track-origins=yes --error-exitcode=119 ./xslt test.xml test.xsl

suppress: all
	valgrind --suppressions=suppressions --gen-suppressions=all --leak-check=yes --show-leak-kinds=all --track-origins=yes --error-exitcode=119 ./xslt test.xml test.xsl

install: xml.so
	mkdir -p /usr/local/lib/lua/$(LUA_VER)/
	cp xml.so /usr/local/lib/lua/$(LUA_VER)/

xml.so: xslt.o
	$(CC) $(LIBS) -O -shared -fpic -o $@ $^

xslt.o: xslt.c
	$(CC) $(CFLAGS) -O2 -fpic -c -o $@ $^

xslt: xml.so lua-test.c
	#$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	$(CC) -o $@ $(CFLAGS) xslt.c lua-test.c $(LIBS)

clean:
	-rm xslt xml.so xslt.o
	-rm -fr xml.so.dSYM/ xslt.dSYM/
