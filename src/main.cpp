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

	ParticleS psystem;
	psystem.create({50, (double)cam.h / 2}, 2, 600, 0, .2, 1.5, 255, 255, 255);
	psystem.setRandomises(0.2, .9, .2);
	psystem.setSpawnInterval(.002);
	Smooth spawnSmooth(.001);

	Timer t;
	bool loop = true;
	double smooth = 0;
	while (loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				loop = false;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_ESCAPE:
					loop = false;
					break;

				case SDL_SCANCODE_SPACE:
					spawnSmooth.want(1);
					break;

				default:
					break;
				}

				break;

			case SDL_KEYUP:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_SPACE:
					spawnSmooth.want(0);
					break;

				default:
					break;
				}
				break;
			default:
				break;
			}
		}

		double dt = t.interval();
		psystem.update(dt, spawnSmooth.getVal());

		SDL_SetRenderDrawColor(cam.r, 0, 0, 0, 255);
		SDL_RenderClear(cam.r);

		psystem.render(&cam);

		SDL_RenderPresent(cam.r);
	}
	return 0;
}