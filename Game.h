#pragma once
#ifndef GAME

#include <SDL.h>
#include <SDL_image.h>

#include <stdio.h>
#include <string>
#include <cmath>

#include <cstdlib>
#include <ctime>

#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <vector>
#include <utility> // for std::pair
#include <chrono>


#include <fstream>

#define GAME
class Game
{
private:
	bool initialized = false;
	SDL_Renderer* gRenderer = NULL;
	SDL_Window* gWindow = NULL;
	TTF_Font* font;

	Mix_Music* gMusic = NULL;

	Mix_Chunk* gShoot = NULL;
	Mix_Chunk* gHit = NULL;
	Mix_Chunk* gWin = NULL;
	Mix_Chunk* gLose = NULL;

public:

	Game(const unsigned int sWidth, const unsigned int sHeight,const char* title);
	bool isInitialized() { return initialized; }
	void DrawText(std::string text, int x, int y,SDL_Color color, SDL_Texture* texture);
	void Update();
	void Present();
	void PlayEffect(short id);
	void ToggleMusic();
	SDL_Renderer* GetRenderer() { return gRenderer; }
	~Game();
};

struct Vector2
{
	int x, y;
	Vector2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};


class Sprite
{
private:
	int mWidth = 0, mHeight = 0;
	SDL_Texture* mTexture = NULL;
	SDL_Renderer* mRenderer;
public:
	bool Load(const char* path, SDL_Renderer* gRenderer);
	~Sprite();
	void SetColor(Uint8 red, Uint8 blue, Uint8 green);
	void Draw(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void Free();
	Vector2 GetSize();
};






#endif // !GAME


