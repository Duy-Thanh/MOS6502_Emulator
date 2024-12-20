#include "Display.h"

namespace MOS6502 {
	Display::Display() : window(nullptr), renderer(nullptr),
		texture(nullptr), pixels(nullptr) { }

	Display::~Display() {
		try {
			if (this->pixels) delete[] this->pixels;
			if (this->texture) SDL_DestroyTexture(this->texture);
			if (this->renderer) SDL_DestroyRenderer(this->renderer);
			if (this->window) SDL_DestroyWindow(this->window);

			SDL_Quit();
		}
		catch (const std::exception& e) {
			std::cout << "DISPLAY: SDL_Error: " << SDL_GetError() << std::endl;
			std::cout << "DISPLAY: EXCEPTION: " << e.what() << std::endl;
		}
	}

	bool Display::Initialize() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cout << "SDL Error: " << SDL_GetError() << std::endl;
			return false;
		}

		this->window = SDL_CreateWindow("MOS6502 Emulator",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			this->WIDTH * this->PIXEL_SCALE, this->HEIGHT * this->PIXEL_SCALE,
			SDL_WINDOW_SHOWN
		);

		if (!this->window) {
			std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
			return false;
		}

		this->renderer = SDL_CreateRenderer(
			this->window, -1, SDL_RENDERER_ACCELERATED
		);

		if (!this->renderer) {
			std::cout << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
			return false;
		}

		this->texture = SDL_CreateTexture(
			this->renderer,
			SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STREAMING,
			this->WIDTH, this->HEIGHT
		);

		if (!this->texture) {
			std::cout << "Texture could not be created! Error: " << SDL_GetError() << std::endl;
			return false;
		}

		this->pixels = new uint32_t[this->WIDTH * this->HEIGHT];
		std::memset(this->pixels, 0, this->WIDTH * this->HEIGHT);

		return true;
	}

	void Display::WritePixel(uint16_t addr, uint8_t value) {
		if (addr >= this->DISPLAY_START && addr < this->DISPLAY_END) {
			uint16_t pixel_addr = addr - this->DISPLAY_START;
			if (pixel_addr < WIDTH * HEIGHT) {  // Add bounds check
				// Convert 8-bit color to 32-bit ARGB
				uint32_t color = (value << 16) | (value << 8) | value;
				this->pixels[pixel_addr] = color | 0xFF000000; // Set alpha to full

				// Debug output
				std::cout << "DISPLAY: Writing pixel at " << pixel_addr
					<< " with value " << (int)value << std::endl;
			}
		}
	}

	void Display::Update() {
		SDL_UpdateTexture(this->texture, NULL, this->pixels, this->WIDTH * sizeof(uint32_t));
		SDL_RenderClear(this->renderer);
		SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
		SDL_RenderPresent(this->renderer);
	}
}