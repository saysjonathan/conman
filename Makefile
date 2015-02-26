PREFIX 	= /usr/local
SRCS	= $(wildcard src/*.c)
USRC	= $(wildcard src/util/*.c)
UOBJ	= $(USRC:.c=.o)
BINS	= $(addprefix cm-, $(notdir ${SRCS:.c=}))

all: ${UOBJ} ${BINS}

%.o: %.c
	cc -c -std=c99 -pedantic -Wall -O0 -I. -I/usr/include -I/usr/local/include -o $@ $<

cm-%: src/%.c
	cc -g -std=c99 -pedantic -Wall -O0 -I. -I/usr/include -I/usr/local/include -Wl,-rpath=/usr/local/lib -L/usr/lib -lc -L/usr/local/lib -lconman -o src/$@ ${UOBJ} $<

install:
	@install -d ${PREFIX}/bin
	@install -m 755 $(addprefix src/, ${BINS}) ${PREFIX}/bin

uninstall:
	@rm -f $(addprefix ${PREFIX}/bin/, ${BINS})

clean:
	@rm -f $(addprefix src/, ${BINS})
	@rm -f ${UOBJ}

.PHONY: all clean install uninstall
