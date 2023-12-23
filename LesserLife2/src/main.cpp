#include "Simulation.h"



int main()
{
	SimulationParameters params;
	params.numberOfPoints = 1000;
	params.screenWidth = 1920;
	params.screenHeight = 1080;
	params.gridFactor = 32;
	params.game_area_size_factor =4;
	params.numberOfPoints = 4500;
	params.gridFactor = 32;
	params.frictionFactor = 0.89090998509950749950;
	params.soften = -2.0;
	params.amplify = 100.0;
	params.timeStep = 0.0015555125425491;
	params.beta = 0.541814362515;
	params.numColors = 15;
	params.bounceAmount = 0.999599998495;
	params.min_interact_distance = 80;
	params.size = 4;
	params.mass = 4.5;
	params.pointSize = 4;

	//const int screenWidth = 256*4;
	//const int screenHeight = 256*4;
	const int screenWidth = 1920;
	const int screenHeight = 1080;//I also need to create a GUI for changing the size of the points





    PointRenderer pointRenderer(screenWidth, screenHeight, &params);

    //the problem is that the GUI is inside the pointRenderer class but it doesn't ahve access to all the parameters
    //i need to pass the parameters to the pointRenderer class
    //I could pass the parameters to the pointRenderer class




 //   Simulation sim(params.numberOfPoints, params.numColors, screenWidth, screenHeight, params.gridFactor, params.beta,
 //   					params.frictionFactor, params.min_interact_distance, params.soften, params.amplify, params.timeStep,
//						params.bounceAmount, params.size, params.mass, params.game_area_size_factor, pointRenderer);
    Simulation sim(screenWidth, screenHeight, &params, pointRenderer);
    sim.run();

    return 0;
}

