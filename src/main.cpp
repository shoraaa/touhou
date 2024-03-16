#include "program.h"

using namespace std;
int main(int argc, char* args[]) {
	cout << "Starting\n";
	Program p;
	if (!p.initialize()) {
		cerr << "Failed to initialize.";
		return 0;
	}
	p.loop();
	p.close();
	return 0;
}
