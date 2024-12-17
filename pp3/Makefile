CC = g++
CFLAGS = -Wall -g
EXECUTABLE = comp
INSTALL_DIR = ~/pp3 

all: $(EXECUTABLE)

$(EXECUTABLE): pp3.cpp ThreadPool.cpp ThreadPool.h
	$(CC) $(CFLAGS) pp3.cpp ThreadPool.cpp -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

cleanI:
	rm -f $(INSTALL_DIR)/$(EXECUTABLE)

.PHONY: all install clean cleanI
