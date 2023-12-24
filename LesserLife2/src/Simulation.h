// Simulation.h
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include "Point.h"
#include "PointRenderer.h"
#include "Grid.h"



class Simulation {
public:
    //Simulation(int numberOfPoints, int numColors, int width, int height, int _gridFactor, float beta,  float friction, float minDist, float soften,  float amplify, float timeStep, float bouncAmount, int size, float mass, float areaFactor, PointRenderer& renderer);
    Simulation(int width, int height, SimulationParameters* params, PointRenderer& renderer);
    void run();

private:

    SimulationParameters* simParams;

    //Grid grid;
    int cellSizeX;
    int cellSizeY;
    std::unique_ptr<Grid> grid;
    int frameCounter;
    int numberOfPoints= 0;
    int screenWidth = 0;
	int screenHeight = 0;

	int area_w;
	int area_h;
	float ax;
	float bx;
	float ay;
	float by;

    std::vector<Point> points;
    PointRenderer& pointRenderer;

    void organisePointsInGrid();
    void handleInteraction(Point& point, Point& otherPoint);
    void configureChasingBehavior(int numColors);
    void updatePoints();
    void updatePointsWithGrid();
    void renderPoints();
    void controlFrameRate(const std::chrono::high_resolution_clock::time_point& startTime);
    void addPointAtMousePosition(int mouseX, int mouseY);
    void applyNewParameters();
    void applyColorChanges();
    void updateEvents();
    double mapMouseXtoSimulationX(int mouseX);
    double mapMouseYtoSimulationY(int mouseY);


};
