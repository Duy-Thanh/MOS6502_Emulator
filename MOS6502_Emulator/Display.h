#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <iostream>

namespace MOS6502 {
	class Display {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
		uint32_t* pixels;

		static const int WIDTH = 256;
		static const int HEIGHT = 240;
		static const int PIXEL_SCALE = 2;
	public:
		Display();
		~Display();

		void Update();

		void WritePixel(uint16_t addr, uint8_t value);

		bool Initialize();

		// Memory mapped registers
		static const uint16_t DISPLAY_START = 0x2000;
		static const uint16_t DISPLAY_END = 0x3FFF;
	};
}