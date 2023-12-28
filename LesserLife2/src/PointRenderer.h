#include <SDL2/SDL.h>
#include "Point.h"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include "imgui_sdl.h"

struct SimulationParameters {
    double game_area_size_factor;
    int numberOfPoints;
    int screenWidth;
    int screenHeight;
    int gridFactor;
    float frictionFactor;
    float soften;
    float amplify;
    float timeStep;
    float beta;
    int numColors;
    float bounceAmount;
    float min_interact_distance;
    //long unsigned int size;
    double mass;
    int pointSize;
    std::vector<std::vector<float>> colorAttractions;
    bool applyRequired = false;
    int paletteIndex = 1;
};

class PointRenderer
{
public:
	double timestep = 0;
	bool randomPressed	= false;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	int screenWidth;
	int screenHeight;
	SimulationParameters* simParams;
	bool running = true;
	float zoom = 1;
	int offset_x = 0;
	int offset_y = 0;

	bool isMiddleMousePressed = false;
	bool isRightMousePressed = false;
	int initialMouseX = 0; // Initial X when middle mouse was pressed
	int initialMouseY = 0; // Initial Y when middle mouse was pressed
	int initialZoomMouseX = 0;
	int initialZoomMouseY = 0;

	std::vector<SDL_Texture*> textureCache;
	std::vector<SDL_Color> originalPalette = {
	    {255, 255, 255, 255},   // White
	    {159, 78, 68, 255},     // Dark Reddish
	    {203, 126, 117, 255},   // Light Reddish
	    {109, 84, 18, 255},     // Dark Yellowish
	    {161, 104, 60, 255},    // Brownish
	    {201, 212, 135, 255},   // Light Yellowish
	    {154, 226, 155, 255},   // Light Green
	    {90, 171, 94, 255},     // Dark Green
	    {106, 191, 198, 255},   // Cyan
	    {136, 126, 203, 255},   // Light Blue
	    {80, 69, 155, 255},     // Dark Blue
	    {160, 87, 163, 255},    // Purple
	    {123, 104, 238, 255},   // Medium Slate Blue
	    {0, 191, 255, 255},     // Deep Sky Blue
	    {255, 105, 180, 255}    // Hot Pink
	};
	std::vector<SDL_Color> rainbowPalette = {
	    {255, 0, 0, 255},       // Red
	    {255, 85, 0, 255},      // Red-Orange
	    {255, 170, 0, 255},     // Orange
	    {255, 255, 0, 255},     // Yellow
	    {170, 255, 0, 255},     // Yellow-Green
	    {85, 255, 0, 255},      // Green-Yellow
	    {0, 255, 0, 255},       // Green
	    {0, 255, 85, 255},      // Green-Cyan
	    {0, 255, 170, 255},     // Cyan-Green
	    {0, 255, 255, 255},     // Cyan
	    {0, 170, 255, 255},     // Cyan-Blue
	    {0, 85, 255, 255},      // Blue-Cyan
	    {0, 0, 255, 255},       // Blue
	    {85, 0, 255, 255},      // Blue-Magenta
	    {170, 0, 255, 255}      // Magenta-Blue
	};
	std::vector<SDL_Color> grayscalePalette = {
	    {0, 0, 0, 255},         // Black
	    {17, 17, 17, 255},      // Very Dark Gray
	    {34, 34, 34, 255},
	    {51, 51, 51, 255},
	    {68, 68, 68, 255},
	    {85, 85, 85, 255},
	    {102, 102, 102, 255},
	    {119, 119, 119, 255},
	    {136, 136, 136, 255},
	    {153, 153, 153, 255},
	    {170, 170, 170, 255},
	    {187, 187, 187, 255},
	    {204, 204, 204, 255},
	    {221, 221, 221, 255},
	    {238, 238, 238, 255},   // Very Light Gray
	    {255, 255, 255, 255}    // White
	};
	std::vector<std::vector<SDL_Color>> colorPalettes = {
	    // Original Palette
	    {
	        {255, 255, 255, 255},
	        {159, 78, 68, 255},
	        {203, 126, 117, 255},
	        {109, 84, 18, 255},
	        {161, 104, 60, 255},
	        {201, 212, 135, 255},
	        {154, 226, 155, 255},
	        {90, 171, 94, 255},
	        {106, 191, 198, 255},
	        {136, 126, 203, 255},
	        {80, 69, 155, 255},
	        {160, 87, 163, 255},
	        {123, 104, 238, 255},
	        {0, 191, 255, 255},
	        {255, 105, 180, 255}
	    },
	    // Rainbow Palette
	    {
	        {255, 0, 0, 255},
	        {255, 85, 0, 255},
	        {255, 170, 0, 255},
	        {255, 255, 0, 255},
	        {170, 255, 0, 255},
	        {85, 255, 0, 255},
	        {0, 255, 0, 255},
	        {0, 255, 85, 255},
	        {0, 255, 170, 255},
	        {0, 255, 255, 255},
	        {0, 170, 255, 255},
	        {0, 85, 255, 255},
	        {0, 0, 255, 255},
	        {85, 0, 255, 255},
	        {170, 0, 255, 255}
	    },
	    // Grayscale Fade Palette
	    {
	        {0, 0, 0, 255},
	        {17, 17, 17, 255},
	        {34, 34, 34, 255},
	        {51, 51, 51, 255},
	        {68, 68, 68, 255},
	        {85, 85, 85, 255},
	        {102, 102, 102, 255},
	        {119, 119, 119, 255},
	        {136, 136, 136, 255},
	        {153, 153, 153, 255},
	        {170, 170, 170, 255},
	        {187, 187, 187, 255},
	        {204, 204, 204, 255},
	        {221, 221, 221, 255},
	        {238, 238, 238, 255},
	        {255, 255, 255, 255}
	    }
	    // Add other palettes here if needed
	};

public:
	float timeStep = 1;
	int mouseX = -1;
	int mouseY = -1;
	int key = 1;
	PointRenderer(int width, int height, SimulationParameters* params)
	: screenWidth(width), screenHeight(height), simParams(params)
	{
		SDL_Init(SDL_INIT_VIDEO);

		// Create window
		window = SDL_CreateWindow("Point Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,screenWidth, screenHeight, SDL_WINDOW_SHOWN);

		if (window == nullptr)
		{
			SDL_Log("Failed to create window: %s", SDL_GetError());
			return;
		}

		// Create renderer
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == nullptr)
		{
			SDL_Log("Failed to create renderer: %s", SDL_GetError());
			return;
		}

		IMGUI_CHECKVERSION();
		ImGuiContext* tmp = ImGui::CreateContext();
		ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui::StyleColorsDark();
        // After initializing SDL and creating SDL_Renderer
        ImGuiSDL::Initialize(renderer, screenWidth, screenHeight);
	}

