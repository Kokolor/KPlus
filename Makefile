CC = gcc
CFLAGS = -Wall -Wextra
TARGET = k+
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

install: all
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
