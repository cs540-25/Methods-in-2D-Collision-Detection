
#include "Object.h"
class Circle : public Object {
public:
	float radius;
	using Object::Object;

	Circle(float x, float y, float radius);
};
