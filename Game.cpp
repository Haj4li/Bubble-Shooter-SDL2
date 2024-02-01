#include "Game.h"



// Sprite

bool Sprite::Load(const char* path, SDL_Renderer* gRenderer)
{
	if (gRenderer == NULL)
		return false;
	mRenderer = gRenderer;
	//Get rid of preexisting texture
	Free();

	//The final texture
	SDL_Texture* newTexture = NULL;


	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
		return false;
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
			SDL_FreeSurface(loadedSurface);
			return false;
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return true;
}

Sprite::~Sprite()
{
	Free();
	
}

void Sprite::SetColor(Uint8 red, Uint8 blue, Uint8 green)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void Sprite::Draw(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(mRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void Sprite::Free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;

		printf("\nSprite Freed.");
	}
}

Vector2 Sprite::GetSize()
{
	return Vector2(mWidth,mHeight);
}

Game::Game(const unsigned int sWidth, const unsigned int sHeight, const char* title)
{
	initialized = true;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		initialized = false;
	}
	else
	{
		TTF_Init();
		// Load a font file
		font = TTF_OpenFont("font.ttf", 20);
		if (!font) {
			// Error handling
			SDL_Log("Failed to load font: %s", TTF_GetError());
			initialized = false;
			return;
		}

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sWidth, sHeight, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			initialized = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				initialized = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					initialized = false;
				}
				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					initialized = false;
				}
				else
				{
					gMusic = Mix_LoadMUS("music.mp3");
					if (gMusic == NULL)
					{
						printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
						initialized = false;
					}
					gShoot = Mix_LoadWAV("shot.mp3");
					if (gMusic == NULL)
					{
						printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
						initialized = false;
					}
					gWin = Mix_LoadWAV("win.mp3");
					if (gMusic == NULL)
					{
						printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
						initialized = false;
					}
					gLose = Mix_LoadWAV("lose.mp3");
					if (gMusic == NULL)
					{
						printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
						initialized = false;
					}
					gHit = Mix_LoadWAV("pop.mp3");
					if (gMusic == NULL)
					{
						printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
						initialized = false;
					}
				}

			}
		}
	}
}

void Game::DrawText(std::string text, int x, int y, SDL_Color color, SDL_Texture* texture)
{
	// Render the text as a surface
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (!surface) {
		// Error handling
		SDL_Log("Failed to render text surface: %s", TTF_GetError());
		TTF_CloseFont(font);
		return;
	}
	// Create a texture from the surface
	texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	if (!texture) {
		// Error handling
		SDL_Log("Failed to create texture: %s", SDL_GetError());
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
		return;
	}
	SDL_Rect dstRect;
	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = surface->w;
	dstRect.h = surface->h;

	SDL_FreeSurface(surface);

	// Render the text texture on the screen
	SDL_RenderCopy(gRenderer, texture, NULL, &dstRect);


	SDL_DestroyTexture(texture);
	texture = NULL;

}

void Game::Update()
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
}

void Game::Present()
{
	SDL_RenderPresent(gRenderer);
}

void Game::PlayEffect(short id)
{
	switch (id)
	{
	case 1:
		Mix_PlayChannel(-1, gShoot, 0);
		break;
	case 2:
		Mix_PlayChannel(-1, gHit, 0);
		break;
	case 3:
		Mix_PlayChannel(-1, gWin, 0);
		break;
	case 4:
		Mix_PlayChannel(-1, gLose, 0);
		break;
	default:
		break;
	}
}

void Game::ToggleMusic()
{
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		Mix_PlayMusic(gMusic, -1);
	}
	//If music is being played
	else
	{
		//If the music is paused
		if (Mix_PausedMusic() == 1)
		{
			//Resume the music
			Mix_ResumeMusic();
		}
		//If the music is playing
		else
		{
			//Pause the music
			Mix_PauseMusic();
		}
	}
}

Game::~Game()
{
	//Free the sound effects
	Mix_FreeChunk(gShoot);
	Mix_FreeChunk(gHit);
	Mix_FreeChunk(gWin);
	Mix_FreeChunk(gLose);
	gShoot = NULL;
	gHit = NULL;
	gWin = NULL;
	gLose = NULL;

	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;


	TTF_CloseFont(font);
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

	gWindow = NULL;
	gRenderer = NULL;

	TTF_Quit();
	Mix_Quit();
	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	printf("\nGame Exit.");
}
