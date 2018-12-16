#pragma once

#include <string>

namespace osomi{
	class FrameState{
		friend class Resources;
	public:
		FrameState( std::string s, unsigned int of, unsigned int fr, float dur = 1): frames(fr), offset(of), name(s), duration(dur) {}
		unsigned int getFrames() { return frames; }
		unsigned int getOffset() { return offset; }
		std::string getName() { return name; }
		float getDuration() { return duration; }
	protected:
		void update(unsigned int fr) { frames = fr; }
	private:
		unsigned int frames, offset;
		std::string name;
		float duration;
	};
}