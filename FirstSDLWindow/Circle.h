
#include "Object.h"
class Circle : public Object {
public:
	float radius;

	Circle(float x, float y, float radius) : Object(x, y);
};
