#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std::chrono;

//consts
const int WINDOW_SIZE = 800; //size of window in pixel in x or y
const int SCREEN_SIZE = 32; //logical window size (amount of pixels, that are rendered in the window)  in x or y
const int START_PADDLE_SIZE = SCREEN_SIZE/3;
const int DIRECTION_RIGHT = 1;
const int DIRECTION_LEFT = -1;

static SDL_Renderer* rend;

struct Paddle{
	int pos_x = 0;
	int pos_y = SCREEN_SIZE-1;
	int size = START_PADDLE_SIZE;
	struct{
		int r = 0xff;
		int g = 0xff;
		int b = 0xff;
		int a = 0xff;
	}color;
	void draw(){
		SDL_SetRenderDrawColor(rend, this->color.r, this->color.g, this->color.b, this->color.a);
		SDL_RenderDrawLine(rend,this->pos_x,this->pos_y,this->pos_x+this->size,this->pos_y);
	}
	inline bool at_wall(){
		return this->pos_x <= 0 || this->pos_x+this->size >= SCREEN_SIZE-1;
	}
};


struct Game{
/*
just game logic
+ draw functions
 */
	bool loop = true;//mainloop condition
	Paddle paddles[SCREEN_SIZE];
	Paddle* top_paddle = &paddles[0];
	int level = 0;
	int direction = DIRECTION_LEFT;//changes to DIRECTION right in update at start
	//vars for next level
	int left_end = 0;
	int right_end = SCREEN_SIZE-1;

	inline int get_speed(){//speed in update per second
		return (level+1)*5;
	}

	void step_up(){
		//resize paddle
		this->left_end = std::max(this->top_paddle->pos_x,this->left_end);
		this->right_end = std::min(this->top_paddle->pos_x+this->top_paddle->size,this->right_end);
		if(right_end <= this->left_end){
			std::cout << "game over\nyour score is: " << this->level << "\n";
			this->loop = false;
		}

		this->top_paddle->pos_x = this->left_end;
		this->top_paddle->size = this->right_end-this->left_end;	
		
		this->level++;
		this->top_paddle = &this->paddles[this->level];	
		*this->top_paddle = this->paddles[this->level-1];
		this->top_paddle->pos_y--;

	}

	void draw(){
		for(int i=0;i<=level;i++){
			this->paddles[i].draw();
		}
	}

	void update(){
		if(this->top_paddle->at_wall()){
			this->direction *=-1;
		}
		this->top_paddle->pos_x += this->direction;
	}
};


void init(){
	//init
	SDL_Init(SDL_INIT_EVERYTHING);
	//create window
	SDL_Window* win = SDL_CreateWindow("unamed game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_SIZE,WINDOW_SIZE, 0);

	//create renderer
	rend = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(rend,SCREEN_SIZE,SCREEN_SIZE);

	
}
int main(){
	init();
	Game game;

	time_point frame_start_time =  high_resolution_clock::now();
	time_point time_now =  high_resolution_clock::now();
	
	while(game.loop){
		//keyboard events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type ==SDL_QUIT){
				game.loop = false;
				break;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
				game.step_up();
			}
		}
		//update
		game.update();
		
		//visuals
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);
		
		game.draw();
		
		SDL_RenderPresent(rend);

		int duration = 0;
		while(duration < 1000000/game.get_speed()){ //frame time in micro_s 30fps 
			time_point time_now =  high_resolution_clock::now();
			duration = duration_cast<microseconds>(time_now-frame_start_time).count();
		}
        	frame_start_time = high_resolution_clock::now();

	}
	return 0;
}
