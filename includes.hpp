#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <memory>

#define WHITE {255, 255, 255, 255}

using namespace std;

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static bool isRunning = true;

class Entity {
protected:
	SDL_FRect rect = { };

public:
	Entity(float x, float y, float w, float h) {
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
	}

	float getX() {
		return rect.x;
	}

	float getY() {
		return rect.y;
	}

	void updatePosition(float x, float y) {
		rect.x = x;
		rect.y = y;
	}

	bool checkCollision(Entity& other) {
		return !(rect.x + rect.w < other.rect.x ||
			rect.x > other.rect.x + other.rect.w ||
			rect.y + rect.h < other.rect.y ||
			rect.y > other.rect.y + other.rect.h);
	}

	void render(SDL_Renderer* renderer, SDL_Color color) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect);
	}
};

class WAV {
	SDL_AudioStream* stream;
	Uint8* wavData;
	Uint32 wavDataLen;

	public:
		WAV(string filepath) {
			const char* wavPath = filepath.c_str();
			SDL_AudioSpec spec = { };
			if (!SDL_LoadWAV(wavPath, &spec, &wavData, &wavDataLen)) cerr << "Failed to load WAV file: " << SDL_GetError() << endl;
			
			stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
			if (!stream) cerr << "Failed to open audio device stream: " << SDL_GetError() << endl;
			else SDL_ResumeAudioStreamDevice(stream);
		}

		~WAV() {
			SDL_free(wavData);
		}

		void play() {
			if (stream) if (SDL_GetAudioStreamQueued(stream) < (int)wavDataLen) SDL_PutAudioStreamData(stream, wavData, wavDataLen);
		}
};
