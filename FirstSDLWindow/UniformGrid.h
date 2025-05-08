#pragma once
#include "Object.h"
#include <vector>
class UniformGrid {
public:
	int cellWidth, cellHeight;
	std::vector<std::vector<std::vector<Object*>>> uniformGrid;
	
	UniformGrid(int cellWidth, int cellHeight, int windowWidth, int windowHeight);

	vector getCell(const vector& pos);	// Returns a vector that has the x position and y position of the cell you're looking for
	void setCells(Object* a);
	void clearCells();

};

