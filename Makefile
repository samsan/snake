CC = gcc
CFLAGS = -O2
LIBS = -lncurses
TARGET = snake

all: $(TARGET)

$(TARGET): snake.c
	$(CC) $(CFLAGS) snake.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)	
