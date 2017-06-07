CFLAGS = $(shell pkg-config --cflags libxml-2.0 libxslt lua) -Wall -Werror -g -pg
LIBS = $(shell pkg-config --libs libxml-2.0 libxslt lua)

all: xslt

run: all
	./xslt test.xml test.xsl

check: all
	valgrind --suppressions=suppressions --leak-check=yes --show-leak-kinds=all --error-exitcode=119 ./xslt test.xml test.xsl

xslt: xslt.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	-rm xslt
