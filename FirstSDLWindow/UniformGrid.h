#pragma once
#include "Object.h"
#include <vector>
#include <set>
class UniformGrid {
public:
	int cellWidth, cellHeight;
	std::vector<std::vector<std::vector<Object*>>> uniformGrid;
	
	UniformGrid(int cellWidth, int cellHeight, int windowWidth, int windowHeight);

	vector getCell(const vector& pos);	// Returns a vector that has the x position and y position of the cell you're looking for
	std::set<Object*> setCellsAndScoutCollision(Object* a);			// Sets all of the cells that the object a would be in
	void clearCells();					// Clears all of the cells in the uniformGrid.

};

