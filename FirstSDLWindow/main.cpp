// Battleground Sandbox
// Benjamin Belandres

// Elevator Pitch:
// Have red and blue guys run around and shoot eachother until one makes it to the other side of the screen

// Goals:
/*
	Have destructible spawners that are visually distinct and spawn in new guys
		Spawn rates are randomly determined 
			Could be calculated through a minigame on the side of the screen?
		Spawns in different guys?

	Units that move towards their respective sides of the board
		Unit types:
			Footman:
				Run of the mill guy. Shoots with less than 100% accuracy.
			Barrierman:
				First deploys an inpassible wall once close enough to an enemy. Then he becomes a normal footman.
			Commander:
				Initiates a charge, making the footmen around him leave their barriers and move straight towards the opposing side
			Builder:
				Creates an additional spawnpoint
				Will only build a spawnpoint where there are enough other units within the radius.

	Victory when one side reaches the opposing side of the board
*/


#include <iostream>
#include "Game.h"

#define RUN_BY_STEP false

// The main elements of a game loop are:
	// Input	
	//	What has the user done to interact with the screen?
	// Update
	//	Run all calculations for changes in the game
	// Render
	//	Draw those new values onto the screen


int main(int args, char* argv[]) {
	/************************************/
	//    I N I T I A L I Z A T I O N
	/************************************/
	/*
		Initialize the screen
		Place all initial units on the board
	*/

	Game game(600, 400, 0);

	/************************************/
	//   G A M E   L O O P
	/************************************/
	/*
		Input
			No inputs are currently planned
		Update
		Render
	*/
	game.setBackgroundColor(0, 0, 255, 255);

	if (RUN_BY_STEP) {
		std::cout << "Enter any key to continue simulation: ";
		char q;
		while (std::cin >> q) {
			game.handleEvents();
			game.update();
			game.render();
			std::cout << "Enter any key to continue simulation: ";
		}
	}
	else {
		while (game.isRunning()) {
			game.handleEvents();
			game.update();
			game.render();

		}
	}

	/*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawPoint(renderer, 30, 30);

	SDL_RenderPresent(renderer);

	SDL_Delay(3000);*/

	/************************************/
	//   R E T U R N I N G   A N D   C L E A N U P
	/************************************/



	return 0;
}