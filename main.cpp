#include <SDL.h>//Main SDL library
#include <SDL_ttf.h>//SDL_ttf for text rendering
#include <array>//For 2D arrays
#include <stdio.h>//For printf
#include <cmath>//For log2
#include <random>//For random number generation
#include <string>//For std::to_string
#include <fstream>//For score saving

const int GRID_SIZE = 4;//CAN BE CHANGED TO ANY (practical) SIZE. Default is 4

const int TILE_MARGIN = 10;
const int FONT_SIZE = 250;

//These numbers will change when the window is resized
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;
int TILE_SIZE = SCREEN_WIDTH/3/GRID_SIZE;
int SQUARE_SIZE = SCREEN_WIDTH/3;
int MARGIN_X = (SCREEN_WIDTH - SQUARE_SIZE) / 2;
int MARGIN_Y = (SCREEN_HEIGHT - SQUARE_SIZE) / 2;
unsigned int HIGHEST_SCORE = 0;
unsigned int CURRENT_SCORE = 0;


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

typedef std::array<std::array<unsigned int,GRID_SIZE>,GRID_SIZE> boardType;//2D array for the board

//SDL variables pre-declared and initialized to NULL
SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;
SDL_Texture* Texture = NULL;
TTF_Font* Font = NULL;
SDL_Texture* scoreTexture = NULL;
SDL_Texture* bestScoreTexture = NULL;


//Loads the highest score from the file "score.txt"
unsigned int loadHighestScore(){

    unsigned int highestScore=0;//Preinitialized to 0 just in case
    std::ifstream file;
    file.open("score.txt");

    //If something is wrong with the file, highest score is 0, otherwise it is the value in the file
    if(file.is_open()){
        if(!(file >> highestScore)){
            highestScore=0;
        }
    }

    file.close();
    return highestScore;
}

//Saves the highest score to the file "score.txt"
void saveHighestScore(unsigned int score){
    std::ofstream file;
    file.open("score.txt");
    if(file.is_open()){
        file << score;
    }
    file.close();
}

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
    Window = SDL_CreateWindow("2048 game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    HIGHEST_SCORE = loadHighestScore();//Loads the highest score from the file "score.txt"
    CURRENT_SCORE=0;//Initializes the current score to 0
    return true;
}


//Draws the background of the game
bool drawBackground(){
    SDL_SetRenderDrawColor(Renderer, 250, 248, 239, 0x00);//Sets the background color

    SDL_RenderClear(Renderer);//Fills the screen with background color

    SDL_SetRenderDrawColor(Renderer, 187, 173, 160, 0xFF);//Sets the color of the play area

    SDL_Rect fillRect = { MARGIN_X, MARGIN_Y, SQUARE_SIZE, SQUARE_SIZE };//Size for the play area

    SDL_RenderFillRect(Renderer, &fillRect);//Draws the play area

    return true;
}
//Draws the score above the play area
bool drawScore(){
    
    //Boxes for the score and best score
    SDL_SetRenderDrawColor(Renderer, 225, 208, 192, 255);

    int scoreRectSize = SQUARE_SIZE/2-TILE_MARGIN*2;
    SDL_Rect scoreRect = {MARGIN_X+TILE_MARGIN+ SQUARE_SIZE/2, MARGIN_Y/3, scoreRectSize, MARGIN_Y/2};
    SDL_RenderFillRect(Renderer, &scoreRect);

    SDL_Rect bestScoreRect = {MARGIN_X+TILE_MARGIN, MARGIN_Y/3,scoreRectSize , MARGIN_Y/2};
    SDL_RenderFillRect(Renderer, &bestScoreRect);

    //Text for the score and best score
    SDL_Color textColor = {0, 0, 0, 255};
    std::string scoreText = "Score: " + std::to_string(CURRENT_SCORE);
    std::string bestScoreText = "Best: " + std::to_string(HIGHEST_SCORE);

    SDL_Surface* bestScoreSurface = TTF_RenderText_Solid( Font, bestScoreText.c_str(), textColor);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid( Font, scoreText.c_str(), textColor);

    if(scoreSurface == NULL || bestScoreSurface == NULL){
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        return false;
    }

    bestScoreTexture = SDL_CreateTextureFromSurface(Renderer, bestScoreSurface);
    scoreTexture = SDL_CreateTextureFromSurface(Renderer, scoreSurface);

    if( scoreTexture == NULL || bestScoreTexture == NULL){
        printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    //Text boxes for the score and best score
    int scoreTextRectMargin = MARGIN_X+TILE_MARGIN;
    int scoreTextRectSize = SQUARE_SIZE/2-TILE_MARGIN*4;
    SDL_Rect bestScoreTextRect = {scoreTextRectMargin + TILE_MARGIN, MARGIN_Y/3, scoreTextRectSize, MARGIN_Y/2};
    SDL_Rect scoreTextRect = { scoreTextRectMargin + SQUARE_SIZE/2+ TILE_MARGIN, MARGIN_Y/3,scoreTextRectSize , MARGIN_Y/2};

    //Rendering the text
    SDL_RenderCopy(Renderer, bestScoreTexture, NULL, &bestScoreTextRect);
    SDL_RenderCopy(Renderer, scoreTexture, NULL, &scoreTextRect);

    SDL_FreeSurface(bestScoreSurface);
    SDL_FreeSurface(scoreSurface);

    SDL_DestroyTexture(bestScoreTexture);
    SDL_DestroyTexture(scoreTexture);

    return true;
}

std::random_device rd;
std::mt19937 gen(rd());

int getRandomNumber(int min, int max) {
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
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
    int x = getRandomNumber(0, GRID_SIZE-1);
    int y = getRandomNumber(0, GRID_SIZE-1);

    //If the tile is empty, add a '2' tile there
    if(board[x][y] == 0){
        board[x][y] = 2;
        return true;
    }else{
        //If the tile is not empty, try again
        return addRandomTile(board);
    }
}

//Draws the tiles on the board
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

            if(numberSize==1){
                textRect = {MARGIN_X + TILE_MARGIN*3 + x * TILE_SIZE,MARGIN_Y+TILE_MARGIN*2+ y * TILE_SIZE, TILE_SIZE-TILE_MARGIN*6, TILE_SIZE-TILE_MARGIN*4};
            }else{
                textRect = {MARGIN_X + TILE_MARGIN + x * TILE_SIZE,MARGIN_Y+TILE_MARGIN*2+ y * TILE_SIZE, TILE_SIZE-TILE_MARGIN*2, TILE_SIZE-TILE_MARGIN*4};
            }
            //  textRect = {MARGIN_X+TILE_MARGIN*2*(4-numberSize)+x * TILE_SIZE,MARGIN_Y+TILE_MARGIN*2+ y * TILE_SIZE, TILE_SIZE/4*(4-numberSize)-TILE_MARGIN*2, TILE_SIZE-TILE_MARGIN*4};

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
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(bestScoreTexture);
    scoreTexture = NULL;
    bestScoreTexture = NULL;
    Texture = NULL;
    SDL_DestroyRenderer(Renderer);
    Renderer = NULL;
    SDL_DestroyWindow(Window);
    Window = NULL;
    TTF_Quit();
    Font=NULL;
    SDL_Quit();
}

