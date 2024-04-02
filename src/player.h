

struct Player {
	const int DELTA_X = 5;
	const int DELTA_Y = 5;
	int x = 0, y = 0;

	void update(const SDL_Event& e) {
		switch (e.key.keysym.sym) {
			case SDLK_UP:
				moveUp();
				break;
			case SDLK_DOWN:
				moveDown();
				break;
			case SDLK_LEFT:
				moveLeft();
				break;
			case SDLK_RIGHT:
				moveRight();
				break;
		} 
	}

	void moveUp() {
		y -= DELTA_Y;
	}

	void moveDown() {
		y += DELTA_Y;
	}

	void moveRight() {
		x += DELTA_X;
	}

	void moveLeft() {
		x -= DELTA_X;
	}
	
};
