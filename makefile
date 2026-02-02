CC=gcc
OUTPUT=main.exe
SANTS=address,undefined -g
FLAGS=-Wall -Wpedantic -Wextra

release: src/main.c
	$(CC) src/main.c -o $(OUTPUT)

$(OUTPUT): src/main.c
	$(CC) src/main.c -o $(OUTPUT) -fsanitize=$(SANTS) $(FLAGS)

clear:
	rm -f *.exe

launch:
	clear
	make clear
	make
	clear
	./$(OUTPUT)

