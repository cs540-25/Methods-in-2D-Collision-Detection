#pragma once
#include <vector>
#include "Object.h"
#include "SDL.h"
#include <chrono>

enum Flags {
	DEBUG_INPUT			= 1 << 0,
	DEBUG_UPDATE		= 1 << 1,
	DEBUG_RENDERER		= 1 << 2,
	PRINT_METRICS		= 1 << 3,
	BRUTE_FORCE_CIRCLE	= 1 << 4,
	BRUTE_FORCE_AABB	= 1 << 5
};

class Game {

public:
	Game(const int width, const int height, const int flags);						// Initializes the screen as well as the initial placements for spawners
	~Game();
	int handleEvents();
	int update();
	void updatePositions();						// Adds the accelerations and velocities to their respective objects
	void handleCollision(Object& a, Object& b);		// Changes the velocities and accelerations of the two objects to their new directions
	int render();
	void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool isRunning();
	size_t totalFrames;
	double totalRuntime;						// Stored in seconds

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

	// Metrics
	float minFPS, maxFPS;
	float fpsTimer;								// Tracks the elapsed time since last fps evaluation
	size_t countedFrames;

	void DrawCircle(SDL_Renderer* renderer, Object& circle);	// Draws a circle. (circle rasterization)

	// Collision Functions
	int boundingCircleCollision(Object& a, Object& b);	// Returns 1 if collision, 0 if not
};

