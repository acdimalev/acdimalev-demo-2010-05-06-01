demo: demo.c config.h
	gcc -o demo demo.c `pkg-config --cflags --libs sdl cairo`
