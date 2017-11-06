#include "LTexture.h"
#include "Cell.h"
#include <time.h>
#include <Windows.h>

int const EASY_W = 144; // 9x9 10
int const EASY_H = EASY_W;
int const NORMAL_W = 256; // 16x16 40
int const NORMAL_H = NORMAL_W;
int const HARD_W = 480; // 16x30 99
int const HARD_H = NORMAL_H;

int const MULT = 2;

int const MINES = 40;
int const SCREEN_WIDTH = (NORMAL_W + 24) * MULT; // width
int const SCREEN_HEIGHT = (NORMAL_H + 62) * MULT; // height

const char* TITLE = "MineSweeper";
const char* gSpriteSheetTexturePath = "MineSweeperSprite.png"; // path

SDL_Window* gWindow;

SDL_Rect gSpriteClips[45];
LTexture gSpriteSheetTexture;

enum SmileFaceState {
	NORMAL,
	NORMAL_CLICKED,
	CLICK,
	WIN,
	LOSE,
};
int currentState = NORMAL;

const int wCells = (SCREEN_WIDTH - 24 * MULT) / (16 * MULT);
const int hCells = (SCREEN_HEIGHT - 62 * MULT) / (16 * MULT);
Cell allCells[hCells][wCells];
int currentSelect[2] = { -1, -1 };

void drawASign(int n, float x0, float y0);
void drawBorder();
void drawSmile();
void initCells();
void drawCells();
void generateMines(int nMines, int initX, int initY);
void setStates();
void revealEmpty();
void endGame();
void revealAll();
void restart();
void smileState(int &cycle, int upper);
bool revealCurrent();
bool checkWin();

void init();
void loadMedia();
void close();

bool firstClick = false;
bool endGameBool = false;
bool sadFace = false;


int wmain() {

	FreeConsole();

	init();
	loadMedia();
	initCells();
	int cycle = 0;
	int upper = 250;
	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					if (e.button.x > SCREEN_WIDTH / 2 - gSpriteClips[currentState + 29].w*MULT / 2 && e.button.y > gSpriteClips[16].h*MULT + (28 / 2) * MULT - gSpriteClips[currentState + 29].h*MULT / 2 && e.button.x <= (SCREEN_WIDTH / 2 - gSpriteClips[currentState + 29].w*MULT / 2) + 26 * MULT && e.button.y <= (gSpriteClips[16].h*MULT + (28 / 2) * MULT - gSpriteClips[currentState + 29].h*MULT / 2) + 26 * MULT) {
						currentState = NORMAL_CLICKED;
						cycle = 0;
						upper = 200;
						endGameBool = true;
					}
					else if (e.button.x > 12 * MULT && e.button.x < SCREEN_WIDTH - 12 * MULT && e.button.y > 50 * MULT && e.button.y < SCREEN_HEIGHT - 12 * MULT) {
						int mouseX = (e.button.x - 12 * MULT) / (16 * MULT);
						int mouseY = (e.button.y - 50 * MULT) / (16 * MULT);
						currentSelect[0] = mouseY;
						currentSelect[1] = mouseX;
						if (!checkWin() && !firstClick) {
							firstClick = true;
							generateMines(MINES, mouseX, mouseY);
							setStates();
							revealEmpty();
							currentState = CLICK;
							cycle = 0;
							//testReveal();
						}
						else if (!checkWin() && !allCells[mouseY][mouseX].getRev()) {
							currentSelect[0] = mouseY;
							currentSelect[1] = mouseX;
							if (allCells[mouseY][mouseX].getFlag() == false) {
								allCells[mouseY][mouseX].setRev(true);
								currentState = CLICK;
								cycle = 0;
								if (allCells[mouseY][mouseX].getState() == -1 && !endGameBool) {
									allCells[mouseY][mouseX].setState(-2);
									sadFace = true;
									currentState = LOSE;
									endGame();
									revealAll();
								}
								revealEmpty();
							}
						}
					}
				}
				else if (e.button.button == SDL_BUTTON_RIGHT) {
					if (!checkWin() && !sadFace) {
						if (firstClick && e.button.x > 12 * MULT && e.button.x < SCREEN_WIDTH - 12 * MULT && e.button.y > 50 * MULT && e.button.y < SCREEN_HEIGHT - 12 * MULT) {
							int mouseX = (e.button.x - 12 * MULT) / (16 * MULT);
							int mouseY = (e.button.y - 50 * MULT) / (16 * MULT);
							if (currentSelect[0] == mouseY && currentSelect[1] == mouseX) {
								if (!revealCurrent()) {
									sadFace = true;
									currentState = LOSE;
									endGame();
									revealAll();
								}
								revealEmpty();
							}
							else if (allCells[mouseY][mouseX].getRev() == false && allCells[mouseY][mouseX].getFlag() == false) {
								allCells[mouseY][mouseX].setFlag(true);
								Cell::numFlags++;
							}
							else if (allCells[mouseY][mouseX].getRev() == false && allCells[mouseY][mouseX].getFlag() == true) {
								allCells[mouseY][mouseX].setFlag(false);
								Cell::numFlags--;
							}
						}
					}
				}
			}
		}

		SDL_SetRenderDrawColor(LTexture::gRenderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(LTexture::gRenderer);

		//draw
		drawBorder();
		drawSmile();
		drawCells();

		SDL_RenderPresent(LTexture::gRenderer);

		cycle++;
		smileState(cycle, upper);
	}
	close();
	return 0;
}


