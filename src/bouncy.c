///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// INCLUDES ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// DEFINES ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define WIDTH       900
#define HEIGHT      600
#define A_GRAVITY   0.2
#define LENGTH      50
#define BALL_COLOR  0xffffffff
#define COLOR_BLACK 0x00000000

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBALS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Uint32 g_colors[] = {
    0xFF0000,0xFF0500,0xFF0A00,0xFF0F00,0xFF1400,0xFF1900,0xFF1E00,0xFF2300,0xFF2800,0xFF2D00,
    0xFF3200,0xFF3700,0xFF3C00,0xFF4100,0xFF4600,0xFF4B00,0xFF5000,0xFF5500,0xFF5A00,0xFF5F00,
    0xFF6400,0xFF6900,0xFF6E00,0xFF7300,0xFF7800,0xFF7D00,0xFF8200,0xFF8700,0xFF8C00,0xFF9100,
    0xFF9600,0xFF9B00,0xFFA000,0xFFA500,0xFFAA00,0xFFAF00,0xFFB400,0xFFB900,0xFFBE00,0xFFC300,
    0xFFCD00,0xFFD700,0xFFDC00,0xFFE100,0xFFE600,0xFFF000,0xFFF000,0xFFFF00,0xFFFF00
};

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STRUCTS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct Circle{
    double x;
    double y;
    double radius;
    double v_x;
    double v_y;
} Circle;


void FillCircle(SDL_Surface *surface, Circle circle, Uint32 color){
    double low_x = circle.x - circle.radius;
    double low_y = circle.y - circle.radius;
    double high_x = circle.x + circle.radius;
    double high_y = circle.y + circle.radius;

    double radius_squared = circle.radius * circle.radius;

    for(double x=low_x; x < high_x; x++){
        for(double y=low_y; y < high_y; y++){
            //Is coordinate within circle?
            double center_distance = (x-circle.x)*(x-circle.x) + (y-circle.y)*(y-circle.y);
            if(center_distance < radius_squared){
                SDL_Rect pixel = (SDL_Rect) {x,y,1,1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}


void FillTrajectory(SDL_Surface *surface, Circle trajectory[LENGTH], int current_trajectory_index){
    for(int i=0; i<current_trajectory_index; i++){
        trajectory[i].radius = i;
        FillCircle(surface, trajectory[i], g_colors[i]);
    }
}


void UpdateTrajectory(Circle trajectory[LENGTH], struct Circle circle, int current_index){
    if(current_index >= LENGTH){
        //shift array - write the circle at the end of the array
        Circle trajectory_copy[LENGTH];
        for(int i=0; i<LENGTH; i++){
            if(i > 0 && i < LENGTH)
                trajectory_copy[i] = trajectory[i+1];
        }
        for(int i=0; i<LENGTH; i++){
            trajectory[i] = trajectory_copy[i];
        }
        trajectory[current_index] = circle;
    }
    else{
        trajectory[current_index] = circle;
    }
}


void Step(Circle *circle){
    //How do we calculate the new position?
    circle->x += circle->v_x;
    circle->y += circle->v_y;
    circle->v_y += (+A_GRAVITY);

    //Did the ball exit the screen?
    //Y axis:
    if( (circle->y + circle->radius) > HEIGHT ){
        circle->y = HEIGHT - circle->radius;
        circle->v_y = -(circle->v_y);
    }
    if( (circle->y - circle->radius) < 0){
        circle->y = circle->radius;
        circle->v_y = -(circle->v_y);
    }
    //X axis:
    if( (circle->x + circle->radius) > WIDTH ){
        circle->x = WIDTH - circle->radius;
        circle->v_x = -(circle->v_x);
    }
    if( (circle->x - circle->radius) < 0){
        circle->x = circle->radius;
        circle->v_x = -(circle->v_x);
    }
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////// MAIN ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(){
    srand(time(0));
    printf("Hello Bouncy Ball.\n");

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    if(surface == NULL){
        printf("ERROR");
    }

    int vel_x = (rand() % 30) - 15;
    int vel_y = (rand() % 30) - 15;

    Circle circle = {200, 200, 50, vel_x, vel_y};
    Circle trajectory[LENGTH];
    int trajectory_entry_count = 0;

    SDL_Rect erase_rect = (SDL_Rect){0,0,WIDTH,HEIGHT};
    SDL_Event event;
    int simulation_running = 1;

    while(simulation_running != 0){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                simulation_running = 0;
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_SPACE){
                    simulation_running = 0;
                }
            }
        }

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        FillTrajectory(surface, trajectory, trajectory_entry_count);
        FillCircle(surface, circle, BALL_COLOR);

        Step(&circle);
        UpdateTrajectory(trajectory, circle, trajectory_entry_count);

        if(trajectory_entry_count < LENGTH) ++trajectory_entry_count;
        SDL_UpdateWindowSurface(window);
        SDL_Delay(5);
    }
}
