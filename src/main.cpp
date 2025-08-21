#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include "mega_utils/utils_all.h"  // EVERYTHING!!!
#include "mega_utils/utils_sdl2.h"
#include "mega_utils/utils_misc.h"


using namespace std;
#define debug(x) std::cout << #x << " = " << x << std::endl;

int main(int argc, char *argv[]) {
	Camera cam;
	cam.simplyInit();

	Timer t;
	Keyboard keyboard;
	bool loop = true;
	while (loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			keyboard.update(event);
			if (event.type == SDL_QUIT)
				loop = false;
		}

		double dt = t.interval();

		SDL_SetRenderDrawColor(cam.r, 0, 0, 0, 255);
		SDL_RenderClear(cam.r);


		SDL_RenderPresent(cam.r);
	}
	return 0;
}