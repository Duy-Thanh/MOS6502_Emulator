#include "Display.h"

namespace MOS6502 {
	Display::Display() : window(nullptr), renderer(nullptr),
		texture(nullptr), pixels(nullptr) { }

	Display::~Display() {
		if (pixels) {
			delete[] pixels;
			pixels = nullptr;
		}
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
		if (renderer) {
			SDL_DestroyRenderer(renderer);
			renderer = nullptr;
		}
		if (window) {
			SDL_DestroyWindow(window);
			window = nullptr;
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
			if (pixel_addr < WIDTH * HEIGHT) {
				// Make color more visible and add debug output
				uint32_t color = (value > 0) ? 0xFFFFFFFF : 0xFF000000;
				this->pixels[pixel_addr] = color;
				
				if (pixel_addr % 256 == 0) {  // Log every 256th pixel to avoid spam
					std::cout << "DISPLAY: Writing pixel at " << std::dec << pixel_addr 
								<< " with value 0x" << std::hex << (int)value 
								<< " (color: 0x" << std::hex << color << ")" << std::endl;
				}
			}
		}
	}

	void Display::Update() {
		//static int update_count = 0;
		//if (update_count++ % 60 == 0) {  // Log every 60 frames
		//	std::cout << "DISPLAY: Updating frame " << update_count << std::endl;
		//}
		
		SDL_UpdateTexture(this->texture, NULL, this->pixels, this->WIDTH * sizeof(uint32_t));
		SDL_RenderClear(this->renderer);
		SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
		SDL_RenderPresent(this->renderer);
	}
}