PREFIX 	= /usr/local
BINPATH = ${PREFIX}/bin
MANPATH = ${PREFIX}/share/man/man1
SRCS	= $(wildcard src/*.c)
USRC	= $(wildcard src/util/*.c)
UOBJ	= $(USRC:.c=.o)
BINS	= $(addprefix cm-, $(notdir ${SRCS:.c=}))
MAN	= $(wildcard doc/*.1)

all: ${UOBJ} ${BINS}

%.o: %.c
	cc -c -std=c99 -pedantic -Wall -O0 -I. -I/usr/include -I/usr/local/include -o $@ $<

cm-%: src/%.c
	cc -g -std=c99 -pedantic -Wall -O0 -I. -I/usr/include -I/usr/local/include -Wl,-rpath=/usr/local/lib -L/usr/lib -lc -L/usr/local/lib -lconman -o src/$@ ${UOBJ} $<

install: all
	@install -d ${BINPATH}
	@install -d ${MANPATH}
	@install -m 755 $(addprefix src/, ${BINS}) ${BINPATH}
	@install -m 644 ${MAN} ${MANPATH}

uninstall:
	@rm -f $(addprefix ${BINPATH}/, ${BINS})
	@rm -f $(addprefix ${MANPATH}/, $(notdir ${MAN}))

clean:
	@rm -f $(addprefix src/, ${BINS})
	@rm -f ${UOBJ}

.PHONY: all clean install uninstall
