#include "GridCell.h"

class Grid {
public:
    int width, height;
    std::vector<std::vector<GridCell>> cells;

    int getWidth(){ return width;}
    int getHeight(){ return height;}
    //Grid(){}

    Grid(int width, int height) : width(width), height(height) {
        // Resize outer vector
        cells.resize(width);

        // Initialize each GridCell with its coordinates
        for (int x = 0; x < width; x++) {
            // Resize inner vector for this column
            cells[x].resize(height);

            for (int y = 0; y < height; y++) {
                // Use emplace to construct GridCell in place
                cells[x][y] = GridCell(x, y);
            }
        }
    }

    GridCell& getCell(int x, int y) {
        return cells[x][y];
    }

    std::vector<GridCell*> getNeighbors(int x, int y) {
        std::vector<GridCell*> neighbors;
        const int depth = 2;
        for (int i = -depth; i <= depth; i++) {
        	int nx = (x + i);
        	if(nx >= width){
        		nx -= width;
        	}else if(nx < 0){
        		nx += width;
        	}
        	for (int j = -depth; j <= depth; j++) {
                //if (i == 0 && j == 0) continue;  // Exclude the input cell itself
                //int nx = (x + i + width) % width;
                //int ny = (y + j + height) % height;

            	int ny = (y + j);
				if(ny >= height){
					ny -= height;
				}else if(ny < 0){
					ny += height;
				}

                neighbors.push_back(&this->cells[nx][ny]);
            }
        }
        return neighbors;
    }


    void addPointToInitialCell(Point& point, int cellSizeX, int cellSizeY, int offsetX, int offsetY) {
        // Shift point's coordinates to positive range
        float shiftedX = point.x + offsetX;
        float shiftedY = point.y + offsetY;

        int cellX = static_cast<int>(shiftedX / cellSizeX);
        int cellY = static_cast<int>(shiftedY / cellSizeY);
        point.cellX = cellX;
        point.cellY = cellY;
        cells[cellX][cellY].addPoint(point);
    }


};
