#include "SDL_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <cmath>


const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

bool init();
bool createBackground();
bool createBoard();
bool loadTextures();
void close();

SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
SDL_Texture* Texture = NULL;


bool init(){
    bool successfulness = true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        successfulness = false;
    }else{
        
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" ) )
		{
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
                SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // int imgFlags = IMG_INIT_PNG;
                // if(!(IMG_Init(imgFlags) & imgFlags)){
                //     successfulness = false;
                //}
            }
        }
    }
    return successfulness;
}
bool createBackground(){
    SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0x00);
    SDL_RenderClear(Renderer);
    return true;
}
SDL_Texture* loadTexture(std::string path){
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }else{
        newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
        if(newTexture == NULL){
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
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
bool createBoard(){
    SDL_Rect FillRect = {SCREEN_WIDTH/3, SCREEN_HEIGHT/3,SCREEN_WIDTH/3, SCREEN_HEIGHT*2/3};
    SDL_SetRenderDrawColor(Renderer, 187, 173, 160, 0);
    SDL_RenderFillRect(Renderer, &FillRect);
}
int main(){
    if(!init()){
        printf("Failed to initialize!\n");
    }else{
            bool quit = false;
            SDL_Event e;
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                SDL_RenderClear(Renderer);
                SDL_RenderCopy(Renderer, Texture, NULL, NULL);
                SDL_RenderPresent(Renderer);
            }
    }
    close();
    return 0;
}