	~PointRenderer()
	{
		//TODO: do this in simulation
		//for (auto& p : points)
		//{
		//    SDL_DestroyTexture(p.texture);
		//}
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void optimisePoints(std::vector<Point>& points) {
		for (auto& p : points) {
			if(!p.isOptimised){
				//p.color = this->colors[p.color_index];

				p.color = colorPalettes[simParams->paletteIndex][p.color_index];
				if (textureCache.size() > p.color_index && textureCache[p.color_index] != nullptr) {
					p.texture = textureCache[p.color_index];
				} else {
					int sf = 32;
					// Create an SDL_Surface for the point
					SDL_Surface* surface = SDL_CreateRGBSurface(0, p.size * sf, p.size * sf, 32,
							0xFF000000, // Alpha mask
							0x00FF0000, // Red mask
							0x0000FF00, // Green mask
							0x000000FF); // Blue mask

					// Transparency value, ranging from 0 (fully transparent) to 255 (fully opaque)
					Uint8 transparencyValue = 200; // You can adjust this value

					// Get the color in the correct format, with alpha channel
					Uint32 sdl_color = SDL_MapRGBA(surface->format, p.color.r, p.color.g, p.color.b, transparencyValue);

					Uint32 sdl_transparent = SDL_MapRGBA(surface->format, 0, 0, 0, 0x00); // Transparent

					// Fill the surface with transparent color
					SDL_FillRect(surface, nullptr, sdl_transparent);

					int radius = p.size;
					int diameter = radius * sf;
					for (int y = 0; y < diameter; y++) {
						for (int x = 0; x < diameter; x++) {
							int dx = radius - x;
							int dy = radius - y;
							if (dx*dx + dy*dy <= radius*radius) {
								((Uint32*)surface->pixels)[(y * surface->w) + x] = sdl_color;
							}
						}
					}
					// Convert the surface to a texture
					p.texture = SDL_CreateTextureFromSurface(renderer, surface);

					// Resize textureCache if needed and store the texture
					if (textureCache.size() <= p.color_index) {
						textureCache.resize(p.color_index + 1, nullptr);
					}
					textureCache[p.color_index] = p.texture;

					// Clean up the surface
					SDL_FreeSurface(surface);
				}

				p.isOptimised = true;
			}
		}
	}

	void updateGUI()
	{
		const float minValue = -1.0f;  // minimum float value
		const float maxValue = 1.0f;  // maximum float value
		//const ImVec4 colorStart = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Light color (e.g., white)
		//const ImVec4 colorEnd = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);   // Dark color (e.g., black)

	    // Start the ImGui frame
	    ImGui_ImplSDL2_NewFrame(window);
	    ImGui::NewFrame();

	    // Here you can create your GUI
	    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("Control Panel");

	    //ImGui::SliderInt("Time Step", 0, 0, 100);
		ImGui::SliderFloat("Time Step", &simParams->timeStep, 0.0f, 0.01f, "%0.4f");

	    //double game_area_size_factor;
	    //int numberOfPoints;
	    //int screenWidth;
	    //int screenHeight;
	    //int gridFactor;
	    ImGui::SliderFloat("Friction", &simParams->frictionFactor, 0.0, 1.0);
	    //float soften;
	    ImGui::SliderFloat("Amplify", &simParams->amplify, -1000, 1000);
	    ImGui::SliderFloat("min_interact_distance", &simParams->min_interact_distance, 10, 1000);

	    //float timeStep;
	    ImGui::SliderFloat("Beta", &simParams->beta, -1.0, 1.0);
	    //int numColors;
	    //float bounceAmount;
	    ImGui::SliderFloat("Bounce Amount", &simParams->bounceAmount, 0.0, 1.0);
	    //double min_interact_distance;
	    //int size;
	    //double mass;
	    //int pointSize;
	    ImGui::SliderFloat("Soften", &simParams->soften, -1.0, 1.0, "%0.2f");
	    int originalNumberOfPoints = simParams->numberOfPoints;
	    bool hasChanged = false;
	    if (ImGui::SliderInt("Number of Points", &simParams->numberOfPoints, 1, 8000)) {
	            // This block is executed when the slider value changes
	            // You can implement additional logic here if needed
	    	simParams->applyRequired = true;
		}

	    if (ImGui::SliderInt("Number of Colors", &simParams->numColors, 1, 15)) { // Adjust 'maxColors' as necessary
	        //applyColorChanges();
	    	//simParams->applyRequired = true;
	    }


		// Check if numberOfPoints has been modified
		hasChanged = simParams->numberOfPoints != originalNumberOfPoints;

		if(hasChanged){
			simParams->applyRequired = true;
		}

		if (ImGui::SliderInt("Palette Index", &simParams->paletteIndex, 0, colorPalettes.size() - 1)) {
		    // Update the palette
		    //updatePalette();
		}

		// Apply button - only enabled if numberOfPoints has changed
		if (!simParams->applyRequired) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button("Apply")) {
			// Handle the apply action
			// For example, update the originalNumberOfPoints
			//originalNumberOfPoints = simParams->numberOfPoints;
			// Additional logic for applying the change can go here
			//simParams->applyRequired = false;
		}

		if (!hasChanged) {
			ImGui::EndDisabled();
		}


	    randomPressed = false;
	    // Add a button

	    // ... Add more controls as needed
	    ImGui::End();

	    ImGui::Begin("Color Grid");

	    ImVec4 colorPositive = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
		ImVec4 colorNegative = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		ImVec4 colorZero = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);     // Black

