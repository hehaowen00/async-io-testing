CC = clang
CFLAGS = -O2 -flto -march=native -Wall -Werror -Wundef -Wconversion \
		 -Wunreachable-code -Wcast-align -Wcast-qual -Wstrict-prototypes \
		 -Wstrict-overflow=5 -Wswitch-default -Wswitch-enum -fpic
LFLAGS = -I./include
OBJS = main.c util.c
TARGET = http-server

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(CFLAGS) $(OBJS) -o $(TARGET)

debug:
	$(CC) $(LFLAGS) $(CFLAGS) -g $(OBJS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean: $(TARGET)
	rm $(TARGET)
