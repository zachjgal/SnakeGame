#include "snake.h"

// Numerical codes for directions
const int upward = 0;
const int rightward = 1;
const int downward = 2;
const int leftward = 3;

// Numerical codes to represent the possible states of any given stage coordinate
const int isEmpty = 0;
const int isBlock = 1;
const int isApple = 2;
const int isSnake = 3;

// Worldly variables, see snake.h for values of dimWidth and dimHeight
int stage [dimHeight][dimWidth];
int direc = rightward;
int lastDirec = rightward;
int score = 0;
bool stop = FALSE;

struct termios noecho, saved;

/* Functions for the apple class*/

apple::apple(int X, int Y)
{
	appleX = X;
	appleY = Y;
}

void apple::setapple(int* X, int* Y, int length)
{
	bool onSnake;
	do
	{
		onSnake = FALSE; // Assume the apple didn't land on the snake

		appleX = rand() % (dimWidth - 2) + 1; // A number between 1 and height/width -2
		appleY = rand() % (dimHeight - 2) + 1;

		// Checks if the apple and the snake share coordinates, and tries again if they do
		for (int i = 0; i < length; i++)
		{
			if (appleX == X[i] && appleY == Y[i])
			{
				onSnake = TRUE;
				break;
			}
		}
	} while (onSnake);
	
	// Set the place on the board determined by the func to have an apple
	stage[appleY][appleX] = isApple;
}

int apple::getX()
{
	return appleX;
}

int apple::getY()
{
	return appleY;
}


/* Functions for the snake class*/

snake::snake()
{
	// Populates coordinates array with initial values
	for(int i = 0; i < length; i++)
	{
		XCoords[i] = 17 - i;
	}
	for (int j = 0; j < length; j++)
	{
		YCoords[j] = 6;
	}
}

int snake::getLength()
{
	return length;
}
int* snake::getXCoords()
{
	return XCoords;
}

int* snake::getYCoords()
{
	return YCoords;
}

void snake::setXCoords(int* coords)
{
	for (int i = 0; i < length; i++)
	{
		XCoords[i] = coords[i];
	}
}

void snake::setYCoords(int* coords)
{
	for (int i = 0; i < length; i++)
	{
		YCoords[i] = coords[i];
	}
}
void snake::lengthen(int increase)
{
	length = startLength + increase;
}


/* Listening functions */

/* Listens for and stores the most recent arrow key input. */
void keyListen()
{
	// Checks the return from GetKeyState and pulls the most significant bit 
	if(GetKeyState(VK_UP) & 0x8000) // Up
	{
		direc = upward;
	}
	if (GetKeyState(VK_RIGHT) & 0x8000) // Right
	{
		direc = rightward;
	}
	if(GetKeyState(VK_DOWN) & 0x8000) // Down
	{
		direc = downward;
	}
	if (GetKeyState(VK_LEFT) & 0x8000) // Left
	{
		direc = leftward;
	}
}

/* See main for content of listenThread */


/* Terminal setting modification functions */

/* Prevents user input from displaying to the terminal, which would offset the cursor
positiions the program relies on controlling to function properly */
void disableEcho()
{
	tcgetattr(STDIN_FILENO, &noecho); // Gets the current terminal settings
	saved = noecho; // Both structs now represent the original settings
	noecho.c_lflag &= ~(ECHO); // Turn local echo off on noecho
	tcsetattr(STDIN_FILENO, TCSANOW, &noecho); // Set terminal settings to reflect change
}

/* Turns it back on at the end so the user can see what they're writing after the game ends. */
void enableEcho()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &saved);
}


/* Stage manipulation and graphic output functions*/

void titleScreen() // Took forever to format
{
	cout << "\n\n";
	for(int i = 0; i < 34; i++)
	{
		cout << "\033[32m█\033[0m";
	}
	usleep(1000000);
	cout << "\n\033[32m                                 █\033[0m\n";
	usleep(1000000);
	cout << "\033[32m▒█▀▀▀█ ▒█▄░▒█ ░█▀▀█ ▒█░▄▀ ▒█▀▀▀  █\033[0m\n";
	usleep(1000000);
	cout << "\033[32m░▀▀▀▄▄ ▒█▒█▒█ ▒█▄▄█ ▒█▀▄░ ▒█▀▀▀  █\033[0m\n";
	usleep(1000000);
	cout << "\033[32m▒█▄▄▄█ ▒█░░▀█ ▒█░▒█ ▒█░▒█ ▒█▄▄▄\033[0m  ";
	cout <<"\033[31m@\033[0m\n\n\n";
	usleep(1000);
}

/* Draws the blocks in the borders of the stage */
void initializeStage()
{
	for (int g = 0; g < dimHeight; g++)
	{
		for (int h = 0; h < dimWidth; h++)
		{
			stage[g][h] = isEmpty; // Initially all empty
		}
	}
	// Top and bottom
	for (int i = 0; i < dimWidth; i++)
	{
		stage[0][i] = isBlock;
		stage[dimHeight - 1][i] = isBlock;
	}
	// Left and right
	for (int k = 0; k < dimHeight; k++)
	{
		stage[k][0] = isBlock;
		stage[k][dimWidth - 1] = isBlock;
	}
}

