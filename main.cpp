#include "SDL_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <cmath>
#include <random>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int GRID_SIZE = 4;
const int TILE_SIZE = SCREEN_WIDTH/3/4;
const SDL_Color TILE_COLORS[] = {
    {205, 193, 180, 255}, // 0
    {238, 228, 218, 255}, // 2
    {237, 224, 200, 255}, // 4
    {242, 177, 121, 255}, // 8
    {245, 149, 99, 255},  // 16
    {246, 124, 95, 255},  // 32
    {246, 94, 59, 255},   // 64
    {237, 207, 114, 255}, // 128
    {237, 204, 97, 255},  // 256
    {237, 200, 80, 255},  // 512
    {237, 197, 63, 255},  // 1024
    {237, 194, 46, 255}   // 2048
};

bool init();
bool createBackground();
bool loadTextures();
void close();
void fillCenterSquare(); 

SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
SDL_Texture* Texture = NULL;

bool init(){
    bool successfulness = true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        successfulness = false;
    }else{
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" ) ){
            printf( "Render scale is not set properly" );
        }

        Window = SDL_CreateWindow("2048 game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(Window == NULL){
            successfulness = false;
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        }else{
            Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
            if(Renderer == NULL){
                successfulness = false;
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
            }else{
                SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0xFF);
            }
        }
    }
    return successfulness;
}

bool createBackground(){
    SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0x00);
    SDL_RenderClear(Renderer);

    int SQUARE_SIZE = SCREEN_WIDTH/3;
    int x = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
    int y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;

    SDL_SetRenderDrawColor(Renderer, 187, 173, 160, 0xFF);

    SDL_Rect fillRect = { x, y, SQUARE_SIZE, SQUARE_SIZE };

    SDL_RenderFillRect(Renderer, &fillRect);

    SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0xFF);

    return true;
}

// SDL_Texture* loadTexture(std::string path){
//     SDL_Texture* newTexture = NULL;
//     SDL_Surface* loadedSurface = IMG_Load(path.c_str());
//     if(loadedSurface == NULL){
//         printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
//     }else{
//         newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
//         if(newTexture == NULL){
//             printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//         }
//         SDL_FreeSurface(loadedSurface);
//     }
//     return newTexture;
// }

// //Function to create the center square for the game
// void fillCenterSquare(){
    
// }
std::random_device rd;
std::mt19937 gen(rd());

int getRandomNumber(int min, int max) {
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
bool addRandomTile(int board[GRID_SIZE][GRID_SIZE]){

    int x = getRandomNumber(0,3);
    int y = getRandomNumber(0, 3);
    //DEBUG!
    printf("x: %d, y: %d\n", x, y);

    if(board[x][y] == 0){
        board[x][y] = 2;
        return true;
    }else{
        return addRandomTile(board);
    }
}
bool drawTile(int board[GRID_SIZE][GRID_SIZE]){

    for(int y = 0; y < GRID_SIZE; ++y){
        for(int x = 0; x < GRID_SIZE; ++x){
            int value = board[x][y];
            if(value > 0){
                //Returns the index for the color using the value(As they are powers of 2)
                int index=log2(value);
                SDL_SetRenderDrawColor(Renderer, TILE_COLORS[index].r, TILE_COLORS[index].g, TILE_COLORS[index].b, TILE_COLORS[index].a);
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderFillRect(Renderer, &tileRect);
            }else{continue;}
        }
    }
    return true;      
}
void initBoard(){
    int board[GRID_SIZE][GRID_SIZE] = {0};
    addRandomTile(board);
    addRandomTile(board);

    //DEBUG!
    for(int y = 0; y < GRID_SIZE; ++y){
        for(int x = 0; x < GRID_SIZE; ++x){
            printf("%d ", board[x][y]);
        }
        printf("\n");
    }
    drawTile(board);
    // return board;
}



void close(){
    SDL_DestroyTexture(Texture);
    Texture = NULL;
    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;
    SDL_DestroyWindow(Window);
    Window = NULL;
    SDL_Quit();
}

int main(){
    if(!init()){
        printf("Failed to initialize!\n");
    }else{
        bool quit = false;
        SDL_Event e;
        initBoard();
        while(!quit){//Main loop
            SDL_RenderClear(Renderer);
            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    quit = true;
                }
            }
            createBackground();
            SDL_RenderCopy(Renderer, Texture, NULL, NULL);
            SDL_RenderPresent(Renderer);
        }
    }
    close();
    return 0;
}
