#pragma once

enum UNIT_TYPE {
	FOOTMAN,
	BARRIERMAN,
	COMMANDER,
	BUILDER,
	SPAWNPOINT
};

struct Color {
	unsigned char r, g, b, a;	// RGB and alpha for opacity
};

struct vector {
	float x = 0;
	float y = 0;
	vector(float x = 0, float y = 0) {
		this->x = x;
		this->y = y;
	}
	vector& operator= (const vector& in) {
		x = in.x;
		y = in.y;
		return *this;
	}
	vector operator+ (const vector& in) {
		return vector(in.x + x, in.y + y);
	}
	vector operator* (const float& in) {
		return vector(x * in, y * in);
	}
};


class Object {	// I'm making all of this public for ease
public:
	vector pos;
	vector vel;
	vector acc;
	UNIT_TYPE type;
	size_t id;
	bool isVisible;
	bool isStatic;
	Color color;

	Object(float x, float y, float ident);
};
