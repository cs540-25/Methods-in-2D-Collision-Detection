#include "Game.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cmath>
#include <algorithm>

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
	backgroundColor.r = 0;
	backgroundColor.g = 0;
	backgroundColor.b = 0;
	backgroundColor.a = 255;
	colliderColor.r = 0;
	colliderColor.g = 255;
	colliderColor.b = 0;
	colliderColor.a = 255;
	totalFrames = 0;
	totalRuntime = 0;
	fpsTimer = 0;
	countedFrames = 0;
	minFPS = std::numeric_limits<float>::infinity();
	maxFPS = 0;

	// Board init
	for (int i = 0; i < numObjects; i++) {	// Adding test objects
		Object* test = new Object(float(rand() % windowWidth), float(rand() % windowHeight), 10, id_count);
		id_count += 1;
		test->acc.x = (float)(rand() % 100 + 1) / 20;
		test->acc.y = (float) 500;
		
		// Adding colliders
		if (FLAG_IS_SET(BRUTE_FORCE_AABB) | FLAG_IS_SET(SWEEP_AND_PRUNE_AABB)) {
			test->createAABB();
		}
		
		objects.push_back(test);

	}
	/*Object test1(25, 25, 25, id_count);
	id_count += 1;
	test1.color.r = 255;
	test1.color.g = 0;
	test1.color.b = 0;
	test1.acc.x = (float)-30;
	test1.acc.y = (float)-500;
	objects.push_back(test1);*/
	endOfLastUpdate = std::chrono::steady_clock::now();		// For deltatime calculations
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
		printf("Framerate per Object:  %20.10f\n", (totalFrames / totalRuntime) / objects.size());
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
			objects[i]->color.a -= 1;

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
	auto start = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(start - endOfLastUpdate).count() / 1000;
	if (DEBUG_UPDATE & flags) std::cout << "Deltatime = " << deltaTime << " seconds" << std::endl;

	// Metrics
	totalFrames++;
	countedFrames++;	// This isn't efficient
	totalRuntime += deltaTime;
	fpsTimer += deltaTime;
	if (fpsTimer >= 0.05) {
		float fps = countedFrames / fpsTimer;
		if (fps > maxFPS)	maxFPS = fps;
		if (fps < minFPS)	minFPS = fps;
		if (PRINT_METRICS & flags) {
			std::cout << countedFrames / fpsTimer << std::endl;
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
					objects[j]->color.g = 0;
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
					objects[j]->color.g = 0;
					handleCollision(*objects[i], *objects[j]);
				}
			}
		}
	}
	else if (FLAG_IS_SET(SWEEP_AND_PRUNE_AABB)) {
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
		std::sort(objects.begin(), objects.end(), cmpAABBPositions);
		//for (size_t i = 0; i < objects.size(); i++) {
		//	std::cout << objects[i]->AABB->min().x << std::endl;
		//}

	}

	// Update Object Positions
	if (DEBUG_UPDATE & flags) std::cout << "Calculating Object Updates!" << std::endl;
	updatePositions();

	endOfLastUpdate = std::chrono::steady_clock::now();
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

int Game::boundingCircleCollision(Object& a, Object& b) {
	vector d = a.pos - b.pos;	// Distance between centers
	float dist2 = d.dot(d);		// This is just d^2
	float radiusSum = a.radius + b.radius;
	return dist2 <= radiusSum * radiusSum;	// is d^2 <= radiusSum^2?
}

int Game::AABBCollision(Object& a, Object& b) {
	const vector* aCenter = a.AABB->center;
	const float* aRadi = a.AABB->radi;
	const vector* bCenter = b.AABB->center;
	const float* bRadi = b.AABB->radi;
	if (abs(aCenter->x - bCenter->x) > (aRadi[0] + bRadi[0])) return 0;
	if (abs(aCenter->y - bCenter->y) > (aRadi[1] + bRadi[1])) return 0;

	return 1;
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
			if (FLAG_IS_SET(RENDER_COLLIDERS | BRUTE_FORCE_AABB)) {
				SDL_SetRenderDrawColor(renderer, colliderColor.r, colliderColor.g, colliderColor.b, colliderColor.a);
				SDL_RenderDrawPoint(renderer, (int)objects[i]->AABB->center->x, (int)objects[i]->AABB->center->y);	// Drawing the center of the collider
				SDL_Rect collider = objects[i]->AABB->toSDLRect();
				SDL_RenderDrawRect(renderer, &collider);
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