// Simulation.cpp
#include <iostream>
#include "Simulation.h"

Simulation::Simulation(int width, int height, SimulationParameters* params, PointRenderer& renderer)
: pointRenderer(renderer), screenWidth(width), screenHeight(height), simParams(params),
	cellSizeX(static_cast<int>(width * params->game_area_size_factor / params->gridFactor + 0.0)),
    cellSizeY(static_cast<int>(height * params->game_area_size_factor / params->gridFactor + 0.0)),
    grid(std::make_unique<Grid>(cellSizeX, cellSizeY)) {
	frameCounter = 0;
	area_w = screenWidth*simParams->game_area_size_factor;
	area_h = screenHeight*simParams->game_area_size_factor;
	simParams->size = simParams->size / simParams->game_area_size_factor;

	ax = -area_w/2;
	bx = area_w/2;
	ay = -area_h/2;
	by = area_h/2;

/*	ax = 0;
	bx = area_w;
	ay = 0;
	by = area_h;
*/

	std::random_device rd;
		std::mt19937 gen(rd());
    std::uniform_int_distribution<int> posxDist(0, area_w);
    std::uniform_int_distribution<int> posyDist(0, area_h);
    std::uniform_int_distribution<int> velDist(-1000, 1000);
    std::uniform_int_distribution<int> colorDist(0, simParams->numColors - 1);

    simParams->colorAttractions.resize(simParams->numColors);
    for (int i = 0; i < simParams->numColors; ++i){
    	simParams->colorAttractions[i].resize(simParams->numColors);
    	for (int j = 0; j < simParams->numColors; ++j){
    		simParams->colorAttractions[i][j] = velDist(gen)/1000.0 * 0.1255;
    	}
    }

    //configureChasingBehavior(numColors);

    for(int i = 0; i < simParams->numColors; ++i) {
        for(int j = 0; j < simParams->numColors; ++j) {
            std::cout << simParams->colorAttractions[i][j] << " ";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < simParams->numberOfPoints; ++i)
    {
    	// Randomly assign a color index to each point
    	int colorIndex = colorDist(gen);
        int xPos = posxDist(gen);
        int yPos = posyDist(gen);
        int velocityX = 0;//velDist(gen);
        int velocityY = 0;//velDist(gen);
        points.emplace_back(colorIndex, xPos + ax, yPos + ay, velocityX, velocityY, simParams->pointSize, 1.0);
     }

    pointRenderer.optimisePoints(points);
    // Assume `points` is a std::vector<Point> containing your points
    for (auto& point : points) {
        grid->addPointToInitialCell(point, cellSizeX, cellSizeY, (int)-ax, (int)-ay);
    }

   //configureChasingBehavior(numColors);
   //rganisePointsInGrid();
}



void Simulation::configureChasingBehavior(int numColors)
{
    for (int i = 0; i < numColors; ++i)
    {
        for (int j = 0; j < numColors; ++j)
        {
            // Default to -0.2 for all attractions
        	simParams->colorAttractions[i][j] = 0.15;
        }

        // Set self-repulsion to -1.8
        simParams->colorAttractions[i][i] = -1.0;

        // Set attraction to the next color to 0.8
        if (i == numColors - 1)
        {
            // If this is the last color, it should chase the first color
        	simParams->colorAttractions[i][(i + 1) % numColors] = 1.00;
        }
        else
        {
            // Otherwise, chase the next color
        	simParams->colorAttractions[i][i + 1] = 1.0;
        }

        // Set repulsion from the previous color to -0.4
        if (i == 0)
        {
            // If this is the first color, repel from the last color
        	simParams->colorAttractions[i][(i - 1 + numColors) % numColors] = -0.754;
        }
        else
        {
            // Otherwise, repel from the previous color
        	simParams->colorAttractions[i][i - 1] = -0.754;
        }
    }
}

void Simulation::run() {
    while (pointRenderer.isWindowOpenAndSetKey()) {
        auto startTime = std::chrono::high_resolution_clock::now();
        updateEvents(); // Call the new method here
        //updatePoints();
        updatePointsWithGrid();
        renderPoints();
        controlFrameRate(startTime);
    }
}

//This method will check to see if things have changed and if so, apply the new parameters, maybe recreate the points array
//IDeally this will see if the numberOfPOint has gone down and remove some points to meet the new parameter
//if more or required they will be added
void Simulation::applyNewParameters(){
	std::uniform_int_distribution<> colorDist(0, simParams->numColors - 1);

	    if (simParams->numberOfPoints > points.size()) {
	    	std::uniform_int_distribution<> posxDist(0, area_w- 1);
	    	std::uniform_int_distribution<> posyDist(0, area_h - 1);

	    	std::random_device rd;
	    	std::mt19937 gen(rd());

	    	while (points.size() < simParams->numberOfPoints) {
	            int colorIndex = colorDist(gen);
	            int xPos = posxDist(gen);
	            int yPos = posyDist(gen);
	            int velocityX = 0; // Replace with actual velocity if needed
	            int velocityY = 0; // Replace with actual velocity if needed
	            Point newPoint = Point(colorIndex, xPos, yPos, velocityX, velocityY, simParams->pointSize, 1.0);

			   // Add the point to the vector
			   points.emplace_back(newPoint);
			   pointRenderer.optimisePoints(points);
			   // Add the point to the grid system
			   grid->addPointToInitialCell(newPoint, cellSizeX, cellSizeY, -ax, -ay);
	        }
	    } else if (simParams->numberOfPoints < points.size()) {
	        points.erase(points.begin() + simParams->numberOfPoints, points.end());
	    }


	if(simParams->numColors != simParams->colorAttractions.size()){
		applyColorChanges();
	}
}

void Simulation::applyColorChanges() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> velDist(-1000, 1000);

    // Resize colorAttractions if it's smaller than numColors
    if (simParams->colorAttractions.size() < simParams->numColors) {
        simParams->colorAttractions.resize(simParams->numColors);
        for (int i = 0; i < simParams->numColors; ++i) {
            // Resize inner vectors only if needed and assign random values to new elements
            if (simParams->colorAttractions[i].size() < simParams->numColors) {
                size_t oldSize = simParams->colorAttractions[i].size();
                simParams->colorAttractions[i].resize(simParams->numColors);
                for (size_t j = oldSize; j < simParams->colorAttractions[i].size(); ++j) {
                    simParams->colorAttractions[i][j] = 0;//velDist(gen) / 1000.0 * 0.1255;
                }
            }
        }
    } else if (simParams->colorAttractions.size() > simParams->numColors) {
        // If the current size is larger, reduce the size of each vector
        for (auto& row : simParams->colorAttractions) {
            row.resize(simParams->numColors);
        }
        // Reduce the outer vector size
        simParams->colorAttractions.resize(simParams->numColors);
    }

    // Remove points with invalid color index and update numberOfPoints
    auto newEnd = std::remove_if(points.begin(), points.end(), [this](const Point& p) {
        return p.color_index >= simParams->numColors;
    });
    points.erase(newEnd, points.end());

    // Update numberOfPoints
    simParams->numberOfPoints = static_cast<int>(points.size());
}


