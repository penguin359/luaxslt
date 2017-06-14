CFLAGS = $(shell pkg-config --cflags libxml-2.0 libxslt lua5.2) -Wall -Werror -g -pg -Wno-unused-variable -Wno-unused-function -O0
LIBS = $(shell pkg-config --libs libxml-2.0 libxslt lua5.2)

all: xslt

run: all
	./xslt test.xml test.xsl

check: all
	valgrind --suppressions=suppressions --leak-check=yes --show-leak-kinds=all --track-origins=yes --error-exitcode=119 ./xslt test.xml test.xsl

suppress: all
	valgrind --suppressions=suppressions --gen-suppressions=all --leak-check=yes --show-leak-kinds=all --track-origins=yes --error-exitcode=119 ./xslt test.xml test.xsl

install: xml.so
	cp xml.so /usr/lib/lua/5.*/

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
