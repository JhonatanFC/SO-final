CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -pthread
TARGET = simgesrc
SOURCE = simgesrc.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

demo: $(TARGET)
	printf "6\n\n0\n" | ./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f resultados/*.txt
	rm -f resultados/*.csv

rebuild: clean all
