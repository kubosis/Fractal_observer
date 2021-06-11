CFLAGS = -Wall -std=gnu99 -O3
LDFLAGS=  -lpthread  -lm -Wl,--no-as-needed -L/usr/lib/x86_64-linux-gnu -Wl,--no-as-needed -lSDL2 -Wl,--no-as-needed -lSDL2_image

CFLAGS+=$(shell sdl2-config --cflags)

BINARIES=Fractal-observer

LDFLAGS+=$(shell sdl2-config --libs)


SOURCES = fo.c utils/utils.c io_handle/iothreads.c io_handle/messages.c io_handle/prg_serial_nonblock.c
SOURCES += gui_win/gui_win_thread.c gui_win/gui.c gui_win/guithreads.c gui_win/xwin_sdl.c
SOURCES += event_handle/event_handle.c event_handle/event_queue.c computation/computation.c


OBJS=${patsubst %.c,%.o,${SOURCES}}

 Fractal-observer: ${OBJS}
	gcc ${OBJS} ${LDFLAGS} -o $@

${OBJS}: %.o: %.c
	gcc -c ${CFLAGS} $< -o $@

clean:
	rm -f  ${BINARIES} 
	rm -f *.o
	rm -f */*.o
