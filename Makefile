CFLAGS = $(shell pkg-config --cflags libxml-2.0 libxslt lua) -Wall -Werror -g -pg -Wno-unused-variable -Wno-unused-function
LIBS = $(shell pkg-config --libs libxml-2.0 libxslt lua)

all: xslt

run: all
	./xslt test.xml test.xsl

check: all
	valgrind --suppressions=suppressions --leak-check=yes --show-leak-kinds=all --error-exitcode=119 ./xslt test.xml test.xsl

xml.so: xslt.o
	$(CC) $(LIBS) -O -shared -fpic -o $@ $^

xslt.o: xslt.c
	$(CC) $(CFLAGS) -O2 -fpic -c -o $@ $^

xslt: xml.so lua-test.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	-rm xslt xml.so xslt.o
	-rm -fr xml.so.dSYM/ xslt.dSYM/
