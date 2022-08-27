// Using SDL, tinyxml2
#include <iostream>
#include <math.h>
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_ttf.h"
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

#define WIDTH 620
#define HEIGHT 720
#define PI 3.14
#define SPEED 9
#define SIZE 16
#define BALL_SPEED 8
#define FONT_SIZE 32

#define COL 20
#define ROW 3
#define SPACING 12

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Color color;
SDL_Rect paddle, ball, lives, brick;
TTF_Font * font;
XMLDocument doc;

bool running;
int fps, timerFPS, lastFrame, frameCount;
float moveY, moveX;
int lifeCount;
int bricks[COL * ROW];
int currentLevel;

void resetBricks(int lives) {

    XMLError err = doc.LoadFile("brickdata.xml");
    if (err != XML_SUCCESS) cout<<"Error, can't read xml file"<<endl;
    XMLElement * pBricks = doc.RootElement()->FirstChildElement("level1");
    string sBricks = pBricks->GetText();

    if (lives > 0) lifeCount--;

    else if (lives <= 0 && NULL != pBricks){
        for (int i = 0; i<COL * ROW; i++) bricks[i] = 1; //(int)sBricks[i];
        lifeCount = 3;
    }
 
    paddle.x = (WIDTH / 2) - (paddle.w / 2);
    ball.y = paddle.y - (paddle.h * 4);
    ball.x = WIDTH / 2 - (SIZE / 2);
    moveY = BALL_SPEED / 2;
    moveX = 0;
}

void setBricks(int i) {
    brick.x = (((i % COL) + 1) * SPACING) + ((i % COL) * brick.w) - (SPACING / 2);
    brick.y = brick.h * 3 + (((i % ROW) + 1) * SPACING) + ((i % ROW) * brick.h) - (SPACING / 2);
}

void write(string text, int x, int y) {
    SDL_Surface * surface;
    SDL_Texture * texture;
    const char * t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    lives.w = surface->w;
    lives.h = surface->h;
    lives.x = x - lives.w;
    lives.y = y - lives.h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &lives);
    SDL_DestroyTexture(texture);
}

void update() {
    if (lifeCount <= 0) resetBricks(lifeCount);
    
    if (SDL_HasIntersection(&ball, &paddle)) {
        double rel = (paddle.x + (paddle.w / 2)) - (ball.x + (SIZE / 2));
        double norm = rel / (paddle.w / 2);
        double bounce = norm * (5 * PI / 12);

        moveY = -BALL_SPEED * cos(bounce);
        moveX = BALL_SPEED * -sin(bounce);
    }

    if (ball.y <= 0) moveY = -moveY;

    if (ball.y + SIZE >= HEIGHT) resetBricks(lifeCount);

    if (ball.x <= 0 || ball.x + SIZE >= WIDTH) moveX = -moveX;

    ball.x += moveX;
    ball.y += moveY;

    if (paddle.x < 0) paddle.x = 0;
    if (paddle.x + paddle.w > WIDTH) paddle.x = WIDTH - paddle.w;

    bool reset = 1;

    /*cycle for bricks to fill
    XMLError err = doc.LoadFile("brickdata.xml");
    if (err != XML_SUCCESS) cout<<"Error, can't read xml file"<<endl;
    XMLElement * pBricks = doc.RootElement()->FirstChildElement("level1");
    string sBricks = pBricks->GetText();
    if (NULL != pBricks) for (int i = 0; i<COL * ROW; i++) bricks[i] = (int)sBricks[i];
    */  

    for (int i = 0; i < COL * ROW; i++) {
        setBricks(i);

        if (SDL_HasIntersection(&ball, &brick) && bricks[i]) {
            
            /*if (bricks[i]!=0) {
                moveX = -moveX;
                moveY = -moveY;
                ball.y -= 18;
                ball.x -= 18;
                bricks[i]--;
            }*/

            bricks[i] = 0;

            if (ball.x >= brick.x) {
                moveX = -moveX;
                ball.x -= 18;
            }

            if (ball.x <= brick.x) {
                moveX = -moveX;
                ball.x += 18;
            }

            if (ball.y >= brick.y) {
                moveY = -moveY;
                ball.y += 18;
            }

            if (ball.y <= brick.y) {
                moveY = -moveY;
                ball.y -= 18;
            }
        }
        if (bricks[i]) reset = 0; 
    }

    //level change goes here
    if (reset) resetBricks(0); 
    //else changeLevel()
    //add level as pattern and part of lv1.xml
    //change level just pulls the next child in Bricks
}

