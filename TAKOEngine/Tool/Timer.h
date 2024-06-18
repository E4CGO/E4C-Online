#pragma once

#include <time.h>

namespace TentacleLib {
	class Timer
	{
	private:
		Timer() = default;
		~Timer() = default;
	public:
		// インスタンス
		static Timer& Instance()
		{
			static Timer instance;
			return instance;
		}

		void Init();
		bool Tick();

		inline int getFPS() { return fps; };
		inline float Delta() { return delta_time; }
	private:
		int frames = 0;
		int fps = 0;
		float delta_time = 0.0f;
		float frameTimer = 0;
		clock_t chrono_start;
		clock_t last_point;
	};
}
