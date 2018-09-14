# Simple makefile generates executable "play"

play: main.o snake.o
	g++ main.o snake.o -o play

main.o: main.cpp snake.cpp
	g++ -c main.cpp

snake.o: snake.cpp snake.h
	g++ -c snake.cpp

clean:
	rm main.o snake.o play
