EXEC = programa

SRC = tarea3.c
HEADERS = tarea3s.h

CC = gcc
CFLAGS = -Wall -pthread -std=c11

all: $(EXEC)

$(EXEC): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

run: $(EXEC)
	./$(EXEC) -b 20 -h 2 -c 3 -v 1 -e 2 -x 4 -y 3 -z 3 -i 100000

clean:
	rm -f $(EXEC)