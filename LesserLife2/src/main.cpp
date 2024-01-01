#include "Simulation.h"



int main()
{
	SimulationParameters params;
	params.numberOfPoints = 1000;
	params.screenWidth = 1366;
	params.screenHeight = 768;
	params.gridFactor = 16;
	params.game_area_size_factor =32;
	params.numberOfPoints = 4500;
	params.gridFactor = 128;
	params.frictionFactor = 0.89090998509950749950;
	params.soften = 0.3;
	params.amplify = 100.0;
	params.timeStep = 0.0015555125425491;
	params.beta = 0.541814362515;
	params.numColors = 8;
	params.bounceAmount = 0.999599998495;
	params.min_interact_distance = 80;
	params.min_interact_distance_squared = params.min_interact_distance*params.min_interact_distance;
	//params.size = 8;
	params.mass = 99.5;
	params.pointSize = 4;
	params.paletteIndex = 1;

    PointRenderer pointRenderer(params.screenWidth, params.screenHeight, &params);

    //the problem is that the GUI is inside the pointRenderer class but it doesn't ahve access to all the parameters
    //i need to pass the parameters to the pointRenderer class
    //I could pass the parameters to the pointRenderer class

    //   Simulation sim(params.numberOfPoints, params.numColors, screenWidth, screenHeight, params.gridFactor, params.beta,
    //   					params.frictionFactor, params.min_interact_distance, params.soften, params.amplify, params.timeStep,
    //						params.bounceAmount, params.size, params.mass, params.game_area_size_factor, pointRenderer);
    Simulation sim(params.screenWidth, params.screenHeight, &params, pointRenderer);
    sim.run();

    return 0;
}


