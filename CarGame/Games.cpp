#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ROAD_WIDTH = 400;
const int ROAD_SPEED = 5;
const int CAR_SPEED = 10;

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}


std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


void renderText(const std::string& text, int x, int y, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w || a.y + a.h <= b.y || a.y >= b.y + b.h);
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init()== -1) {
        std::cerr << "Failed to initialize SDL or TTF: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Car Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cerr << "Failed to create window or renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Load assets
    SDL_Texture* roadTexture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\road1.jpg", renderer);
    SDL_Texture* grassTexture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\longtree1.jpg", renderer);
    SDL_Texture* carTexture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\car.png", renderer);
    SDL_Texture* carOpp1Texture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\caropp1.png", renderer);
    SDL_Texture* carOpp2Texture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\caropp2.png", renderer);
    SDL_Texture* explosionTexture = loadTexture("D:\\SDL Games\\Car Racing Game Files\\Images\\explosion.png", renderer);

    TTF_Font* font = TTF_OpenFont("D:\\SDL Games\\Car Racing Game Files\\Consolas-Regular.ttf", 24);
    if (!roadTexture || !grassTexture || !carTexture || !carOpp1Texture || !carOpp2Texture || !explosionTexture || !font) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Rect road1 = { (WINDOW_WIDTH - ROAD_WIDTH) / 2, 0, ROAD_WIDTH, WINDOW_HEIGHT };
    SDL_Rect road2 = { (WINDOW_WIDTH - ROAD_WIDTH) / 2, -WINDOW_HEIGHT, ROAD_WIDTH, WINDOW_HEIGHT };
    SDL_Rect car = { WINDOW_WIDTH / 2 - 25, WINDOW_HEIGHT - 150, 50, 100 };

    SDL_Rect opponent1 = { (WINDOW_WIDTH - ROAD_WIDTH) / 2 + rand() % (ROAD_WIDTH - 50), -200, 50, 100 };
    SDL_Rect opponent2 = { (WINDOW_WIDTH - ROAD_WIDTH) / 2 + rand() % (ROAD_WIDTH - 50), -400, 50, 100 };

    SDL_Rect explosion = { 0, 0, 100, 100 };

    SDL_Rect grass1Left = { 0, 0, (WINDOW_WIDTH - ROAD_WIDTH) / 2, WINDOW_HEIGHT };
    SDL_Rect grass2Left = { 0, -WINDOW_HEIGHT, (WINDOW_WIDTH - ROAD_WIDTH) / 2, WINDOW_HEIGHT };
    SDL_Rect grass1Right = { (WINDOW_WIDTH + ROAD_WIDTH) / 2, 0, (WINDOW_WIDTH - ROAD_WIDTH) / 2, WINDOW_HEIGHT };
    SDL_Rect grass2Right = { (WINDOW_WIDTH + ROAD_WIDTH) / 2, -WINDOW_HEIGHT, (WINDOW_WIDTH - ROAD_WIDTH) / 2, WINDOW_HEIGHT };

    bool quit = false;
    SDL_Event event;

    int score = 0;
    bool gameOver = false;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (!gameOver) {
            if (keystate[SDL_SCANCODE_LEFT]) {
                car.x -= CAR_SPEED;
            }
            if (keystate[SDL_SCANCODE_RIGHT]) {
                car.x += CAR_SPEED;
            }
        }

        // Keep car within bounds
        if (car.x < (WINDOW_WIDTH - ROAD_WIDTH) / 2) {
            car.x = (WINDOW_WIDTH - ROAD_WIDTH) / 2;
            gameOver = true;
            explosion.x = car.x;
            explosion.y = car.y;
        }
        if (car.x + car.w > (WINDOW_WIDTH + ROAD_WIDTH) / 2) {
            car.x = (WINDOW_WIDTH + ROAD_WIDTH) / 2 - car.w;
            gameOver = true;
            explosion.x = car.x;
            explosion.y = car.y;
        }

        // Move road and grass
        road1.y += ROAD_SPEED;
        road2.y += ROAD_SPEED;
        grass1Left.y += ROAD_SPEED;
        grass2Left.y += ROAD_SPEED;
        grass1Right.y += ROAD_SPEED;
        grass2Right.y += ROAD_SPEED;

        if (road1.y >= WINDOW_HEIGHT) road1.y = road2.y - WINDOW_HEIGHT;
        if (road2.y >= WINDOW_HEIGHT) road2.y = road1.y - WINDOW_HEIGHT;
        if (grass1Left.y >= WINDOW_HEIGHT) grass1Left.y = grass2Left.y - WINDOW_HEIGHT;
        if (grass2Left.y >= WINDOW_HEIGHT) grass2Left.y = grass1Left.y - WINDOW_HEIGHT;
        if (grass1Right.y >= WINDOW_HEIGHT) grass1Right.y = grass2Right.y - WINDOW_HEIGHT;
        if (grass2Right.y >= WINDOW_HEIGHT) grass2Right.y = grass1Right.y - WINDOW_HEIGHT;

        // Move opponent cars
        opponent1.y += ROAD_SPEED;
        opponent2.y += ROAD_SPEED;

        if (opponent1.y > WINDOW_HEIGHT) {
            opponent1.y = -200;
            opponent1.x = (WINDOW_WIDTH - ROAD_WIDTH) / 2 + rand() % (ROAD_WIDTH - 50);
            score++;
        }
        if (opponent2.y > WINDOW_HEIGHT) {
            opponent2.y = -400;
            opponent2.x = (WINDOW_WIDTH - ROAD_WIDTH) / 2 + rand() % (ROAD_WIDTH - 50);
            score++;
        }

        // Check collisions
        if (checkCollision(car, opponent1) || checkCollision(car, opponent2)) {
            gameOver = true;
            explosion.x = car.x;
            explosion.y = car.y;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, grassTexture, NULL, &grass1Left);
        SDL_RenderCopy(renderer, grassTexture, NULL, &grass2Left);
        SDL_RenderCopy(renderer, grassTexture, NULL, &grass1Right);
        SDL_RenderCopy(renderer, grassTexture, NULL, &grass2Right);
        SDL_RenderCopy(renderer, roadTexture, NULL, &road1);
        SDL_RenderCopy(renderer, roadTexture, NULL, &road2);
        SDL_RenderCopy(renderer, carTexture, NULL, &car);
        SDL_RenderCopy(renderer, carOpp1Texture, NULL, &opponent1);
        SDL_RenderCopy(renderer, carOpp2Texture, NULL, &opponent2);

        if (gameOver) {
    		SDL_RenderCopy(renderer, explosionTexture, NULL, &explosion);}
		else {
    		renderText("Score: " + intToString(score), WINDOW_WIDTH - 200, 10, renderer, font);
		}

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(roadTexture);
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(carTexture);
    SDL_DestroyTexture(carOpp1Texture);
    SDL_DestroyTexture(carOpp2Texture);
    SDL_DestroyTexture(explosionTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