void drawASign(int n, float x0, float y0)
{
	switch (n)
	{
	case -3:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[7], MULT, MULT);
		break;
	case -2:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[6], MULT, MULT);
		break;
	case -1:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[5], MULT, MULT);
		break;
	case 0:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[1], MULT, MULT);
		break;
	case 1:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[8], MULT, MULT);
		break;
	case 2:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[9], MULT, MULT);
		break;
	case 3:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[10], MULT, MULT);
		break;
	case 4:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[11], MULT, MULT);
		break;
	case 5:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[12], MULT, MULT);
		break;
	case 6:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[13], MULT, MULT);
		break;
	case 7:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[14], MULT, MULT);
		break;
	case 8:
		gSpriteSheetTexture.render(x0, y0, &gSpriteClips[15], MULT, MULT);
		break;
	default:
		break;
	}
}

void drawBorder()
{
	//background rectangle
	SDL_Rect topBackground;
	topBackground.x = gSpriteClips[16].w * MULT;
	topBackground.y = gSpriteClips[16].h * MULT;
	topBackground.w = SCREEN_WIDTH - gSpriteClips[18].w * MULT - gSpriteClips[16].w * MULT;
	topBackground.h = 28 * gSpriteClips[19].h * MULT;
	SDL_SetRenderDrawColor(LTexture::gRenderer, 192, 192, 192, 0xff);
	SDL_RenderFillRect(LTexture::gRenderer, &topBackground);
	SDL_SetRenderDrawColor(LTexture::gRenderer, 0xff, 0xff, 0xff, 0xff);

	//top smile rectangle
	SDL_Rect smileRec;
	smileRec.x = SCREEN_WIDTH / 2 - gSpriteClips[currentState + 29].w*MULT / 2;
	smileRec.y = gSpriteClips[16].h*MULT + (28 / 2) * MULT - gSpriteClips[currentState + 29].h*MULT / 2;
	smileRec.w = 26 * MULT;
	smileRec.h = 26 * MULT;
	SDL_RenderFillRect(LTexture::gRenderer, &smileRec);

	//current height
	int curH = 0;

	//top
	gSpriteSheetTexture.render(0, 0, &gSpriteClips[16], MULT, MULT);
	for (int i = gSpriteClips[16].w*MULT; i < SCREEN_WIDTH - gSpriteClips[18].w*MULT; i += MULT)
	{
		gSpriteSheetTexture.render(gSpriteClips[17].w*i, 0, &gSpriteClips[17], MULT, MULT);
	}
	gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[18].w*MULT, 0, &gSpriteClips[18], MULT, MULT);
	curH += gSpriteClips[16].h;

	//top to mid
	for (int i = curH*MULT; i < curH*MULT + 28 * MULT; i += MULT)
	{
		gSpriteSheetTexture.render(0, gSpriteClips[19].h*i, &gSpriteClips[19], MULT, MULT);
		gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[20].w*MULT, gSpriteClips[20].h*i, &gSpriteClips[20], MULT, MULT);
	}
	curH += 28 * gSpriteClips[19].h;

	//mid
	gSpriteSheetTexture.render(0, curH*MULT, &gSpriteClips[21], MULT, MULT);
	for (int i = gSpriteClips[21].w * MULT; i < SCREEN_WIDTH - gSpriteClips[23].w * MULT; i += MULT)
	{
		gSpriteSheetTexture.render(gSpriteClips[22].w*i, curH * MULT, &gSpriteClips[22], MULT, MULT);
	}
	gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[23].w*MULT, curH * MULT, &gSpriteClips[23], MULT, MULT);
	curH += gSpriteClips[21].h;

	//mid to bot
	for (int i = curH * MULT; i < SCREEN_HEIGHT - gSpriteClips[25].w*MULT; i += MULT)
	{
		gSpriteSheetTexture.render(0, gSpriteClips[24].h*i, &gSpriteClips[24], MULT, MULT);
		gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[25].w*MULT, gSpriteClips[25].h*i, &gSpriteClips[25], MULT, MULT);
	}

	//bot
	gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[26].h * MULT, &gSpriteClips[26], MULT, MULT);
	for (int i = gSpriteClips[26].w * MULT; i < SCREEN_WIDTH - gSpriteClips[28].w * MULT; i += MULT)
	{
		gSpriteSheetTexture.render(gSpriteClips[27].w*i, SCREEN_HEIGHT - gSpriteClips[27].h * MULT, &gSpriteClips[27], MULT, MULT);
	}
	gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[28].w*MULT, SCREEN_HEIGHT - gSpriteClips[28].h * MULT, &gSpriteClips[28], MULT, MULT);
}

