#include "Game.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

size_t id_count = 0;

Game::Game(const int width, const int height, const int flags) {
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

	// Board init
	for (int i = 0; i < 10; i++) {	// Adding test objects
		Object* test = new Object(float(rand() % windowWidth), float(rand() % windowHeight), 4, id_count);
		id_count += 1;
		test->acc.x = (float)(rand() % 100 + 1) / 20;
		test->acc.y = (float) 500;
		objects.push_back(*test);

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
	// Removing all Objects from the object vector
	for (size_t i = 0; i < objects.size(); i++) {
		delete& objects[i];
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
	return;
}

void Game::updatePositions() {
	for (auto i = 0; i < objects.size(); i++) {
		if (!objects[i].isStatic) {
			// Movement
			objects[i].vel = objects[i].vel + objects[i].acc * deltaTime;
			objects[i].pos = objects[i].pos + objects[i].vel * deltaTime;
			objects[i].color.a -= 1;

			// Collision with edges
			if (objects[i].pos.x >= windowWidth || objects[i].pos.x < 0) {	// on x axis
				objects[i].vel.x *= -1;
			}
			if (objects[i].pos.y >= windowHeight || objects[i].pos.y < 0) {	// on y axis
				objects[i].vel.y *= -1;
			}
		}
	}
}

int Game::update() {
	// Deltatime
	auto start = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(start - endOfLastUpdate).count() / 1000;
	if (DEBUG_UPDATE & flags) std::cout << "Deltatime = " << deltaTime << " seconds" << std::endl;

	// Update objects
	if (DEBUG_UPDATE & flags) std::cout << "Calculating Collisions!" << std::endl;
	if (BRUTE_FORCE_CIRCLE & flags) {
		for (size_t i = 0; i < objects.size(); i++) {
			for (size_t j = i; j < objects.size(); j++) {
				if (boundingCircleCollision(objects[i], objects[j])) {
					
				}
			}
		}
	}
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

	SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
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

int Game::boundingCircleCollision(Object& a, Object& b) {
	vector d = a.pos - b.pos;	// Distance between centers
	float dist2 = d.dot(d);		// This is just d^2
	float radiusSum = a.radius + b.radius;
	return dist2 <= radiusSum * radiusSum;	// is d^2 <= radiusSum^2?
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
		if (DEBUG_RENDERER & flags) printf("\t\tColor = (%d, %d, %d, %d)\n", objects[i].color.r, objects[i].color.g, objects[i].color.b, objects[i].color.a);
		if (DEBUG_RENDERER & flags) printf("\t\tCoordinate = (%f, %f)\n", objects[i].pos.x, objects[i].pos.y);
		if (objects[i].isCircle) {	// Is the object just a point or a circle?
			// Draw circle
			DrawCircle(renderer, objects[i]);
		}
		else {
			// Draw point
			SDL_SetRenderDrawColor(renderer, objects[i].color.r, objects[i].color.g, objects[i].color.b, objects[i].color.a); // Drawing a point
			SDL_RenderDrawPoint(renderer, (int)objects[i].pos.x, (int)objects[i].pos.y);
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

bool Game::isRunning() {
	return running;
}