void input() {
    SDL_Event e;
    const Uint8 * keystates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;

    if (keystates[SDL_SCANCODE_ESCAPE]) running = false;
    if (keystates[SDL_SCANCODE_LEFT]) paddle.x -= SPEED;
    if (keystates[SDL_SCANCODE_RIGHT]) paddle.x += SPEED;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);
    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;

    if (timerFPS < (1000 / 60)) SDL_Delay((1000 / 60) - timerFPS);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);

    write(to_string(lifeCount), WIDTH / 2 + FONT_SIZE / 2, FONT_SIZE * 1.5);

    //load file
    XMLError err = doc.LoadFile("brickdata.xml");

    //check if it exists
    if (err != XML_SUCCESS) cout<<"Error, can't read xml file"<<endl;
    
    XMLElement * pRootElem = doc.RootElement();

    if (NULL != pRootElem) {
        //get brickstyles list
        XMLElement * pStyles = pRootElem->FirstChildElement("styles"); 

        //get brick layout
        //hardcoded level change?
        //this should change each time a new level is introduced
        XMLElement * pBricks = pRootElem->FirstChildElement("level1"); 

        if (NULL != pBricks)
        {
            XMLElement * pBrickColor;

            //split bricks xml into array for each brick individually
            string sBricks = pBricks->GetText();
            char cBricks[sBricks.length()+1];
            strcpy(cBricks, sBricks.c_str());

            //for each brick in bricks in xml, pull the brick,
            //pull its color/hp values and set it in place
            for (int i = 0; i < COL * ROW; i++)
            {
                char check = cBricks[i];

                //depending on brick id, pull color values
                //first check if emptyspace, if emptyspace just skip that iteration
                //if emptyspace, skip entire while and do it again
                //hardcoded, no overload for string
                switch(check){
                    case '1':
                        pBrickColor = pStyles->FirstChildElement("brickstyle1")->FirstChildElement("color");
                        break;
                    case '2':
                        pBrickColor = pStyles->FirstChildElement("brickstyle2")->FirstChildElement("color");
                        break;
                    case '3':
                        pBrickColor = pStyles->FirstChildElement("brickstyle3")->FirstChildElement("color");
                        break;
                    case '4':
                        pBrickColor = pStyles->FirstChildElement("brickstyle4")->FirstChildElement("color");
                        break;
                    case '5':
                        pBrickColor = pStyles->FirstChildElement("brickstyle5")->FirstChildElement("color");
                        break;
                }

                //get each value for the color
                XMLElement * r = pBrickColor->FirstChildElement("r");
                int rr = stoi(r->GetText());
                XMLElement * g = pBrickColor->FirstChildElement("g");
                int gg = stoi(g->GetText());
                XMLElement * b = pBrickColor->FirstChildElement("b");
                int bb = stoi(b->GetText());
                XMLElement * a = pBrickColor->FirstChildElement("a");
                int aa = stoi(a->GetText());

                //color the block
                SDL_SetRenderDrawColor(renderer, rr, gg, bb, aa);

                if (bricks[i]) {
                    setBricks(i);
                    SDL_RenderFillRect(renderer, &brick);
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int main() {
    //err
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) cout << "Failed at SDL_Init()" << endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) cout << "Failed at SDL_CreateWindowAndRenderer" << endl;

    SDL_SetWindowTitle(window, "Breakout");

    TTF_Init();
    font = TTF_OpenFont("font.ttf", FONT_SIZE);

    running = 1;
    static int lastTime = 0;

    color.r = color.g = color.b = 255;
    paddle.h = 11;
    paddle.y = HEIGHT - paddle.h - 32;
    paddle.w = WIDTH / 4;
    ball.w = ball.h = SIZE;
    brick.w = (WIDTH - (SPACING * COL)) / COL;
    brick.h = 22;

    resetBricks(lifeCount);

    while (running) {
        lastFrame = SDL_GetTicks();

        if (lastFrame >= lastTime + 1000) {
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }

        update();
        input();
        render();
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();

    return 0;
}