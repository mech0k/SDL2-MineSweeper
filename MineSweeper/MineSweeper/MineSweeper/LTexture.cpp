#include "LTexture.h"

SDL_Renderer* LTexture::gRenderer = NULL;

void LTexture::free()
{
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render(float x, float y, SDL_Rect* clip, float wMult, float hMult)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (clip != NULL) {
		renderQuad.w = clip->w * wMult;
		renderQuad.h = clip->h * hMult;
	}

	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::LoadFromFile(std::string path)
{
	free();

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xff, 0xff, 0xff));

	SDL_Texture* newTexture;
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;

	SDL_FreeSurface(loadedSurface);

	mTexture = newTexture;
}

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}


LTexture::~LTexture() { free(); }

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }


