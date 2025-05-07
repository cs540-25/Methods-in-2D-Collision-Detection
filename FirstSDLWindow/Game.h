#pragma once
#include <vector>
#include "Object.h"
#include "SDL.h"
#include <chrono>

enum Flags {
	DEBUG_INPUT				= 1 << 0,
	DEBUG_UPDATE			= 1 << 1,
	DEBUG_RENDERER			= 1 << 2,
	PRINT_METRICS			= 1 << 3,
	RENDER_COLLIDERS		= 1 << 4,
	BRUTE_FORCE_CIRCLE		= 1 << 5,
	BRUTE_FORCE_AABB		= 1 << 6,
	SWEEP_AND_PRUNE_AABB	= 1 << 7
};

class Game {

public:
	Game(const int width, const int height, const int numObjects, const int flags);						// Initializes the screen as well as the initial placements for spawners
	~Game();
	int handleEvents();
	int update();
	void updatePositions();						// Adds the accelerations and velocities to their respective objects
	void handleCollision(Object& a, Object& b);		// Changes the velocities and accelerations of the two objects to their new directions
	int render();
	void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void setColliderColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool isRunning();
	size_t totalFrames;
	double totalRuntime;						// Stored in seconds

private:
	Color backgroundColor = Color(0,0,0,0);						// The default color for the background is black
	Color colliderColor = Color(0,255,0,255);
	Color collisionColor = Color(255, 0, 0, 255);
	Color overlapColor = Color(0, 100, 128, 255);
	SDL_Window* window;
	int windowHeight;
	int windowWidth;
	SDL_Renderer* renderer;
	std::vector<Object*> objects;

	std::chrono::steady_clock::time_point lastTime;
	float deltaTime;							// Deltatime is measured in seconds
	int flags;
	bool running;

	// Metrics
	float minFPS, maxFPS;
	float fpsTimer;								// Tracks the elapsed time since last fps evaluation
	float fpsTimerInterval = 0.05;				// How many seconds often to print the FPS
	size_t countedFrames;						// This could also be called currentFrame

	void DrawCircle(SDL_Renderer* renderer, Object& circle);	// Draws a circle. (circle rasterization)
	
	// Sweep and prune members
	static char sortAxis;		// This should only ever be 'x' or 'y'
	static bool cmpAABBPositions(const Object* a, const Object* b);
	int AABBOverlap(Object* a, Object* b);	// Returns 1 if overlap, otherwise returns 0; updates the lastOverlapFrame member in objects

	// Collision Functions
	int boundingCircleCollision(Object& a, Object& b);	// Returns 1 if collision, 0 if not; updates the lastCollisionFrame member in objects
	int AABBCollision(Object& a, Object& b);			// Returns 1 if collision, 0 if not; updates the lastCollisionFrame member in objects
	int isColliding(Object* object);					// Returns 1 if there was a collision this frame;
	int isOverlapping(Object* object);					// Returns 1 if the object is overlapping with another this frame (only updated for SWEEP_AND_PRUNE_AABB)
};

