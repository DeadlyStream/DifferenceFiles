#include "cli.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

char getHourglassCharacter() {
	static auto past = system_clock::now();
	const long long msToWait = 250;

	static const char frames[]{ '-', '\\', '|', '/' };
	const int frameCount = sizeof(frames);
	static int currentFrame = 0;

	const auto now = system_clock::now();
	const auto delta = duration_cast<milliseconds>(now - past).count();

	if( delta >= msToWait ) {
		currentFrame = (currentFrame + 1) % frameCount;
		past = system_clock::now();
	}

	return frames[currentFrame];
}