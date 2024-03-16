#include "player.h"
#include <SDL.h>

struct Game {
	Player p;

	void initialize() {
		
	}


	void loop(SDL_Event& e) {
		switch (e.key.keysym.sym) {
			case SDLK_UP:
				p.moveUp();
				break;
			case SDLK_DOWN:
				p.moveDown();
				break;
			case SDLK_LEFT:
				p.moveLeft();
				break;
			case SDLK_RIGHT:
				p.moveRight();
				break;
		} 
	}
};
