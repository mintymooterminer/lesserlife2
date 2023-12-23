#ifndef POINT_H
#define POINT_H
#include <vector>
#include <SDL2/SDL.h>

class Point
{
public:

	int color_index;
    int cellX;
    int cellY;

	float x;
    float y;
    float vx;
    float vy;
    SDL_Color color;
    int size;
    float mass;
    float inverseMass;
    bool isOptimised;
    SDL_Texture* texture;

    Point(int colorIndex, float xPos, float yPos, float velocityX, float velocityY,
          int pointSize, float pointMass)
        : color_index(colorIndex), x(xPos), y(yPos), vx(velocityX), vy(velocityY),
          size(pointSize), mass(pointMass),
          inverseMass(1.0 / pointMass)
    {
    	cellX=0;
    	cellY=0;
    	texture = nullptr;
    	isOptimised = false;
    }

    void setVelocityX(float newVx)
    {
        vx = newVx;
    }


    void setVelocityY(float newVy)
    {
        vy = newVy;
    }

    void draw(SDL_Renderer* renderer, int scale, int bx, int by) const
    {
        SDL_Color c = color;
    	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

        if(size == 1){
            SDL_RenderDrawPoint(renderer, x/scale + bx, y/scale + by);
            return;
        }

        // Calculate radius based on size
        int radius = size / 2;

        // Draw filled circle
        for (int dy = -radius; dy <= radius; dy++)
        {
            for (int dx = -radius; dx <= radius; dx++)
            {
                if (dx * dx + dy * dy <= radius * radius)
                {
                    SDL_RenderDrawPoint(renderer, (x+bx)/scale + dx , (y+by)/scale + dy);
                }
            }
        }
    }
};

#endif
