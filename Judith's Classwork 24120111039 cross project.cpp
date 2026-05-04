#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("SDL3 Test", 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // Load first image (Top)
    SDL_Surface* surface1 = IMG_Load("C:/Users/HP/Pictures/Saved Pictures/download.jpg");
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_DestroySurface(surface1);
    SDL_FRect imgrect1 = { 300, 50, 200, 200 };

    // Load second image (Bottom)
    SDL_Surface* surface2 = IMG_Load("C:/Users/HP/Pictures/Saved Pictures/download.jpg");
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_DestroySurface(surface2);
    SDL_FRect imgrect2 = { 300, 350, 200, 200 };

    // Load third image (Left)
    SDL_Surface* surface3 = IMG_Load("C:/Users/HP/Pictures/Saved Pictures/download.jpg");
    SDL_Texture* texture3 = SDL_CreateTextureFromSurface(renderer, surface3);
    SDL_DestroySurface(surface3);
    SDL_FRect imgrect3 = { 50, 200, 200, 200 };

    // Load fourth image (Right)
    SDL_Surface* surface4 = IMG_Load("C:/Users/HP/Pictures/Saved Pictures/download.jpg");
    SDL_Texture* texture4 = SDL_CreateTextureFromSurface(renderer, surface4);
    SDL_DestroySurface(surface4);
    SDL_FRect imgrect4 = { 550, 200, 200, 200 };

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) running = false;
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture1, NULL, &imgrect1);  // Top
        SDL_RenderTexture(renderer, texture2, NULL, &imgrect2);  // Bottom
        SDL_RenderTexture(renderer, texture3, NULL, &imgrect3);  // Left
        SDL_RenderTexture(renderer, texture4, NULL, &imgrect4);  // Right
        SDL_RenderPresent(renderer);
    }

    // Clean up all textures
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texture4);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}