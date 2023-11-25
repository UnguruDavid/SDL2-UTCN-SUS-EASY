#include <SDL.h>
#include <stdio.h>
#include <SDL_messagebox.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CUBE_SIZE = 50;
const int SMALL_CUBE_SIZE = 10;
const int MAX_SMALL_CUBES = 10;

// Function to check for collision between two SDL_Rect structures
bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Moving Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Rect cubeRect = { 0, 0, CUBE_SIZE, CUBE_SIZE };
    SDL_Rect lowerCubeRect = { 0, 4 * SCREEN_HEIGHT / 4 - CUBE_SIZE / 2, CUBE_SIZE, CUBE_SIZE };

    // Array to store small cubes
    SDL_Rect smallCubes[MAX_SMALL_CUBES];

    // Initialize small cubes
    for (int i = 0; i < MAX_SMALL_CUBES; ++i) {
        smallCubes[i] = { 0, 0, SMALL_CUBE_SIZE, SMALL_CUBE_SIZE };
    }

    bool quit = false;
    SDL_Event e;
    int k = 0;
    int g = 1;
    int shootIndex = 0;

    Uint32 lastMoveTime = SDL_GetTicks(); // Get the initial time

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    lowerCubeRect.x -= 10;
                    // Check if lowerCube is going out of the left boundary
                    if (lowerCubeRect.x < 0) {
                        lowerCubeRect.x = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    lowerCubeRect.x += 10;
                    // Check if lowerCube is going out of the right boundary
                    if (lowerCubeRect.x + lowerCubeRect.w > SCREEN_WIDTH) {
                        lowerCubeRect.x = SCREEN_WIDTH - lowerCubeRect.w;
                    }
                    break;
                case SDLK_SPACE:
                    // Shoot small cube
                    smallCubes[shootIndex].x = lowerCubeRect.x + CUBE_SIZE / 2 - SMALL_CUBE_SIZE / 2;
                    smallCubes[shootIndex].y = lowerCubeRect.y;
                    shootIndex = (shootIndex + 1) % MAX_SMALL_CUBES;
                    break;
                }
            }
        }

        // Move the cube down every 5 seconds
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= 5000) {
            cubeRect.y += 30;
            lastMoveTime = currentTime;
        }

        // Move the cube to the right
        if (cubeRect.x + cubeRect.w < SCREEN_WIDTH && k == 0) {
            cubeRect.x += 2;
        }

        if (!(cubeRect.x + cubeRect.w < SCREEN_WIDTH && k == 0)) {
            k = 1;
            g = 0;
            cubeRect.x = cubeRect.x - 2;
            // Cube reached the right edge, you can add some additional logic here if needed
        }

        if (cubeRect.x == 0.0f && k == 1) {
            k = 0;
            cubeRect.x += 2;
        }

        // Check for collision with the player's cube
        if (cubeRect.y >= SCREEN_HEIGHT - cubeRect.h / 2) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "Sorry, you lost the game.", NULL);
            quit = true;
        }

        // Move small cubes upwards and handle collisions
        for (int i = 0; i < MAX_SMALL_CUBES; ++i) {
            if (smallCubes[i].y > 0) {
                smallCubes[i].y -= 2;

                // Check for collision with the normal cube
                if (checkCollision(smallCubes[i], cubeRect)) {
                    // If collision detected, mark the small cube as inactive
                    smallCubes[i].y = -SMALL_CUBE_SIZE;  // Move it outside the visible screen
                    cubeRect.y = -200;
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "You win!", "Congratulations! You won the game.", NULL);

                    // Set quit to true to exit the main loop
                    quit = true;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        // Draw the cube
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &cubeRect);

        // Draw the lower cube
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &lowerCubeRect);

        // Draw small cubes
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        for (int i = 0; i < MAX_SMALL_CUBES; ++i) {
            // Only draw active small cubes
            if (smallCubes[i].y > 0) {
                SDL_RenderFillRect(renderer, &smallCubes[i]);
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Add a small delay to slow down the movement
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    //return 0;
}