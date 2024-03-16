

struct Player {
	const int DELTA_X = 5;
	const int DELTA_Y = 5;
	int x = 0, y = 0;

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
