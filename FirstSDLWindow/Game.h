#pragma once
#include <vector>
#include "Object.h"
#include "SDL.h"
#include <chrono>

enum Flags {
	DEBUG_INPUT		= 1 << 0,
	DEBUG_UPDATE	= 1 << 1,
	DEBUG_RENDERER	= 1 << 2
};

class Game {

public:
	Game(const int width, const int height, const int flags);						// Initializes the screen as well as the initial placements for spawners
	~Game();
	int handleEvents();
	int update();
	void updatePositions();						// Adds the accelerations and velocities to their respective objects
	int render();
	void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool isRunning();

private:
	Color backgroundColor;						// The default color for the background is black
	SDL_Window* window;
	int windowHeight;
	int windowWidth;
	SDL_Renderer* renderer;
	std::vector<Object> objects;

	std::chrono::steady_clock::time_point endOfLastUpdate;
	float deltaTime;							// Deltatime is measured in seconds

	int flags;
	bool running;

	void DrawCircle(SDL_Renderer* renderer, float centreX, float centreY, float radius);	// Draws a circle. Set the desired color before you call this function (circle rasterization)
};

