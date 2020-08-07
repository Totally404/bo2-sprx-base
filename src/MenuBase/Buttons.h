enum Buttons
{
	Null = 0,
	X = 1,
	O = 2,
	SQUARE = 3,
	TRIANGLE = 4,
	L1 = 5,
	R1 = 6,
	START = 14,
	SELECT = 15,
	L3 = 16,
	R3 = 17,
	L2 = 18,
	R2 = 19,
	UP = 20,
	DOWN = 21,
	LEFT = 22,
	RIGHT = 23,
	APAD_UP = 28,
	APAD_DOWN = 29,
	APAD_LEFT = 30,
	APAD_RIGHT = 31
};

bool detect(int Button)
{
	int Key_IsDown_t[2] = { Addresses::Key_isDown, 0xD67E98 };
	int(*Key_IsDown)(int localClientNum, int keynum) = (int(*)(int, int))&Key_IsDown_t;
	return Key_IsDown(0, Button);
}

/*
//SDK button monitoring, works on every game
//Doesn't work well in paint hook, may work better in separate thread
//Keeps getting detected for about a second after letting go of the button

#include <cell/pad.h>
#include <time.h> 
#include <sys\spu_thread.h>

#define LEFT (1 << 7)
#define DOWN (1 << 6)
#define RIGHT (1 << 5)
#define UP (1 << 4)
#define START (1 << 3)
#define R3 (1 << 2)
#define L3 (1 << 1)
#define SELECT (1 << 0)
#define SQUARE (1 << 15)
#define CROSS (1 << 14) 
#define CIRCLE (1 << 13)
#define TRIANGLE (1 << 12)
#define R1 (1 << 11)
#define L1 (1 << 10)
#define R2 (1 << 9)
#define L2 (1 << 8)

CellPadData PadData;

bool KeyPressed(int key) {
	cellPadGetData(0, &PadData);
	_Uint32t _key = (PadData.button[3] << 8) + PadData.button[2];
	return ((_key == key) & 1);
}

int Key() {
	cellPadGetData(0, &PadData);
	_Uint32t _key = (PadData.button[3] << 8) + PadData.button[2];
	return _key;
}
*/