void drawSmile()
{
	gSpriteSheetTexture.render(SCREEN_WIDTH / 2 - gSpriteClips[currentState + 29].w*MULT / 2, gSpriteClips[16].h*MULT + (28 / 2) * MULT - gSpriteClips[currentState + 29].h*MULT / 2, &gSpriteClips[currentState + 29], MULT, MULT);
}

void initCells()
{
	int x0 = 12;
	int y0 = 50;
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			allCells[i][j] = Cell(x0 * MULT + j * MULT * 16, y0 * MULT + i * MULT * 16);
		}
	}
}

void drawCells()
{
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getRev() == false) {
				if (allCells[i][j].getFlag() == false) {
					gSpriteSheetTexture.render(allCells[i][j].getX(), allCells[i][j].getY(), &gSpriteClips[0], MULT, MULT);
				}
				else {
					gSpriteSheetTexture.render(allCells[i][j].getX(), allCells[i][j].getY(), &gSpriteClips[2], MULT, MULT);
				}
			}
			else {
				drawASign(allCells[i][j].getState(), allCells[i][j].getX(), allCells[i][j].getY());
			}
		}
	}
}

void generateMines(int nMines, int initX, int initY)
{

	allCells[initY][initX].setRev(true);
	if (initY - 1 >= 0 && initX - 1 >= 0) allCells[initY - 1][initX - 1].setRev(true);
	if (initY - 1 >= 0) allCells[initY - 1][initX].setRev(true);
	if (initY - 1 >= 0 && initX + 1 < wCells) allCells[initY - 1][initX + 1].setRev(true);
	if (initX - 1 >= 0) allCells[initY][initX - 1].setRev(true);
	if (initX + 1 < wCells) allCells[initY][initX + 1].setRev(true);
	if (initY + 1 < hCells && initX - 1 >= 0) allCells[initY + 1][initX - 1].setRev(true);
	if (initY + 1 < hCells) allCells[initY + 1][initX].setRev(true);
	if (initY + 1 < hCells && initX + 1 < wCells) allCells[initY + 1][initX + 1].setRev(true);


	int xRand;
	int cellX; // w
	int cellY; // h
	srand(time(0));
	for (int i = 0; i < nMines; i++)
	{
		xRand = rand() % (hCells*wCells);
		cellX = xRand % wCells;
		cellY = (xRand / wCells) % hCells;

		if (allCells[cellY][cellX].getRev() == false && allCells[cellY][cellX].getState() != -1) {
			allCells[cellY][cellX].setState(-1);
		}
		else {
			i--;
		}
	}

}

