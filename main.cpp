#include <iostream>
#include "includes.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

unique_ptr<WAV> beep;

Entity paddle(515, 600, 300, 20);
float speed = 2.5;

Entity ball(635, 355, 30, 30);
float ballSpeed = 0.5;
bool hDirection = false;
bool vDirection = false;

vector<vector<Entity>> bricks;

Uint64 now = SDL_GetPerformanceCounter();
Uint64 last = 0;
float deltaTime = 0;

SDL_Event event;

void mainLoop() {
	last = now;
	now = SDL_GetPerformanceCounter();
	deltaTime = static_cast<float>((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());

	int numkeys;
	const bool* state = SDL_GetKeyboardState(&numkeys);

	while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) isRunning = false;

	if (state[SDL_SCANCODE_D]) paddle.updatePosition(paddle.getX() + speed * deltaTime, paddle.getY());
	if (state[SDL_SCANCODE_A]) paddle.updatePosition(paddle.getX() - speed * deltaTime, paddle.getY());

	ball.updatePosition(ball.getX() + (hDirection ? ballSpeed * deltaTime : -ballSpeed * deltaTime),
		ball.getY() + (vDirection ? -ballSpeed * deltaTime : ballSpeed * deltaTime));

	if (paddle.checkCollision(ball)) {
		float xcenter = paddle.getX() + 150;
		hDirection = ball.getX() + 15 < xcenter ? false : true;

		vDirection = true;

		beep->play();
	}

	if (ball.getX() < 0) {
		hDirection = true;
		beep->play();
	}
	else if (ball.getX() + 30 > 1280) {
		hDirection = false;
		beep->play();
	}

	if (ball.getY() < 0) {
		vDirection = false;
		beep->play();
	}
	else if (ball.getY() > 720) {
		hDirection = !hDirection;
		vDirection = false;

		ball.updatePosition(635, 355);
	}

	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderClear(renderer);
	paddle.render(WHITE);
	ball.render(WHITE);
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 3; j++) {
			if (j % 2 == 0) bricks[j][i].render(i % 2 == 0 ? SDL_Color(WHITE) : SDL_Color(GRAY));
			else bricks[j][i].render(i % 2 == 0 ? SDL_Color(GRAY) : SDL_Color(WHITE));

			if (bricks[j][i].checkCollision(ball)) {
				bricks[j][i].rendering = false;
				bricks[j][i].ignore = true;
				vDirection = !vDirection;
				beep->play();
			}
		}

	SDL_RenderPresent(renderer);
}

int main() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		cerr << "Failed to initialize SDL, error: " << SDL_GetError() << endl;
		return 1;
	}

	if (!SDL_CreateWindowAndRenderer("Brick Breaker", 1280, 720, 0, &window, &renderer)) {
		cerr << "Failed to create window/renderer, error: " << SDL_GetError() << endl;
		return 1;
	}
	SDL_SetRenderLogicalPresentation(renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	beep = make_unique<WAV>("beep.wav");

	vector<Entity> row1, row2, row3;
	bricks.push_back(row1);
	bricks.push_back(row2);
	bricks.push_back(row3);

	for (int i = 0; i < 10; i++) for (int j = 0; j < 3; j++) bricks[j].push_back(Entity(i * 128, j * 36, 128, 36));

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainLoop, 0, 1);
#else
	while (isRunning) mainLoop();
#endif

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