/* Substitutes appropriate characters for numerical codes and prints stage buffer */
void drawStage(snake &theSnake, int time)
{
	cout << "\033[32m" << "Score: " << score << " | Length: " << theSnake.getLength() << " | Time: " << time <<"\033[0m" << "\n";
	for (int i = 0; i < dimHeight; i++)
	{
		for (int j = 0; j < dimWidth; j++)
		{
			if (stage[i][j] == isBlock)
			{
				cout << "\033[32m█\033[0m";
			}
			if (stage[i][j] == isSnake)
			{
				cout << "\033[33m█\033[0m";
			}
			else if (stage[i][j] == isApple)
			{
				cout <<"\033[31m@\033[0m";
			}
			else if (stage[i][j] == isEmpty)
			{
				cout << " ";
			}
		}
		cout << "\n";
	}
}

/* Prevents the snake from moving in the direction opposite its direction at last stage refresh,
   which would drive the snake into itself and end the game. Returns a final direction code "move". */
int finalizeDirection() // Helper for updateStage
{
	// Move in the specified direction unless it's the wrong direction
	int move = direc;

	if(direc == downward && lastDirec == upward)
	{
		move = upward;
	}
	if(direc == upward && lastDirec == downward)
	{
		move = downward;
	}
	if(direc == rightward && lastDirec == leftward)
	{
		move = leftward;
	}
	if(direc == leftward && lastDirec == rightward)
	{
		move = rightward;
	}

	return move;
}

/* This function checks for 3 things, as there are 3 things the snake could come into contact with.
   1. Did the snake hit the apple?
   2. Did the snake hit the wall of the stage?
   3. Did the snake hit itself? 

   The function will return TRUE if 2 or 3 are true, which will cause the game to end. */
bool collisionCheck(int* sX, int* sY, int length, apple &theApple) // Helper for updateStage
{
	int headX = sX[0];
	int headY = sY[0];

	if (headX == theApple.getX() && headY == theApple.getY()) // Head coordinates match apple coordinates
	{
		theApple.setapple(sX, sY, length);
		score++;
	}
	// Snake is out of bounds
	if (headX == 0 || headX == dimWidth - 1)
	{
		return TRUE;
	}
	if (headY == 0 || headY == dimHeight - 1)
	{
		return TRUE;
	}
	// Checks for duplicate coordinate pairs, ie snake ran into itself
	int xCurr;
	int yCurr;
	for(int k = 0; k < length; k++)
	{
		int xCurr = sX[k];
		int yCurr = sY[k];
		for (int m = 0; m < length - k; m++)
		{
			if (xCurr == sX[m] && yCurr == sY[m] && m != k)
			{
				return TRUE;
			}
		}
	}

	return FALSE; // Nothing was hit
}

/* Updates the coordinates of the snake and writes them to the stage buffer */
void updateStage(snake &theSnake, apple &theApple)
{
	int* newXCoords = theSnake.getXCoords(); // Makes this new pointer point to the list of coordinates
	int* newYCoords = theSnake.getYCoords();

	// Shifts position arrays over, with the effect of overwriting the last or "tail" position
	for (int i = theSnake.getLength() - 1; i >= 0; i--)
	{
		newXCoords[i + 1] = newXCoords[i];
		newYCoords[i + 1] = newYCoords[i];
	}

	int mv = finalizeDirection();
	// Sets the first or "head" element of the array based on the direction the snake is to move
	if (mv == upward)
	{
		newYCoords[0] = newYCoords[1] - 1; // New head is 1 higher
		newXCoords[0] = newXCoords[1];
	}
	if (mv == downward)
	{
		newYCoords[0] = newYCoords[1] + 1; // New head is 1 column down
		newXCoords[0] = newXCoords[1];
	}
	if (mv == leftward)
	{

		newXCoords[0] = newXCoords[1] - 1; // New head is 1 to the left
		newYCoords[0] = newYCoords[1];
	}
	if (mv == rightward)
	{

		newXCoords[0] = newXCoords[1] + 1; // New head is 1 to the right
		newYCoords[0] = newYCoords[1];
	}

	// Commits the change by changing the actual data in the snake class
	theSnake.setXCoords(newXCoords);
	theSnake.setYCoords(newYCoords);

	lastDirec = mv; // Stores the current movement direction for reference next time the stage refreshes

	for(int k = 0; k < theSnake.getLength() - 1; k++)
	{
		stage[newYCoords[k]][newXCoords[k]] = isSnake; // Changes the stage to match the coords
	}

	if(collisionCheck(newXCoords, newYCoords, theSnake.getLength(), theApple)) // Stops if the snake hit something
	{
		stop = TRUE;
	}
}

/* Uses ANSI codes to move the cursor to the top so when the stage is printed again, it overwrites the
last print perfectly. These are used over ncurses to be more compatible with a ZedBoard, as another
version was written to operate on that platform using the push buttons instead of arrow keys. The usleep
call dictates the refresh rate of the screen, currently 1/5 of a second. The issue with ANSI codes is
that they introduce some "magic numbers". If the dimensions were changed, these codes would need to be
rewritten. */
void resetStage()
{
	// TODO: Concatenate the rest of the code with dimHeight and dimWidth to a string buffer, and print that.

	cout << "\u001b[21A"; // Up 21 because we printed the score on one line, then the stage
	cout << "\u001b[50D"; // Left 50
	usleep(200000);
	cout << "\u001b[0J"; // Clear below cursor
	for(int i = 1; i < dimHeight - 1; i++)
	{
		for (int j = 1; j < dimWidth - 1; j++)
		{
			if (stage[i][j] == isSnake) // No need to clear the walls
			{
				stage[i][j] = isEmpty;
			}
		}
	}
}
