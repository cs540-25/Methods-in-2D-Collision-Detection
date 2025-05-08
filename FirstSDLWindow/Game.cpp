#include "Game.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cmath>
#include <algorithm>
#include <set>

#define FLAG_IS_SET(flag) (((flag) & (flags)) == (flag))

size_t id_count = 0;
char Game::sortAxis = 'x';

Game::Game(const int width, const int height, const int numObjects, const int flags) {
	this->flags = flags;
	
	// SDL init
	SDL_Init(SDL_INIT_EVERYTHING);
	running = true;
	window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	windowWidth = width;
	windowHeight = height;
	renderer = SDL_CreateRenderer(window, -1, 0);
	totalFrames = 0;
	totalRuntime = 0;
	fpsTimer = 0;
	countedFrames = 0;
	minFPS = std::numeric_limits<float>::infinity();
	maxFPS = 0;

	// Board init
	for (int i = 0; i < numObjects; i++) {	// Adding test objects
		Object* test = new Object(float(rand() % windowWidth), float(rand() % windowHeight), 5, id_count);
		id_count += 1;
		test->acc.x = (float)(rand() % 100 + 1) / 20;
		test->acc.y = (float) 500;
		
		// Adding colliders
		if (FLAG_IS_SET(BRUTE_FORCE_AABB) || 
			FLAG_IS_SET(SWEEP_AND_PRUNE_AABB) || 
			FLAG_IS_SET(UNIFORM_GRID_AABB) ||
			FLAG_IS_SET(VARIANCE_SWEEP_AND_PRUNE_AABB)) {
			test->createAABB();
		}
		
		objects.push_back(test);

	}
	if (FLAG_IS_SET(UNIFORM_GRID_AABB)) {
		int cellSize = (int)(objects[0]->radius * 2);
		uniformGrid = UniformGrid(cellSize, cellSize, width, height);
	}

	// Deltatime setup
	lastTime = std::chrono::steady_clock::now();		// For deltatime calculations
	deltaTime = 0;
}

