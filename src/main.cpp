#include "program.h"

using namespace std;
int main(int argc, char* args[]) {
	Program p;
	if (!p.initialize()) {
		cout << "Failed to initialize.";
		return 1;
	}
	cout << "HOW\n";

	p.loop();
	p.close();
	return 0;
}
