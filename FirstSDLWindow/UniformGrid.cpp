#include "UniformGrid.h"

UniformGrid::UniformGrid(int cellWidth, int cellHeight, int windowWidth, int windowHeight)
{
	int numXCells = windowWidth / cellWidth;
	int numYCells = windowHeight / cellHeight;
	this->cellWidth = cellWidth;
	this->cellHeight = cellHeight;

	// Each Object* will begin as NULL and will already be 5 spots large for ease of pushing
	uniformGrid = std::vector<std::vector<std::vector<Object*>>>(numXCells, std::vector<std::vector<Object*>>(numYCells, std::vector<Object*>(5, NULL)));
	
}

vector UniformGrid::getCell(const vector& pos)
{
	return vector((int)pos.x/cellWidth, (int)pos.y/cellHeight);
}

//setCells(Object* a)
//	min = getCell(a.min)
//	max = getCell(a.max)
//	for (i = min.x; i <= max.x; i++)
//		for (j = min.y; j <= max.y; j++)
//			uniformGrid[i][j].gridArray.push_back(a)
void UniformGrid::setCells(Object* a)
{
	vector min = getCell(a->AABB->min());
	vector max = getCell(a->AABB->max());
	for (int i = (int)min.x; i <= (int)max.x; i++) {
		for (int j = (int)min.y; j < (int)max.y; j++) {
			uniformGrid[i][j].push_back(a);
		}
	}
}

void UniformGrid::clearCells()
{
	for (auto i = 0; i < uniformGrid.size(); i++) {
		for (auto j = 0; j < uniformGrid[i].size(); j++) {
			uniformGrid[i][j].clear();
		}
	}
}


