#include "SDL_pixels.h"
#include "SDL_render.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <array>
#include <stdio.h>
#include <cmath>
#include <random>
#include <string>

const int GRID_SIZE = 4;
const int TILE_MARGIN = 10;
const int FONT_SIZE = 250;
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;
int TILE_SIZE = SCREEN_WIDTH/3/GRID_SIZE;
int SQUARE_SIZE = SCREEN_WIDTH/3;
int MARGIN_X = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
int MARGIN_Y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;

enum KeyPressSurfaces{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

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

typedef std::array<std::array<int,GRID_SIZE>,GRID_SIZE> boardType;


SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
SDL_Texture* Texture = NULL;
TTF_Font* Font = NULL;

bool init(){
    //Initializes SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        return false;
    }
    //Sets the render scale quality
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" ) ){
        printf( "Render scale is not set properly" );
    }
    //Creates the window
    Window = SDL_CreateWindow("2048 game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(Window == NULL){
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    //Creates the renderer
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    if(Renderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    //Initializes SDL_ttf
    if(TTF_Init() == -1){
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }
    //Loads the font from clearSans.ttf
    Font = TTF_OpenFont( "clearSans.ttf", FONT_SIZE );
    if( Font == NULL ){
        printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
        return false;
    }
    return true;
}

bool drawBackground(){
    SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0x00);//Sets the background color

    SDL_RenderClear(Renderer);//Fills the screen with background color

    SDL_SetRenderDrawColor(Renderer, 187, 173, 160, 0xFF);//Sets the color of the play area

    SDL_Rect fillRect = { MARGIN_X, MARGIN_Y, SQUARE_SIZE, SQUARE_SIZE };//Size for the play area

    SDL_RenderFillRect(Renderer, &fillRect);//Draws the play area

    return true;
}


std::random_device rd;
std::mt19937 gen(rd());

int getRandomNumber(int min, int max) {
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
//TODO: 
void gameover(){
    printf("Game over\n");
}
//Adds a random tile to the board when there are empty tiles left
bool addRandomTile(boardType& board){//Passing by reference to not create a copy
    bool emptySpaces=false;

    //Checks if there are any empty spaces left
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){
            if(board[x][y] == 0){
                emptySpaces=true;
            }
        }
    //If there are no empty spaces left, returns false
    }if(!emptySpaces){
        return false;
    }

    //Generates a random position for the new tile
    int x = getRandomNumber(0,3);
    int y = getRandomNumber(0, 3);

    //If the tile is empty, add a '2' tile there
    if(board[x][y] == 0){
        board[x][y] = 2;
        return true;
    }else{
        //If the tile is not empty, try again
        return addRandomTile(board);
    }
}
bool drawTile(boardType& board){//Passing by reference to not create a copy
    //Going over all the tiles in the board
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){

            int value = board[x][y];
            if(value == 0){
                continue;
            }
            //Returns the index for the color using the value(As they are powers of 2)
            int index=log2(value);

            //Draws a tile with the color corresponding to the value
            SDL_SetRenderDrawColor(Renderer, TILE_COLORS[index].r, TILE_COLORS[index].g, TILE_COLORS[index].b, TILE_COLORS[index].a);
            SDL_Rect tileRect = {MARGIN_X+TILE_MARGIN+x * TILE_SIZE,MARGIN_Y+TILE_MARGIN+ y * TILE_SIZE, TILE_SIZE-TILE_MARGIN*2, TILE_SIZE-TILE_MARGIN*2};
            SDL_RenderFillRect(Renderer, &tileRect);

            //to_string(value).c_str() because TTF_RenderText_Solid takes a char*
            SDL_Color currentColor =  {static_cast<Uint8>(256 - TILE_COLORS[index].r), static_cast<Uint8>(256 - TILE_COLORS[index].g), static_cast<Uint8>(256 - TILE_COLORS[index].b), 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid( Font,std::to_string(value).c_str() , currentColor);

            //If unable to create the text surface, return false
            if(textSurface == NULL ){
                printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
                return false;
            }

            Texture = SDL_CreateTextureFromSurface(Renderer, textSurface);


            //If unable to create the texture, return false
            if( Texture == NULL ){
                printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
                return false;
            }

            int numberSize = std::to_string(value).size();
            SDL_Rect textRect={0,0,0,0};

            if(numberSize>2){
                textRect = {MARGIN_X+TILE_MARGIN*2+x * TILE_SIZE,MARGIN_Y+TILE_MARGIN+ y * TILE_SIZE, TILE_SIZE/4*numberSize - TILE_MARGIN*(4-numberSize), TILE_SIZE-TILE_MARGIN*2};
            }else{
                textRect = {MARGIN_X+TILE_MARGIN*4+x * TILE_SIZE,MARGIN_Y+TILE_MARGIN*2+ y * TILE_SIZE, TILE_SIZE - TILE_MARGIN*8, TILE_SIZE-TILE_MARGIN*4};
            }

            SDL_RenderCopy(Renderer, Texture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(Texture);
        }
    }
    return true;      
}