		const ImVec2 buttonSize = ImVec2(20.0f, 20.0f); // Adjust size as needed
if(true){
		for (size_t i = 0; i < simParams->colorAttractions.size(); ++i) {
			for (size_t j = 0; j < simParams->colorAttractions[i].size(); ++j) {
				float value = simParams->colorAttractions[i][j];
				ImVec4 buttonColor;

				if (value > 0) {
					// Positive: Shade of green based on value
					float greenIntensity = std::min(value / maxValue, 1.0f);
					buttonColor = ImVec4(colorPositive.x, colorPositive.y * greenIntensity, colorPositive.z, 1.0f);
				} else if (value < 0) {
					// Negative: Shade of red based on value
					float redIntensity = std::min(-value / maxValue, 1.0f);
					buttonColor = ImVec4(colorNegative.x * redIntensity, colorNegative.y, colorNegative.z, 1.0f);
				} else {
					// Zero: Black
					buttonColor = colorZero;
				}

				ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
				// Create the button
				ImGui::Button(("##" + std::to_string(i) + std::to_string(j)).c_str(), buttonSize);

				// Check for mouse clicks on the button
				if (ImGui::IsItemHovered()) {
				   if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					   // Left click - increase value, but not above maxValue
					   simParams->colorAttractions[i][j] = std::min(simParams->colorAttractions[i][j] + 0.1f, maxValue);
				   }
				   if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					   // Right click - decrease value, but not below minValue
					   simParams->colorAttractions[i][j] = std::max(simParams->colorAttractions[i][j] - 0.1f, minValue);
				   }
				}
				ImGui::PopStyleColor(1);

				if (j < simParams->colorAttractions[i].size() - 1) ImGui::SameLine();
			}
		}
}
		// Add a button for resetting values
		if (ImGui::Button("Reset All Attractions")) {
		   for (auto& row : simParams->colorAttractions) {
			   std::fill(row.begin(), row.end(), 0.0f); // Set all values in the row to zero
		   }
		}

		if (ImGui::Button("Randomize All Attractions")) {
		   handleRandomizeButtonPress(simParams); // Example function call
		}

		if (ImGui::Button("Randomize Positions")) {
		   randomPressed = true;
		}
	    ImGui::End();

	    // Rendering
	    ImGui::Render();
	    ImGuiSDL::Render(ImGui::GetDrawData());
	}

	void handleRandomizeButtonPress(SimulationParameters* params) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> velDist(-1000, 1000);
		std::uniform_int_distribution<int> betaDist(30, 80);
		simParams->beta = betaDist(gen)/100;
		simParams->beta = 0.3;
		std::cout << simParams->beta << std::endl;
		for (int i = 0; i < simParams->numColors; ++i){
			for (int j = 0; j < simParams->numColors; ++j){
				simParams->colorAttractions[i][j] = velDist(gen)/1000.0 * 0.1255;
			}
		}
	}

	void renderPoints(const std::vector<Point>& points, int scale, int bx, int by)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set point color to white
		for (const auto& p : points)
		{
			//point.draw(renderer, scale, bx, by);
			//SDL_Color c = this->colors[p.color_index];
			/*
			SDL_Color c = p.color;
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

			// Calculate radius based on size
			int radius = p.size*zoom;
			int tmpx = ((p.x+bx)/scale)*zoom + offset_x;
			int tmpy = ((p.y+by)/scale)*zoom + offset_y;
			// Draw filled circle
			for (int dy = -radius; dy <= radius; dy++)
			{
				for (int dx = -radius; dx <= radius; dx++)
				{
					if (dx * dx + dy * dy <= radius * radius)
					{
						SDL_RenderDrawPoint(renderer, (tmpx + dx) ,
													  (tmpy + dy));
					}
				}
			}
			*/

			SDL_Rect rect;
			rect.x = (int)((p.x+bx)/scale - p.size);
			rect.y = (int)((p.y+by)/scale - p.size);
			//rect.x = (int)((p.x)/scale - p.size);
			//rect.y = (int)((p.y)/scale - p.size);
			rect.w = (int)(p.size * 16 * zoom);
			rect.h = (int)(p.size * 16 * zoom);

			if(rect.w <= 0){
				rect.w = 1;
			}

			// zoom
			rect.x = (int)(rect.x*zoom) + offset_x;
			rect.y = (int)(rect.y*zoom) + offset_y;


			// Render the point's texture
			SDL_RenderCopy(renderer, p.texture, NULL, &rect);

		}
		updateGUI();

		SDL_RenderPresent(renderer);
	}

	bool isWindowOpenAndSetKey()
	{
		double worldMouseX;
		double worldMouseY;
		double newScreenMouseX;
		double newScreenMouseY;
		int windowWidth = screenWidth;  // Your window's width
		int windowHeight = screenWidth; // Your window's height

		// This is the point which we'll keep stationary during zooming
		int centerX = windowWidth / 2;
		int centerY = windowHeight / 2;

		// Convert that point from screen coordinates to world coordinates
		double worldCenterX = (centerX - offset_x) / zoom;
		double worldCenterY = (centerY - offset_y) / zoom;

		mouseX = -1;
		mouseY = -1;
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			switch (e.type) {
				case SDL_QUIT:
					return false;
				break;
				case (SDL_MOUSEWHEEL):{
				    // Convert mouse screen position to world position (before zooming)
				    double worldMouseXBeforeZoom = e.motion.x / zoom + offset_x;
				    double worldMouseYBeforeZoom = e.motion.y / zoom + offset_y;

				    // Adjust zoom based on wheel direction
				    if (e.wheel.y > 0) {
				        zoom *= 1.05;  // Increase zoom by 5%
				    } else if (e.wheel.y < 0) {
				        zoom /= 1.05;  // Decrease zoom by 5%
				    }

				    // Convert the mouse screen position to the world position again (after zooming)
				    double worldMouseXAfterZoom = e.motion.x / zoom + offset_x;
				    double worldMouseYAfterZoom = e.motion.y / zoom + offset_y;

				    // Adjust the offsets to make sure the world position of the mouse stays the same after zooming
				    offset_x += worldMouseXBeforeZoom - worldMouseXAfterZoom;
				    offset_y += worldMouseYBeforeZoom - worldMouseYAfterZoom;
				break;
				}
				case SDL_MOUSEBUTTONDOWN:
					if (e.button.button == SDL_BUTTON_MIDDLE) {
						isMiddleMousePressed = true;
						initialMouseX = e.button.x;
						initialMouseY = e.button.y;
					}
					if (e.button.button == SDL_BUTTON_RIGHT) {
						isRightMousePressed = true;
						initialMouseY = e.button.y; // Only Y is needed for zoom

						initialZoomMouseX = e.button.x;
						initialZoomMouseY = e.button.y;
					}
					if(e.button.button == SDL_BUTTON_LEFT){
						mouseX = e.button.x;
						mouseY = e.button.y;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (e.button.button == SDL_BUTTON_MIDDLE) {
						isMiddleMousePressed = false;
					}
					if (e.button.button == SDL_BUTTON_RIGHT) {
						isRightMousePressed = false;
					}
					break;
				case SDL_MOUSEMOTION:
					if (isMiddleMousePressed) {
						// Assuming you want 1-to-1 mouse movement to pan ratio.
						offset_x += (e.motion.x - initialMouseX);
						offset_y += (e.motion.y - initialMouseY);
						initialMouseX = e.motion.x;
						initialMouseY = e.motion.y;
					}
					if (isRightMousePressed) {
						// World coordinates of the mouse before zoom
						//double worldMouseX = (e.motion.x - offset_x) / zoom;
						//double worldMouseY = (e.motion.y - offset_y) / zoom;

						double worldMouseX = (initialZoomMouseX - offset_x) / zoom;
						double worldMouseY = (initialZoomMouseY - offset_y) / zoom;


						// Apply zoom
						//int deltaY = e.motion.y - initialMouseY;
						int deltaY = e.motion.y - initialZoomMouseY;
						zoom += 0.0001 * deltaY;
						if (zoom <= 0.0001) zoom = 0.0001;

						// New screen coordinates after zoom
						double newScreenMouseX = worldMouseX * zoom + offset_x;
						double newScreenMouseY = worldMouseY * zoom + offset_y;

						// Adjust offsets to keep the mouse point anchored
						//offset_x += e.motion.x - newScreenMouseX;
						//offset_y += e.motion.y - newScreenMouseY;

						// Adjust offsets to keep the initial zoom point anchored
						offset_x += initialZoomMouseX - newScreenMouseX;
						offset_y += initialZoomMouseY - newScreenMouseY;

						initialMouseY = e.motion.y; // Reset the starting point for continuous zooming
					}

					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_1:
						key = 1;
						break;
					case SDLK_2:
						key = 2;
						break;
					case SDLK_3:
						key = 3;
						break;
					case SDLK_4:
						key = 4;
						break;
					case SDLK_5:
						key = 5;
						break;
					case SDLK_6:
						key = 6;
						break;
					case SDLK_7:
						key = 7;
						break;
					case SDLK_8:
						key = 8;
						break;
					case SDLK_9:
						key = 9;
						break;
					case SDLK_UP:
						//offset_y+=10;
						timestep += 0.01;
						break;
					case SDLK_DOWN:
						timestep -= 0.0001;
						//offset_y-=10;
						break;
					case SDLK_LEFT:
						offset_x+=10;
						break;
					case SDLK_RIGHT:
						offset_x-=10;
						break;
					case SDLK_PAGEUP:
						zoom += 0.01;
						// After adjusting the zoom, re-center:
						offset_x = centerX - worldCenterX * zoom;
						offset_y = centerY - worldCenterY * zoom;
						break;
					case SDLK_PAGEDOWN:
						zoom -= 0.01;
						if (zoom <= 0.01) zoom = 0.01; // Ensure zoom doesn't go too small or negative.
						// After adjusting the zoom, re-center:
						offset_x = centerX - worldCenterX * zoom;
						offset_y = centerY - worldCenterY * zoom;
						break;
					}
				    break;
			}
		}
		return true;
	}
};
