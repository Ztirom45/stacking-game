all:
	g++ -o main src/main.cpp -I/usr/include -Llib -lSDL2 -lSDL2main -lSDL2_image -lm -Wall -Wpedantic
	./main

compile:
	g++ -o main src/main.cpp -I/usr/include -Llib -lSDL2 -lSDL2main -lSDL2_image -lm -Wall -Wpedantic
	