//Displays the board into the console for debugging
void logBoard(boardType& board){
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){
            printf("%d ", board[x][y]);
        }
        printf("\n");
    }
}

//Initializes the board with 2 random tiles and returns it
boardType initBoard(){
    boardType board={0};
    addRandomTile(board);
    addRandomTile(board);
    return board;
}

//Closes the window and hopefully frees all the memory
void close(){
    SDL_DestroyTexture(Texture);
    Texture = NULL;
    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;
    SDL_DestroyWindow(Window);
    Window = NULL;
    SDL_Quit();
    TTF_Quit();
    Font=NULL;
}

//TODO make the moves actually boolean
bool moveUp(boardType& board){
    for(int x=0;x<GRID_SIZE;x++){
        for(int y=0;y<GRID_SIZE;y++){
            if(board[x][y] !=0){
                while(y>0 && board[x][y-1] == 0){//It will not go out of range because y>0
                    board[x][y-1] = board[x][y];
                    board[x][y] = 0;
                    y--;
                }
                if(y>0 && board[x][y-1] == board[x][y]){//If the tile above is the same, merge them
                    board[x][y-1] *= 2;
                    board[x][y] = 0;
                }
            }
        }
    }
    addRandomTile(board);
    return true;
}
bool moveDown(boardType& board){
    for(int x=0;x<GRID_SIZE;x++){
        for(int y=GRID_SIZE-1;y>=0;y--){
            if(board[x][y] !=0){
                while(y<GRID_SIZE-1 && board[x][y+1] == 0){//It will not go out of range because y<3(GRID_SIZE-1)
                    board[x][y+1] = board[x][y];
                    board[x][y] = 0;
                    y++;
                }
                if(y<GRID_SIZE-1 && board[x][y+1] == board[x][y]){//If the tile below is the same, merge them
                    board[x][y+1] *= 2;
                    board[x][y] = 0;
                }
            }
        }
    }
    addRandomTile(board);
    return true;
}
bool moveLeft(boardType& board){
    for(int y=0;y<GRID_SIZE;y++){
        for(int x=0;x<GRID_SIZE;x++){
            if(board[x][y] !=0){
                while(x>0 && board[x-1][y] == 0){//It will not go out of range because x>0
                    board[x-1][y] = board[x][y];
                    board[x][y] = 0;
                    x--;
                }
                if(x>0 && board[x-1][y] == board[x][y]){//If the tile to the left is the same, merge them
                    board[x-1][y] *= 2;
                    board[x][y] = 0;
                }
            }
        }
    }
    addRandomTile(board);
    return true;
}
bool moveRight(boardType& board){
    for(int y=0;y<GRID_SIZE;y++){
        for(int x=GRID_SIZE-1;x>=0;x--){
            if(board[x][y] !=0){
                while(x<GRID_SIZE-1 && board[x+1][y] == 0){//It will not go out of range because x<3(GRID_SIZE-1)
                    board[x+1][y] = board[x][y];
                    board[x][y] = 0;
                    x++;
                }
                if(x<GRID_SIZE-1 && board[x+1][y] == board[x][y]){//If the tile to the right is the same, merge them
                    board[x+1][y] *= 2;
                    board[x][y] = 0;
                }
            }
        }
    }
    addRandomTile(board);
    return true;
}

int main(){
    if(!init()){
        printf("Failed to initialize!\n");
        close();
        return 0;
    }

    bool quit = false;
    SDL_Event e;
    boardType board = initBoard();//Initializes the board of size GRID_SIZE*GRID_SIZE

    while(!quit){//Main loop

        while(SDL_PollEvent(&e) != 0){//Event handler
            if(e.type == SDL_QUIT){
                quit = true;
            } else if( e.type == SDL_KEYDOWN ){//User presses a key
                switch( e.key.keysym.sym ){//TODO make it so that if 4 moves were made and the board is the same, game is over
                    case SDLK_UP:
                    case SDLK_w:
                        moveUp(board);
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        moveDown(board);
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        moveLeft(board);
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        moveRight(board);
                        break;
                    case SDLK_r:
                        board = initBoard();
                        break;
                    case SDLK_q:
                        quit = true;
                        break;
                    default:
                        break;
                }
            }else if( e.type == SDL_WINDOWEVENT ){
                switch( e.window.event ){
                    //Get new dimensions
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SCREEN_WIDTH = e.window.data1;
                        SCREEN_HEIGHT = e.window.data2;
                        TILE_SIZE = SCREEN_WIDTH/3/GRID_SIZE;
                        SQUARE_SIZE = SCREEN_WIDTH/3;
                        MARGIN_X = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
                        MARGIN_Y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;
                        break;
                    default:
                        break;
                }
            }
        }
        if(!drawBackground()){printf("Failed to draw the background");}//Draws the background each loop
        if(!drawTile(board)){printf("Failed to draw the tiles");}//Draws the tiles each loop
        SDL_RenderPresent(Renderer);//Updates the screen
    }
    //Closes the window and frees the memory when user closes the game
    close();
    return 0;
}
