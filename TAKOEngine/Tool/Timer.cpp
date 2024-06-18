#include "Timer.h"

#include <sstream>

void TentacleLib::Timer::Init()
{
	frames = 0;
	fps = 0;
	delta_time = 0.0f;
	frameTimer = 0;
	chrono_start = clock();
	last_point = chrono_start;
}

bool TentacleLib::Timer::Tick()
{
	clock_t current = clock();
	delta_time = static_cast<float>(current - last_point) / CLOCKS_PER_SEC;
	last_point = current;

	frames++;
	frameTimer += delta_time;

	if (frameTimer >= 1.0f)
	{
		fps = frames;
		// Rest timer
		frames = 0;
		frameTimer = 0;

		return true;
	}
	return false;
}