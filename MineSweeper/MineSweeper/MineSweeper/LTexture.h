#pragma once
#include "libs.h"

class LTexture
{
public:
	static SDL_Renderer* gRenderer;
	void LoadFromFile(std::string path);
	void free();
	void render(float x, float y, SDL_Rect* clip = NULL, float wMult = 1, float hMult = 1);
	void setColor(Uint8 r, Uint8 g, Uint8 b);
	LTexture();
	~LTexture();

	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};