void Simulation::updateEvents() {
	if(pointRenderer.randomPressed){
		// If you need to map the mouse coordinates to simulation coordinates, use the mapping methods:
		double simX = mapMouseXtoSimulationX(pointRenderer.mouseX) - area_w/2;
		double simY = mapMouseYtoSimulationY(pointRenderer.mouseY)- area_h/2;


		std::cout << "cREATING POINT AT SIMx: " << simX << " AND SIMy: " << simY << std::endl;
	    //addPointAtMousePosition(simX, simY);
		//randomizeAttractions();
		//for std::vector<Point> points

		std::random_device rd;
		std::mt19937 gen(rd());
	    std::uniform_int_distribution<int> posxDist(0, area_w);
	    std::uniform_int_distribution<int> posyDist(0, area_h);
	    std::uniform_int_distribution<int> velDist(-1000, 1000);
	    for (int i = 0; i < simParams->numberOfPoints; ++i)
		{
			// Randomly assign a color index to each point
			//int colorIndex = colorDist(gen);
			int xPos = posxDist(gen);
			int yPos = posyDist(gen);
			//points.emplace_back(colorIndex, xPos + ax, yPos + ay, velocityX, velocityY, simParams->pointSize, 1.0);
			points[i].x = xPos;
			points[i].y = yPos;
		 }
	} else if (simParams->applyRequired) {
		applyNewParameters();
		simParams->applyRequired = false;
	}
}




void Simulation::organisePointsInGrid(){
	// Calculate the number of rows and columns in the grid.
	double gridScale = 1.0/4; // Value between 0 and 1

	// Calculate the number of rows and columns in the grid.
	int numRows = sqrt(numberOfPoints);
	int numCols = ceil((double)numberOfPoints / numRows);

	// Calculate the space between points in the x and y directions based on the grid scale.
	int xposInc = area_w * gridScale / numCols;
	int yposInc = area_h * gridScale / numRows;

	// Calculate the total width and height of the grid and the offsets needed to center it.
	int gridWidth = xposInc * numCols;
	int gridHeight = yposInc * numRows;
	int xOffset = (area_w - gridWidth) / 2 - area_w;
	int yOffset = (area_h - gridHeight) / 2;

	int xPos = xOffset;
	int yPos = yOffset + area_h/2;

	for (int i = 0; i < numberOfPoints; ++i)
	{
		// Sequentially assign a color index to each point
		int colorIndex = i % 12;

		// If the next point would exceed the grid width, reset xPos and increase yPos
		if (xPos + xposInc > xOffset + gridWidth)
		{
			yPos += yposInc;
			xPos = xOffset;
		}

		int velocityX = 0;
		int velocityY = 0;

		points.emplace_back(colorIndex, xPos, yPos, velocityX, velocityY, simParams->pointSize, 1.0);

		// Increment the x position for the next point
		xPos += xposInc;
	}
}

void Simulation::updatePoints() {
	// Update positions based on velocities
	for (auto& point : points)
	{
		// Apply friction to velocity
		point.vx *= simParams->frictionFactor;
		point.vy *= simParams->frictionFactor;

		point.x += point.vx * (simParams->timeStep+pointRenderer.timestep);
		point.y += point.vy * (simParams->timeStep+pointRenderer.timestep);

		if(point.x > bx){ point.x -= (bx-ax); }
		else if(point.x < ax){ point.x += (bx-ax); }
		if(point.y > by){ point.y -= (by-ay); }
		else if(point.y < ay){ point.y += (by-ay);}

		// Apply physical interactions
	   for (auto& otherPoint : points)
	   {
		   if (&point != &otherPoint) // Exclude self-interaction
		   {
			   handleInteraction(point, otherPoint);
		   }
	   }
	}

    frameCounter++;
}


void Simulation::updatePointsWithGrid() {
	// Update positions based on velocities and grid cells
	for (auto& point : points)
	{
	    // Apply friction to velocity
	    point.vx *= simParams->frictionFactor;
	    point.vy *= simParams->frictionFactor;

	    // Calculate new position
	    float newX = point.x + (point.vx * (simParams->timeStep+pointRenderer.timestep));
	    float newY = point.y + (point.vy * (simParams->timeStep+pointRenderer.timestep));

	    if(newX - ax < 0){
	    	//std::cout << "OH SHIT!" << std::endl;
	    }

		int newGridX = (static_cast<int>((newX - ax) / cellSizeX + 0.0) + grid->getWidth()) % grid->getWidth();
		int newGridY = (static_cast<int>((newY - ay) / cellSizeY + 0.0) + grid->getHeight()) % grid->getHeight();


	    //int newGridX = (static_cast<int>((newX - ax) / cellSizeX + 0.0));
   		//int newGridY = (static_cast<int>((newY - ay) / cellSizeY + 0.0));


		// Wrap grid coordinates if necessary
		if(newGridX < 0){
			newGridX += grid->getWidth();
			//std::cout << "OH SHIT!" << std::endl;
		}else if(newGridX >= grid->getWidth()){
			newGridX -= grid->getWidth();
			//std::cout << "OH SHIT!" << std::endl;
		}

		if(newGridY < 0){
			newGridY += grid->getHeight();
			//std::cout << "OH SHIT!" << std::endl;
		}
		else if(newGridY >= grid->getHeight())
		{
			newGridY -= grid->getHeight();
			//std::cout << "OH SHIT!" << std::endl;
		}

	    // Update grid cells
	    GridCell& oldCell = grid->getCell(point.cellX, point.cellY);
	    GridCell& newCell = grid->getCell(newGridX, newGridY);
	    if (&oldCell != &newCell) {
	        oldCell.removePoint(point);
	        newCell.addPoint(point);

	        // Update the point's grid cell coordinates
	        point.cellX = newGridX;
	        point.cellY = newGridY;
	    }

	    if(newX > bx){ newX -= (bx-ax); }
	    else if(newX < ax){ newX += (bx-ax); }
	    if(newY > by){ newY -= (by-ay); }
	    else if(newY < ay){ newY += (by-ay);}

	    // Update the point's position
	    point.x = newX;
	    point.y = newY;
	}

	for (auto& point : points)
	{
	 //   GridCell& cell = grid->getCell(point.cellX, point.cellY);

	    // Check points in the same cell
	   // for (auto& otherPoint : cell.points) {
	   //     if (&point != otherPoint) {
	        	// handleInteraction(point, *otherPoint);
	   //     }
	   // }

	    // Check points in neighboring cells
	    for (auto& neighbor : grid->getNeighbors(point.cellX, point.cellY)) {
	        for (auto& otherPoint : neighbor->points) {
	        	if (&point != otherPoint) {
	        		handleInteraction(point, *otherPoint);
	        	}
	        }
	    }
	}

    frameCounter++;
}


void Simulation::handleInteraction(Point& point, Point& otherPoint) {
    // Calculate distance between the points
    float dx = otherPoint.x - point.x;
    float dy = otherPoint.y - point.y;

    // Wrap around the coordinates if necessary
    if (dx > bx){dx -= (bx - ax);}
    else if (dx < ax){dx += (bx - ax);}
    if (dy > by){dy -= (by - ay);}
    else if (dy < ay){dy += (by - ay);}

/*
	float dx = otherPoint.x - point.x;
	float dy = otherPoint.y - point.y;

	// Handle wrap-around for x coordinate
	if (std::abs(dx) > (bx - ax) / 2) {
	    dx = (dx > 0) ? dx - (bx - ax) : dx + (bx - ax);
	}

	// Handle wrap-around for y coordinate
	if (std::abs(dy) > (by - ay) / 2) {
	    dy = (dy > 0) ? dy - (by - ay) : dy + (by - ay);
	}
*/
    // Calculate the distance using the wrapped coordinates
    float distance = dx * dx + dy * dy;

float tempDistance = distance;
    	distance = std::sqrt(distance);
    if (distance < simParams->min_interact_distance) {

        // Calculate the overlap and push distance
        float overlap = otherPoint.size + point.size + otherPoint.size + point.size - distance;
        if (distance < otherPoint.size + point.size + otherPoint.size + point.size) {

            // Exchange velocities because the masses are equal
            float tempVx = point.vx;
            float tempVy = point.vy;

            const float bounceFactor = simParams->bounceAmount;
            point.vx = otherPoint.vx * bounceFactor;
            point.vy = otherPoint.vy * bounceFactor;

            otherPoint.vx = tempVx * bounceFactor;
            otherPoint.vy = tempVy * bounceFactor;

            // Re-position points to prevent overlap
            point.x -= overlap / 2 * dx / distance;
            point.y -= overlap / 2 * dy / distance;
            otherPoint.x += overlap / 2 * dx / distance;
            otherPoint.y += overlap / 2 * dy / distance;
        }

        // Calculate attraction based on color indices
        float attraction = simParams->colorAttractions[point.color_index][otherPoint.color_index];


		float normalisedDistance = distance / simParams->min_interact_distance;
		float magnitude = 0.0;
		if ( normalisedDistance < simParams->beta ){
			magnitude = normalisedDistance / simParams->beta - 1;
		}else if( simParams->beta  < normalisedDistance && normalisedDistance < 1 ){
			magnitude = attraction * (1 - abs(2 * normalisedDistance - 1 - simParams->beta) / (1 - simParams->beta));
		}

        // Update velocities based on attraction and distance
        //float magnitude = amplify *(attraction / (distance - 1.0)); // Soften the effect

		///but wait, if the particles are really close - we want them to push each other away:
		//if(distance < point.size*2){
		//	magnitude = attraction * (1 - abs(2 * distance - 1 - 0.3) / (1 - 0.3));
		//}

		if(magnitude > 1) magnitude = 1;
		if(magnitude < -1) magnitude = -1;

		magnitude *= simParams->amplify;

		//std::cout << "magnitude: " << magnitude << std::endl;

        point.vx += (dx * magnitude);
        point.vy += (dy * magnitude);
    }


	// I want the effect of this to make particles push away from each other when they are very close
	//float near_push = (soften / (distance)); // Soften the effect

	//point.vx += (dx * near_push);
	//point.vy += (dy * near_push);
	//otherPoint.vx -= (dx * near_push);
	//otherPoint.vy -= (dy * near_push);

}