void setStates()
{
	int currC;
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getState() == -1) continue;

			currC = 0;
			if (i - 1 >= 0 && j - 1 >= 0 && allCells[i - 1][j - 1].getState() == -1) currC++;
			if (i - 1 >= 0 && allCells[i - 1][j].getState() == -1) currC++;
			if (i - 1 >= 0 && j + 1 < wCells && allCells[i - 1][j + 1].getState() == -1) currC++;
			if (j - 1 >= 0 && allCells[i][j - 1].getState() == -1) currC++;
			if (j + 1 < wCells && allCells[i][j + 1].getState() == -1) currC++;
			if (i + 1 < hCells && j - 1 >= 0 && allCells[i + 1][j - 1].getState() == -1) currC++;
			if (i + 1 < hCells && allCells[i + 1][j].getState() == -1) currC++;
			if (i + 1 < hCells && j + 1 < wCells && allCells[i + 1][j + 1].getState() == -1) currC++;

			allCells[i][j].setState(currC);
		}
	}
}

void revealEmpty()
{
	bool trigger = false;
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getState() == 0 && allCells[i][j].getRev() == true) {
				if ((i - 1 >= 0 && j - 1 >= 0 && allCells[i - 1][j - 1].getRev() == false) ||
					(i - 1 >= 0 && allCells[i - 1][j].getRev() == false) ||
					(i - 1 >= 0 && j + 1 < wCells && allCells[i - 1][j + 1].getRev() == false) ||
					(j - 1 >= 0 && allCells[i][j - 1].getRev() == false) ||
					(j + 1 < wCells && allCells[i][j + 1].getRev() == false) ||
					(i + 1 < hCells && j - 1 >= 0 && allCells[i + 1][j - 1].getRev() == false) ||
					(i + 1 < hCells && allCells[i + 1][j].getRev() == false) ||
					(i + 1 < hCells && j + 1 < wCells && allCells[i + 1][j + 1].getRev() == false)) {
					trigger = true;

					if (i - 1 >= 0 && j - 1 >= 0) allCells[i - 1][j - 1].setRev(true);
					if (i - 1 >= 0) allCells[i - 1][j].setRev(true);
					if (i - 1 >= 0 && j + 1 < wCells) allCells[i - 1][j + 1].setRev(true);
					if (j - 1 >= 0) allCells[i][j - 1].setRev(true);
					if (j + 1 < wCells) allCells[i][j + 1].setRev(true);
					if (i + 1 < hCells && j - 1 >= 0) allCells[i + 1][j - 1].setRev(true);
					if (i + 1 < hCells) allCells[i + 1][j].setRev(true);
					if (i + 1 < hCells && j + 1 < wCells) allCells[i + 1][j + 1].setRev(true);
				}
			}
		}
	}

	if (trigger) revealEmpty();
}

void endGame()
{
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getState() != -1 && allCells[i][j].getFlag() == true) {
				allCells[i][j].setFlag(false);
				allCells[i][j].setRev(true);
				allCells[i][j].setState(-3);
			}
		}
	}

	currentSelect[0] = -1;
	currentSelect[1] = -1;
}

void revealAll()
{
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getState() == -1 && allCells[i][j].getFlag() == true) {

			}
			else {
				allCells[i][j].setRev(true);
			}
		}
	}
}

void restart()
{
	firstClick = false;
	endGameBool = false;
	sadFace = false;
	currentState = NORMAL;
	initCells();
}

void smileState(int &cycle, int upper)
{
	if (cycle == upper) {
		cycle = 0;
		if (endGameBool) {
			restart();
		}

		if (checkWin()) {
			currentState = WIN;
		}
		else if (!sadFace) {
			currentState = NORMAL;
		}
		else {
			currentState = LOSE;
		}
		upper = 250;
	}
}

