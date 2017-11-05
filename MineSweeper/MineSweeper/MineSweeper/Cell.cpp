#include "Cell.h"


int Cell::numFlags = 0;

Cell::Cell()
{
	this->x = 0;
	this->y = 0;
	this->state = 0;
}

Cell::Cell(int x, int y)
{
	this->x = x;
	this->y = y;
	this->state = 0;
}

Cell::~Cell()
{
}

void Cell::setX(int x) { this->x = x; }

void Cell::setY(int y) { this->y = y; }

void Cell::setState(int state) { this->state = state; }

void Cell::setRev(bool rev) { this->rev = rev; }

void Cell::setFlag(bool flag) { this->flag = flag; }

int Cell::getX() { return this->x; }

int Cell::getY() { return this->y; }

int Cell::getState() { return this->state; }

bool Cell::getRev() { return this->rev; }

bool Cell::getFlag() { return this->flag; }