Game::~Game() {
	// Printing Metrics
	if (PRINT_METRICS & flags) {
		printf("Total Runtime:         %20.10f\n", totalRuntime);
		printf("Total Frames:          %20.10zu\n", totalFrames);
		printf("Average Framerate:     %20.10f\n", totalFrames / totalRuntime);
		printf("Maximum Framerate:     %20.10f\n", maxFPS);
		printf("Minimum Framerate:     %20.10f\n", minFPS);
		printf("Framerate Variability: %20.10f\n", maxFPS - minFPS);
		printf("Frames per Object:  %20.10f\n", totalFrames / (float)objects.size());
		//std::string response;
		//std::cout << "Enter any character to close: ";
		//std::cin >> response;
	}

	// Object cleanup
	if (flags & BRUTE_FORCE_AABB) {
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->destroyAABB();
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int Game::handleEvents() {
	if (DEBUG_INPUT & flags)
		std::cout << "Reading Input!" << std::endl;
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		running = false;
		break;
	default:
		break;
	}
	return 0;
}

void Game::handleCollision(Object& a, Object& b) {	// Supposedly, a collision with a static object should be much faster to calculate than two moving objects
	if (b.isStatic) {
		a.vel.x *= -1;
		a.vel.y *= -1;
		return;
	}
	else if (a.isStatic) {
		b.vel.x *= -1;
		b.vel.y *= -1;
		return;
	}

	// Nonstatic Collision
	vector newVelocityA, newVelocityB;
	newVelocityB.x = (2 * a.mass * a.vel.x + b.mass * b.vel.x - a.mass * b.vel.x) / (a.mass + b.mass);
	newVelocityB.y = (2 * a.mass * a.vel.y + b.mass * b.vel.y - a.mass * b.vel.y) / (a.mass + b.mass);
	newVelocityA.x = b.vel.x + newVelocityB.x - a.vel.x;
	newVelocityA.y = b.vel.y + newVelocityB.y - a.vel.y;

	a.vel = newVelocityA;
	b.vel = newVelocityB;
	return;
}

void Game::updatePositions() {
	for (auto i = 0; i < objects.size(); i++) {
		if (!objects[i]->isStatic) {
			// Movement
			objects[i]->vel = objects[i]->vel + objects[i]->acc * deltaTime;
			objects[i]->pos = objects[i]->pos + objects[i]->vel * deltaTime;

			// Collision with edges
			if (objects[i]->pos.x + objects[i]->radius >= windowWidth || objects[i]->pos.x - objects[i]->radius < 0) {	// on x axis
				objects[i]->vel.x *= -1;
			}
			if (objects[i]->pos.y + objects[i]->radius >= windowHeight || objects[i]->pos.y - objects[i]->radius < 0) {	// on y axis
				objects[i]->vel.y *= -1;
			}
		}
	}
}

int Game::update() {
	// Deltatime
	auto currentTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(currentTime - lastTime).count() / 1000;
	lastTime = currentTime;
	if (DEBUG_UPDATE & flags) std::cout << "Deltatime = " << deltaTime << " seconds" << std::endl;

	// Metrics
	totalFrames++;
	countedFrames++;	// This isn't efficient
	totalRuntime += deltaTime;
	if (totalRuntime >= 10.0) {
		running = false;
	}
	fpsTimer += deltaTime;
	if (fpsTimer >= 0.5) {
		float fps = countedFrames / fpsTimer;
		if (fps > maxFPS)	maxFPS = fps;
		if (fps < minFPS)	minFPS = fps;
		if (PRINT_METRICS & flags) {
			std::cout << fps << std::endl;
		}
		fpsTimer = 0;
		countedFrames = 0;
	}
	
	// Determine what kind of collision detection are we using (set through flags from constructor)
	if (DEBUG_UPDATE & flags) std::cout << "Calculating Collisions!" << std::endl;
	if (BRUTE_FORCE_CIRCLE & flags) {
		for (size_t i = 0; i < objects.size(); i++) {
			for (size_t j = i + 1; j < objects.size(); j++) {
				if (boundingCircleCollision(*objects[i], *objects[j])) {
					/*std::cout << "Collision moment\n";*/
					objects[i]->color.b = 0;
					objects[i]->color.g = 0;
					objects[j]->color.b = 0;
					objects[j]->color.a = 255;
					handleCollision(*objects[i], *objects[j]);
				}
			}
		}
	}
	else if (BRUTE_FORCE_AABB & flags) {
		for (size_t i = 0; i < objects.size(); i++) {
			for (size_t j = i + 1; j < objects.size(); j++) {
				if (AABBCollision(*objects[i], *objects[j])) {
					//std::cout << "Collision moment\n";
					objects[i]->color.b = 0;
					objects[i]->color.g = 0;
					objects[j]->color.b = 0;
					objects[j]->color.a = 255;
					handleCollision(*objects[i], *objects[j]);
				}
			}
		}
	}
	else if (FLAG_IS_SET(SWEEP_AND_PRUNE_AABB) || FLAG_IS_SET(VARIANCE_SWEEP_AND_PRUNE_AABB)) {
		// Sort the object array in ascending order based on an axis (it doesn't matter which)
		//	The axis will be chosen through sortAxis
		//	cmpAABBPositions(const Object* a, const Object* b)
		//		Comparison function
		//		if sortAxis = x
		//			minA = a.min.x
		//			minB = b.min.x
		//		etc...
		//		if (minA < minB) return true
		//		else return false
		//	sort(objects.begin(), objects.end(), cmpAABBPositions)
		// Check for overlapping colliders
		//	AABBOverlap(const Object* a, const Object* b)
		//		if min(a) <= min(b) <= max(a), there is overlap
		//		else if min(a) <= max(b) <= max(a), there is overlap
		//		otherwise, no overlap
		// Check the overlapping colliders for collisions
		// OPTIONAL (Could maybe add this as an additional collision detection method)
		//	Calculate the variance (maxOverallPosition - minOverallPosition) of each axis
		//	Update sortAxis to the axis with the most variance


		// For variance based sweep and prune
		float xVariance, yVariance;	
		float minX = std::numeric_limits<float>::infinity();
		float maxX = 0;
		float minY = std::numeric_limits<float>::infinity();
		float maxY = 0;
		std::sort(objects.begin(), objects.end(), cmpAABBPositions);
		//for (size_t i = 0; i < objects.size(); i++) {
		//	std::cout << objects[i]->AABB->min().x << std::endl;
		//}
		for (size_t i = 0; i < objects.size(); i++) {
			if (FLAG_IS_SET(VARIANCE_SWEEP_AND_PRUNE_AABB)) {	// Recording the maximums and minimums for the calculation of variance
				if (objects[i]->AABB->max().x > maxX) maxX = objects[i]->AABB->max().x;
				if (objects[i]->AABB->min().x < minX) minX = objects[i]->AABB->min().x;
				if (objects[i]->AABB->max().y > maxY) maxY = objects[i]->AABB->max().y;
				if (objects[i]->AABB->min().y > minY) minY = objects[i]->AABB->min().y;
			}
			for (size_t j = i + 1; j < objects.size(); j++) {	// Only looking at objects after the 'i'th object as to not waste time
				if (objects[j]->AABB->min().x > objects[i]->AABB->max().x) {
					break;
				}
				if (AABBOverlap(objects[i], objects[j])) {
					objects[i]->lastOverlapFrame = totalFrames;
					if (AABBCollision(*objects[i], *objects[j])) {
						handleCollision(*objects[i], *objects[j]);
					}
				}
			}
		}
		if (FLAG_IS_SET(VARIANCE_SWEEP_AND_PRUNE_AABB)) {
			xVariance = maxX - minX;
			yVariance = maxY - minY;
			if (xVariance >= yVariance) {
				sortAxis = 'x';
			}
			else {
				sortAxis = 'y';
			}
		}

	}
	else if (FLAG_IS_SET(UNIFORM_GRID_AABB)) {
		uniformGrid.clearCells();
		for (size_t i = 0; i < objects.size(); i++) {
			std::set<Object*> possibleCollisions = uniformGrid.setCellsAndScoutCollision(objects[i]);
			for (auto collisionObject : possibleCollisions) {
				if (AABBCollision(*objects[i], *collisionObject)) {
					handleCollision(*objects[i], *collisionObject);
				}
			}
		}
	}

	// Update Object Positions
	if (DEBUG_UPDATE & flags) std::cout << "Calculating Object Updates!" << std::endl;
	updatePositions();

	return 0;
}

void Game::DrawCircle(SDL_Renderer* renderer, Object& circle) {
	float radius = circle.radius;
	float centreX = circle.pos.x;
	float centreY = circle.pos.y;
	
	const float diameter = (radius * 2);

	float x = (radius - 1);
	float y = 0;
	float tx = 1;
	float ty = 1;
	float error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, (int)(centreX + x), (int)(centreY - y));
		SDL_RenderDrawPoint(renderer, (int)(centreX + x), (int)(centreY + y));
		SDL_RenderDrawPoint(renderer, (int)(centreX - x), (int)(centreY - y));
		SDL_RenderDrawPoint(renderer, (int)(centreX - x), (int)(centreY + y));
		SDL_RenderDrawPoint(renderer, (int)(centreX + y), (int)(centreY - x));
		SDL_RenderDrawPoint(renderer, (int)(centreX + y), (int)(centreY + x));
		SDL_RenderDrawPoint(renderer, (int)(centreX - y), (int)(centreY - x));
		SDL_RenderDrawPoint(renderer, (int)(centreX - y), (int)(centreY + x));

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

bool Game::cmpAABBPositions(const Object* a, const Object* b) {	// For sorting the AABB objects
	float minA, minB;
	if (sortAxis == 'x') {
		minA = a->AABB->min().x;
		minB = b->AABB->min().x;
	}
	else {	// sortAxis is y
		minA = a->AABB->min().y;
		minB = b->AABB->min().y;
	}
	return (minA < minB);
}

int Game::AABBOverlap(Object* a, Object* b) {
	float minA, maxA, minB, maxB;
	if (sortAxis == 'x') {
		minA = a->AABB->min().x;
		maxA = a->AABB->max().x;
		minB = b->AABB->min().x;
		maxB = b->AABB->max().x;
	}
	else {	// sortAxis == 'y'
		minA = a->AABB->min().y;
		maxA = a->AABB->max().y;
		minB = b->AABB->min().y;
		maxB = b->AABB->max().y;
	}
	if (maxA >= minB && maxB >= minA) {
		a->lastOverlapFrame = totalFrames;
		b->lastOverlapFrame = totalFrames;
		return 1;
	}
	return 0;
}

int Game::boundingCircleCollision(Object& a, Object& b) {
	vector d = a.pos - b.pos;	// Distance between centers
	float dist2 = d.dot(d);		// This is just d^2
	float radiusSum = a.radius + b.radius;
	if (dist2 <= radiusSum * radiusSum) {
		a.lastCollisionFrame = totalFrames;
		b.lastCollisionFrame = totalFrames;
		return true;	// is d^2 <= radiusSum^2?
	}
	return false;
}

int Game::AABBCollision(Object& a, Object& b) {
	const vector* aCenter = a.AABB->center;
	const float* aRadi = a.AABB->radi;
	const vector* bCenter = b.AABB->center;
	const float* bRadi = b.AABB->radi;
	if (abs(aCenter->x - bCenter->x) > (aRadi[0] + bRadi[0])) return 0;
	if (abs(aCenter->y - bCenter->y) > (aRadi[1] + bRadi[1])) return 0;

	a.lastCollisionFrame = totalFrames;
	b.lastCollisionFrame = totalFrames;
	return 1;
}

int Game::isColliding(Object* object) {
	return (object->lastCollisionFrame == totalFrames);
}

int Game::isOverlapping(Object* object) {
	return (object->lastOverlapFrame == totalFrames);
}

int Game::render() {
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(renderer);
	if (DEBUG_RENDERER & flags) {
		std::cout << "Rendering to the Screen!" << std::endl;
		std::cout << "Number of Objects in object vector = " << objects.size() << std::endl;
	}

	for (auto i = 0; i < objects.size(); i++) {
		if (DEBUG_RENDERER & flags) std::cout << "\tDrawing object " << i << std::endl;
		if (DEBUG_RENDERER & flags) printf("\t\tColor = (%d, %d, %d, %d)\n", objects[i]->color.r, objects[i]->color.g, objects[i]->color.b, objects[i]->color.a);
		if (DEBUG_RENDERER & flags) printf("\t\tCoordinate = (%f, %f)\n", objects[i]->pos.x, objects[i]->pos.y);
		if (FLAG_IS_SET(DEBUG_RENDERER | BRUTE_FORCE_AABB)) printf("\t\tCoordinateAABB = (%f, %f)\n", objects[i]->AABB->center->x, objects[i]->AABB->center->y);
		if (objects[i]->isCircle) {	// Is the object just a point or a circle?
			// Draw circle
			SDL_SetRenderDrawColor(renderer, objects[i]->color.r, objects[i]->color.g, objects[i]->color.b, objects[i]->color.a);
			DrawCircle(renderer, *objects[i]);
			
			// Drawing colliders
			if (FLAG_IS_SET(RENDER_COLLIDERS)) {
				if (FLAG_IS_SET(BRUTE_FORCE_AABB) || FLAG_IS_SET(SWEEP_AND_PRUNE_AABB)) {
					if (isColliding(objects[i])) {
						SDL_SetRenderDrawColor(renderer, collisionColor.r, collisionColor.g, collisionColor.b, collisionColor.a);	// Change color to red if colliding
					}
					else if (isOverlapping(objects[i])) {
						SDL_SetRenderDrawColor(renderer, overlapColor.r, overlapColor.g, overlapColor.b, overlapColor.a);	// Change color to light blue if overlapping
					}
					else {
						SDL_SetRenderDrawColor(renderer, colliderColor.r, colliderColor.g, colliderColor.b, colliderColor.a); // Change color to default color for no collisions or overlap
					}
					SDL_RenderDrawPoint(renderer, (int)objects[i]->AABB->center->x, (int)objects[i]->AABB->center->y);	// Drawing the center of the collider
					SDL_Rect collider = objects[i]->AABB->toSDLRect();
					SDL_RenderDrawRect(renderer, &collider);
				}
			}
		}
		else {
			// Draw point
			SDL_SetRenderDrawColor(renderer, objects[i]->color.r, objects[i]->color.g, objects[i]->color.b, objects[i]->color.a); // Drawing a point
			SDL_RenderDrawPoint(renderer, (int)objects[i]->pos.x, (int)objects[i]->pos.y);
		}
	}

	SDL_RenderPresent(renderer);
	return 0;
}

void Game::setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	backgroundColor.r = r;
	backgroundColor.g = g;
	backgroundColor.b = b;
	backgroundColor.a = a;
}

void Game::setColliderColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	colliderColor.r = r;
	colliderColor.g = g;
	colliderColor.b = b;
	colliderColor.a = a;
}

bool Game::isRunning() {
	return running;
}