bool revealCurrent()
{
	bool ok = true;
	int i = currentSelect[0];
	int j = currentSelect[1];
	if (allCells[i][j].getRev() == true) {
		if (i - 1 >= 0 && j - 1 >= 0) {
			allCells[i - 1][j - 1].setRev(true);
			if (allCells[i - 1][j - 1].getState() == -1 && allCells[i - 1][j - 1].getFlag() == false) {
				allCells[i - 1][j - 1].setState(-2);
				ok = false;
			}
			else if (allCells[i - 1][j - 1].getFlag() == true) {
				allCells[i - 1][j - 1].setRev(false);
			}
		}
		if (i - 1 >= 0) {
			allCells[i - 1][j].setRev(true);
			if (allCells[i - 1][j].getState() == -1 && allCells[i - 1][j].getFlag() == false) {
				allCells[i - 1][j].setState(-2);
				ok = false;
			}
			else if (allCells[i - 1][j].getFlag() == true) {
				allCells[i - 1][j].setRev(false);
			}
		}
		if (i - 1 >= 0 && j + 1 < wCells) {
			allCells[i - 1][j + 1].setRev(true);
			if (allCells[i - 1][j + 1].getState() == -1 && allCells[i - 1][j + 1].getFlag() == false) {
				allCells[i - 1][j + 1].setState(-2);
				ok = false;
			}
			else if (allCells[i - 1][j + 1].getFlag() == true) {
				allCells[i - 1][j + 1].setRev(false);
			}
		}
		if (j - 1 >= 0) {
			allCells[i][j - 1].setRev(true);
			if (allCells[i][j - 1].getState() == -1 && allCells[i][j - 1].getFlag() == false) {
				allCells[i][j - 1].setState(-2);
				ok = false;
			}
			else if (allCells[i][j - 1].getFlag() == true) {
				allCells[i][j - 1].setRev(false);
			}
		}
		if (j + 1 < wCells) {
			allCells[i][j + 1].setRev(true);
			if (allCells[i][j + 1].getState() == -1 && allCells[i][j + 1].getFlag() == false) {
				allCells[i][j + 1].setState(-2);
				ok = false;
			}
			else if (allCells[i][j + 1].getFlag() == true) {
				allCells[i][j + 1].setRev(false);
			}
		}
		if (i + 1 < hCells && j - 1 >= 0) {
			allCells[i + 1][j - 1].setRev(true);
			if (allCells[i + 1][j - 1].getState() == -1 && allCells[i + 1][j - 1].getFlag() == false) {
				allCells[i + 1][j - 1].setState(-2);
				ok = false;
			}
			else if (allCells[i + 1][j - 1].getFlag() == true) {
				allCells[i + 1][j - 1].setRev(false);
			}
		}
		if (i + 1 < hCells) {
			allCells[i + 1][j].setRev(true);
			if (allCells[i + 1][j].getState() == -1 && allCells[i + 1][j].getFlag() == false) {
				allCells[i + 1][j].setState(-2);
				ok = false;
			}
			else if (allCells[i + 1][j].getFlag() == true) {
				allCells[i + 1][j].setRev(false);
			}
		}
		if (i + 1 < hCells && j + 1 < wCells) {
			allCells[i + 1][j + 1].setRev(true);
			if (allCells[i + 1][j + 1].getState() == -1 && allCells[i + 1][j + 1].getFlag() == false) {
				allCells[i + 1][j + 1].setState(-2);
				ok = false;
			}
			else if (allCells[i + 1][j + 1].getFlag() == true) {
				allCells[i + 1][j + 1].setRev(false);
			}
		}
	}

	return ok;
}

bool checkWin()
{
	bool win = true;
	int bC = 0;
	for (int i = 0; i < hCells; i++)
	{
		for (int j = 0; j < wCells; j++)
		{
			if (allCells[i][j].getRev() == false && allCells[i][j].getFlag() == false) {
				win = false;
			}
			if (allCells[i][j].getState() == -1 && allCells[i][j].getFlag() == true && allCells[i][j].getRev() == false) {
				bC++;
			}
		}
	}

	if (bC == MINES && win == true) return true;

	return false;
}

