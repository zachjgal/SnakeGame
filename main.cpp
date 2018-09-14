#include "snake.h"

/* Calls keylisten which is what actually listens for key input. Also, pressing s will end the game. */
void *listenThread(void *arg)
{
		while(true)
		{
			// Calls function to get states of necessary buttons 
			keyListen();

			if(GetKeyState('S') & 0x8000) // stop listening
			{
				stop = TRUE;
			}
		}

	pthread_exit(NULL);
}

int main(void)
{
	// Initial procedures

	disableEcho();

	titleScreen();

	time_t start = time(0);
	double seconds_since_start = difftime(time(0), start); // Keep track of time

	pthread_t new_thread; // Create a new thread to listen for button input
	int ret = pthread_create(&new_thread, NULL, &listenThread, NULL);
	if (ret != 0)
	{
		cout << "Error: thread creation failed\n";
		return 1;
	}

	initializeStage();

	snake s;
	apple a(30, 6); // First apple coordinates
	srand(time(NULL)); // Seeds the rand function

	a.setapple(s.getXCoords(), s.getYCoords(), s.getLength()); // Set the first apple

	// Begin the game
	while (true)
	{
		// Lengthen the snake every fifth second
		seconds_since_start = difftime(time(0), start);
		s.lengthen((int)seconds_since_start/5);

		updateStage(s, a);
		drawStage(s, seconds_since_start);
		resetStage();

		// Checks for interrupt signaling game over
		if(stop == TRUE)
		{
			cout << "\u001b[0J";
			cout << "\033[32mGAME OVER\033[0m\n";
			enableEcho();
			return 0;
		}
	}
}
