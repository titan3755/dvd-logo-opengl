#include <application.h>

constexpr auto WIDTH = 1280;
constexpr auto HEIGHT = 720;
constexpr auto TITLE = "OpenGL Learning";

int main(int argc, char* argv[]) {
	srand(time(NULL));
	Application app(WIDTH, HEIGHT, TITLE);
	app.run();
	return 0;
}