void init()
{
	SDL_Init(SDL_INIT_VIDEO);
	gWindow = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	LTexture::gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(LTexture::gRenderer, 0xff, 0xff, 0xff, 0xff);
	IMG_Init(IMG_INIT_PNG);
}

void loadMedia()
{
	gSpriteSheetTexture.LoadFromFile(gSpriteSheetTexturePath);

	//first 2 rows - 16
	int num = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			gSpriteClips[num].x = j * 17;
			gSpriteClips[num].y = i * 17;
			gSpriteClips[num].w = 16;
			gSpriteClips[num].h = 16;
			num++;
		}
	}

	//borders
	gSpriteClips[16].x = 0; // top left
	gSpriteClips[16].y = 85;
	gSpriteClips[16].w = 12;
	gSpriteClips[16].h = 11;

	gSpriteClips[17].x = 13; // top mid
	gSpriteClips[17].y = 85;
	gSpriteClips[17].w = 1;
	gSpriteClips[17].h = 11;

	gSpriteClips[18].x = 15; // top right
	gSpriteClips[18].y = 85;
	gSpriteClips[18].w = 12;
	gSpriteClips[18].h = 11;

	gSpriteClips[19].x = 0; // side under top left
	gSpriteClips[19].y = 97;
	gSpriteClips[19].w = 12;
	gSpriteClips[19].h = 1;

	gSpriteClips[20].x = 15; // side under top right
	gSpriteClips[20].y = 97;
	gSpriteClips[20].w = 12;
	gSpriteClips[20].h = 1;

	gSpriteClips[21].x = 0; // mid left
	gSpriteClips[21].y = 99;
	gSpriteClips[21].w = 12;
	gSpriteClips[21].h = 11;

	gSpriteClips[22].x = 13; // mid mid
	gSpriteClips[22].y = 99;
	gSpriteClips[22].w = 1;
	gSpriteClips[22].h = 11;

	gSpriteClips[23].x = 15; // mid right
	gSpriteClips[23].y = 99;
	gSpriteClips[23].w = 12;
	gSpriteClips[23].h = 11;

	gSpriteClips[24].x = 0; // side under mid left
	gSpriteClips[24].y = 111;
	gSpriteClips[24].w = 12;
	gSpriteClips[24].h = 1;

	gSpriteClips[25].x = 15; // side under mid right
	gSpriteClips[25].y = 111;
	gSpriteClips[25].w = 12;
	gSpriteClips[25].h = 1;

	gSpriteClips[26].x = 0; // bot left
	gSpriteClips[26].y = 113;
	gSpriteClips[26].w = 12;
	gSpriteClips[26].h = 12;

	gSpriteClips[27].x = 13; // bot mid
	gSpriteClips[27].y = 113;
	gSpriteClips[27].w = 1;
	gSpriteClips[27].h = 12;

	gSpriteClips[28].x = 15; // bot right
	gSpriteClips[28].y = 113;
	gSpriteClips[28].w = 12;
	gSpriteClips[28].h = 12;

	//smile
	gSpriteClips[29].x = 0;
	gSpriteClips[29].y = 58;
	gSpriteClips[29].w = 26;
	gSpriteClips[29].h = 26;

	gSpriteClips[30].x = 27;
	gSpriteClips[30].y = 58;
	gSpriteClips[30].w = 26;
	gSpriteClips[30].h = 26;

	gSpriteClips[31].x = 54;
	gSpriteClips[31].y = 58;
	gSpriteClips[31].w = 26;
	gSpriteClips[31].h = 26;

	gSpriteClips[32].x = 81;
	gSpriteClips[32].y = 58;
	gSpriteClips[32].w = 26;
	gSpriteClips[32].h = 26;

	gSpriteClips[33].x = 108;
	gSpriteClips[33].y = 58;
	gSpriteClips[33].w = 26;
	gSpriteClips[33].h = 26;
}

void close()
{
	gSpriteSheetTexture.free();

	gWindow = NULL;
	LTexture::gRenderer = NULL;

	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(LTexture::gRenderer);

	IMG_Quit();
	SDL_Quit();
}