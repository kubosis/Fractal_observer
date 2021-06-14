CFLAGS = -Wall -std=gnu99 -O3 -g
LDFLAGS=  -lpthread  -lm -Wl,--no-as-needed -L/usr/lib/x86_64-linux-gnu -Wl,--no-as-needed -lSDL2 -Wl,--no-as-needed -lSDL2_image 

CFLAGS+=$(shell sdl2-config --cflags)

BINARIES=Fractal-observer

LDFLAGS+=$(shell sdl2-config --libs)


SOURCES = ${patsubst %.c,%.o,${wildcard *.c */*.c}}

OBJS=${patsubst %.c,%.o,${SOURCES}}

 Fractal-observer: ${OBJS}
	gcc ${OBJS} ${LDFLAGS} -o $@

${OBJS}: %.o: %.c
	gcc -c ${CFLAGS} $< -o $@

clean:
	rm -f  ${BINARIES} 
	rm -f *.o
	rm -f */*.o
