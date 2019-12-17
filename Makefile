DEPS = database.h utils.h
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -rdynamic  `mysql_config --cflags`
LIBS = `pkg-config --libs gtk+-3.0` `mysql_config --libs` 

all: $(DEPS)
	gcc -o login $(CFLAGS) main.c database.c utils.c $(LIBS)
	gcc -o server $(CFLAGS) server.c database.c utils.c $(LIBS)

