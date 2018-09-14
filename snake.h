#ifndef SNAKE_H
#define SNAKE_H

// Standard
#include <Windows.h> // To read button input
#include <iostream>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h> // Disable terminal echo
#include <pthread.h>
#include <time.h>
#include <stdlib.h> // rand

// Stage size, #defined since it is used to declare arrays
#define dimWidth 50
#define dimHeight 20

using namespace std;

/* See snake.cpp for enumeration of all external constants and initial values of other
   global variables */

// Numerical codes for directions
extern const int upward;
extern const int rightward;
extern const int downward;
extern const int leftward;

// Numerical codes to represent the possible states of any given stage coordinates
extern const int isEmpty;
extern const int isBlock;
extern const int isApple;
extern const int isSnake;

// Worldly variables 
extern int stage [dimHeight][dimWidth];
extern int direc;
extern int lastDirec;
extern int score;
extern bool stop; // Interrupt flag stop the game

// Structs capable of storing terminal settings
extern struct termios noecho, saved;

class apple
{
private:
	int appleX = 4;
	int appleY = 4;
public:
	apple() {}
	apple(int X, int Y);
	void setapple(int* X, int* Y, int length);
	int getX();
	int getY();
};

class snake
{
private:
	int length = 5;
	int startLength = 5;
	// TODO: Replace with vectors
	int XCoords[(dimWidth - 2) * (dimHeight - 2)]; 
	int YCoords[(dimWidth - 2) * (dimHeight - 2)];
public:
	snake();
	int getLength();
	int* getXCoords();
	int* getYCoords();
	void setXCoords(int* coords);
	void setYCoords(int* coords);
	void lengthen(int increase);
};

// Listener functions
void keyListen();
void *listenThread(void *arg);

// Terminal modification functions
void disableEcho();
void enableEcho();

// Screen functions
void titleScreen();
void initializeStage();
void drawStage(snake &theSnake, int time);

void updateStage(snake &theSnake, apple &theApple);
int finalizeDirection();
bool collisionCheck(int* sX, int* sY, int length, apple &theApple);

void resetStage();

#endif