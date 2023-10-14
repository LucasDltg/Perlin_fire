#include "Perlin.hpp"
#include <SDL2/SDL.h>
#include <vector>

using namespace std;

inline uint32_t Color_gradient_linear(uint32_t color1, uint32_t color2, float percentage)
{
    float dr = ((uint8_t)(color2 >> 24)-(uint8_t)(color1 >> 24));
    float dg = ((uint8_t)(color2 >> 16)-(uint8_t)(color1 >> 16));
    float db = ((uint8_t)(color2 >>  8)-(uint8_t)(color1 >>  8));

    uint32_t color = (uint8_t)((uint8_t)(color1 >> 24) + percentage*dr) << 24 | (uint8_t)((uint8_t)(color1 >> 16) + percentage*dg) << 16 | (uint8_t)((uint8_t)(color1 >> 8) + percentage*db) << 8;
    
    return color;
}

int main(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] > %s", SDL_GetError());
        return 1;
    }
    SDL_Window *win; SDL_Renderer *re;
    SDL_CreateWindowAndRenderer(800, 800, 0, &win, &re);
    if(win == NULL || re == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] > %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowTitle(win, "Perlin Noise");

    SDL_SetRenderDrawBlendMode(re, SDL_BLENDMODE_NONE);
    SDL_bool run = SDL_TRUE;
    SDL_Event events;
    Perlin p(LINEAR_INTERPOLATION, 0);
    float x_max=6, y_max=6, x_min=0, y_min=0;
    int x_points = 600, y_points=600;
    double z = 0.0f;
    while(run)
    {
        while(SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                run = SDL_FALSE;
            break;
            case SDL_KEYDOWN:
                switch(events.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    run = SDL_FALSE;
                break;
                }
            break;

            }
        }

        vector<uint32_t> image;
        for(int j(0);j<y_points+1;j++)
        {
            vector<uint32_t> t;
            for(int i(0);i<x_points+1;i++)
            {
                uint8_t gray = (uint8_t)(255*p.perlinNoise(x_min + (float)i*(x_max-x_min)/(float)x_points, y_min + (float)j*(y_max-y_min)/(float)y_points, z, 5, 0.6));

                uint32_t red = 0xFF000000;
                uint32_t yellow = 0xFFFF0000;
                uint32_t black = 0x00000000;
                uint32_t white = 0xFFFFFF00;
                uint32_t color;


                gray *= 255;
                gray += -75+150*(float)j/(float)y_points;
                
                if(gray < 140)
                    color = black;
                else if(gray < 170)
                    color = Color_gradient_linear(red, yellow, (gray-170.0f)/30.0f);
                else if(gray < 200)
                    color = Color_gradient_linear(white, yellow, (gray-200.0f)/55.0f);
                else
                    color = white;

                color |= (uint8_t)(255*(float)j/(float)y_points);

                

                image.push_back(color);

            }
        }
        z+=0.1f;

        SDL_Surface *s = SDL_CreateRGBSurfaceFrom(&image[0], x_points+1, y_points+1, 32, (1+x_points)*4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
        SDL_Texture *text = SDL_CreateTextureFromSurface(re, s);

        SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
        SDL_RenderClear(re);
        SDL_Rect dest = {100, 100, x_points, y_points};
        SDL_RenderCopy(re, text, nullptr, &dest);
        
        
        SDL_RenderPresent(re);
    }
    
    SDL_DestroyRenderer(re);
    SDL_DestroyWindow(win);
    return 0;
}