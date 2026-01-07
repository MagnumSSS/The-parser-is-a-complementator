CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
LDFLAGS = -lm
TARGET = engcalc

# Все .c файлы, которые участвуют в сборке
SRCS = main.c gears.c threads.c utils.c \
       parser.c stack.c file_io.c solver.c

# Автоматически создаём список .o файлов
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
