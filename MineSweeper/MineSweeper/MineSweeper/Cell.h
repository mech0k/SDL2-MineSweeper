#pragma once
class Cell
{
public:
	static int numFlags;
	Cell();
	Cell(int x, int y);
	~Cell();
	void setX(int x);
	void setY(int y);
	void setState(int state);
	void setRev(bool rev);
	void setFlag(bool flag);
	int getX();
	int getY();
	int getState();
	bool getRev();
	bool getFlag();
private:
	int x;
	int y;
	int state;
	bool rev = false;
	bool flag = false;
};