void Simulation::renderPoints() {
    pointRenderer.renderPoints(points, simParams->game_area_size_factor, area_w/2, area_h/2);
}

void Simulation::controlFrameRate(const std::chrono::high_resolution_clock::time_point& startTime) {

	// Calculate elapsed time and sleep if necessary for a smooth 60fps
	auto endTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	long long frameTimeMicros = 1000000 / 60; // Microseconds per frame for 60fps

	if (elapsedTime < std::chrono::microseconds(frameTimeMicros))
	{
		auto sleepTime = std::chrono::microseconds(frameTimeMicros) - elapsedTime;
		std::this_thread::sleep_for(sleepTime);
	}

	// Update the end time after sleeping
	endTime = std::chrono::high_resolution_clock::now();
	elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

	// Calculate and print FPS (frames per second)
	float frameTimeSecs = elapsedTime.count() / 1e6; // Convert microseconds to seconds
	float fps = 1.0 / frameTimeSecs; // Frames per second
	if (frameCounter % 60 == 0){
		std::cout << "FPS: " << fps << std::endl;
		if(fps < 30){
			//randomizeAttractions();
		}
	}
}

void Simulation::addPointAtMousePosition(int mouseX, int mouseY) {
    // Assuming you have a way to determine the color index for the new point
    int colorIndex = pointRenderer.key-1;//determineColorIndex();

    // You might need to map the mouse coordinates to your simulation's coordinate system
    float xPos = mapMouseXtoSimulationX(mouseX);
    float yPos = mapMouseYtoSimulationY(mouseY);

    // Set initial velocity, size, etc., as needed
    int velocityX = 0;
    int velocityY = 0;

    // Create a new point and add it to the points vector
    points.emplace_back(colorIndex, xPos, yPos, velocityX, velocityY, simParams->pointSize, 1.0);

    // You may also need to add the point to the grid and update the renderer
    grid->addPointToInitialCell(points.back(), cellSizeX, cellSizeY, -ax, -ay);
    pointRenderer.optimisePoints(points);
}

double Simulation::mapMouseXtoSimulationX(int mouseX) {
    // Assuming the simulation's x-axis ranges from simMinX to simMaxX
    double simMinX = 0.0;
    double simMaxX = area_w;

    // Assuming the window's x-axis ranges from winMinX to winMaxX
    int winMinX = 0;
    int winMaxX = screenWidth; // Assuming windowWidth is the width of your window

    // Linearly map mouseX from the window's coordinate system to the simulation's coordinate system
    return simMinX + (mouseX - winMinX) * (simMaxX - simMinX) / (winMaxX - winMinX);
}

double Simulation::mapMouseYtoSimulationY(int mouseY) {
    // Assuming the simulation's y-axis ranges from simMinY to simMaxY
    double simMinY = 0.0;
    double simMaxY = area_h;

    // Assuming the window's y-axis ranges from winMinY to winMaxY
    int winMinY = 0;
    int winMaxY = screenHeight; // Assuming windowHeight is the height of your window

    // Linearly map mouseY from the window's coordinate system to the simulation's coordinate system
    return simMinY + (mouseY - winMinY) * (simMaxY - simMinY) / (winMaxY - winMinY);
}