bool moveTiles(boardType& board, int dx, int dy) {
    bool moved = false;

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            
            //Current tile
            int x = dx == 1 ? GRID_SIZE - 1 - j : j;//If dx is 1, x is GRID_SIZE-1-j, otherwise x is j. So it will start from the opposite side if needed
            int y = dy == 1 ? GRID_SIZE - 1 - i : i;//Same here

            int nextX = x + dx;//If dx is 1, nextX is x+1, if dx is -1, nextX is x-1
            int nextY = y + dy;//Same here

            while (nextX >= 0 && nextX < GRID_SIZE && nextY >= 0 && nextY < GRID_SIZE) {//While the next tile is within the board

                //If the next tile is empty and the current one is not
                if (board[nextX][nextY] == 0 && board[x][y] != 0) {

                    board[nextX][nextY] = board[x][y];// Move the tile to the empty spot
                    board[x][y] = 0;

                    //Go to the next tile
                    x = nextX;
                    y = nextY;

                    //Update the next tile
                    nextX += dx;
                    nextY += dy;

                    moved = true;

                //If the next tile is the same and not empty
                } else if (board[nextX][nextY] == board[x][y] && board[x][y] != 0) {

                    // Merge the tiles
                    board[nextX][nextY] *= 2;
                    board[x][y] = 0;

                    //Update the highest and current score
                    if(board[nextX][nextY]>CURRENT_SCORE){
                        CURRENT_SCORE=board[nextX][nextY];
                        if(CURRENT_SCORE>HIGHEST_SCORE){
                            HIGHEST_SCORE=CURRENT_SCORE;
                            saveHighestScore(HIGHEST_SCORE);
                        }
                    }
                    
                    moved = true;
                //If nothing can be done, break the loop
                } else {
                    break;
                }
            }
        }
    }
    if(moved){addRandomTile(board);}
    return moved;
}

int main(){
    if(!init()){
        printf("Failed to initialize!\n");
        close();
        return 0;
    }

    bool quit = false;
    SDL_Event event;
    boardType board = initBoard();//Initializes the board of size GRID_SIZE*GRID_SIZE
    
    std::array<bool,4> state = {1,1,1,1};//State of the moves, if all are 0, game is over

    while(!quit){//Main loop

        while(SDL_PollEvent(&event) != 0){//Event handler
            if(event.type == SDL_QUIT){
                quit = true;
            } else if( event.type == SDL_KEYDOWN ){//User presses a key
                switch( event.key.keysym.sym ){
                    case SDLK_UP:
                    case SDLK_w:
                        if(moveTiles(board,0,-1)){//Up
                            state.fill(1);
                        }else{
                            state[0]=0;
                        }
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        if(moveTiles(board,0,1)){//Down
                            state.fill(1);
                        }else{
                            state[1]=0;
                        }
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        if(moveTiles(board,-1,0)){//Left
                            state.fill(1);
                        }else{
                            state[2]=0;
                        }
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        if(moveTiles(board,1,0)){//Right
                            state.fill(1);
                        }else{
                            state[3]=0;
                        }
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
            }else if( event.type == SDL_WINDOWEVENT ){
                switch( event.window.event ){
                    //Get new dimensions
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SCREEN_WIDTH = event.window.data1;
                        SCREEN_HEIGHT = event.window.data2;
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
        if(state[0]==0 && state[1]==0 && state[2]==0 && state[3]==0){//If all moves are invalid, game is over
            board=initBoard();
            state.fill(1);
        }
        if(!drawBackground()){printf("Failed to draw the background");}//Draws the background each loop
        if(!drawScore()){printf("Failed to draw the score");}//Draws the score each loop
        if(!drawTile(board)){printf("Failed to draw the tiles");}//Draws the tiles each loop
        SDL_RenderPresent(Renderer);//Updates the screen
    }
    //Closes the window and frees the memory when user closes the game
    close();
    return 0;
}
