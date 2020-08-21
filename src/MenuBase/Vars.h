bool keyboardOpened = false;
void typedef (*voidFuncChar)(char*);
voidFuncChar keyboardFunction;


enum subMenus {
	Closed,

	Main,
	MainSub1,
	MainSub2,
	MainSub1Sub1,

	Pre,
	PreSub1,
	PreSub2,
	PreSub1Sub1,

	Name,
	Designer,
};

struct _menu{
	int scroll, options, prevOptions, subDepth, framesPassed, scrollPos;
	bool open;
	subMenus subMenu;
	char* infoText;
	int infoLines;

	int maxOptions;
	float x, y, width, scale;
	float headerHeight;
	float footerHeight;
	float optionPadding, optionSpacing, optionFontSize;
	char *font;
	struct _ncolors {
		float* bg;
		float* text;
		float* primary;
		float* primaryContrast1;
		float* primaryContrast2;
	}colors;
}menu;

struct _screen{
	int width, height;
}screen;

struct _game {
	bool inGame;
}game;

struct _options {
	bool grid;
	float fov;
	int testIndex;
}options;
char * testArr[10] = { "A", "B", "C", "D" };

float Red[4] = { 1, 0, 0, 1 };
float LightRed[4] = { 1, 0, 0, 0.6f };
float Green[4] = { 0, 1, 0, 1 };
float LightGreen[4] = { 0, 1, 0, 0.2f };
float Blue[4] = { 0, 0, 1, 0.4 };
float LightBlue[4] = { 0, 0, 1, 0.2f };
float Yellow[4] = { 1, 1, 0, 1 };
float LightYellow[4] = { 1, 1, 0, 0.2f };
float Purple[4] = { 0.4, 0, 0.7, 1 };
float LightPurple[4] = { 0.4, 0, 0.7, 0.2f };
float Pink[4] = { 0.9, 0, 0.9, 1 };
float LightPink[4] = { 0.9, 0, 0.9, 0.2f };
float LightPinkfull[4] = { 0.6, 0, 0.6, 1.0f };
float Orange[4] = { 1, 0.4, 0, 1 };
float LightOrange[4] = { 1, 0.4, 0, 0.6f };
float Cyan[4] = { 0, 1, 0.9, 1 };
float White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightWhite[4] = { 1.0f, 1.0f, 1.0f, 0.2f };
float Black[4] = { 0, 0, 0, 1.0f };
float LightBlack[4] = { 0, 0, 0, 0.6f };
float Mint[4] = { 0.04, 0.82, 0.43, 1 };
float Grey[4] = { 0.5, 0.5, 0.5, 1 };
float mainBlue[4] = { 0, 0.67f, 0.54f, 0.7f };
float mainBlueESP[4] = { 0, 0.67f, 0.54f, 0.2f };
float WhiteESP[4] = { 1.0f, 1.0f, 1.0f, 0.2f };
float mainBlueAlpha[4] = { 0, 0.67f, 0.54f, 1 };
float LightGrey[4] = { 0.20, 0.20, 0.20, 0.95 };
float LightBlack3[4] = { 0, 0, 0, 0.3f };
float LightBlack5[4] = { 0, 0, 0, 0.5f };
float LightBlack8[4] = { 0, 0, 0, 0.8f };
float FullWhite[4] = { 1.0, 1.0f, 1.0f, 1 };

void setVars() {
	//screen width gets dynamically set later
	//Width - 1080p: 1280, 720p: 1280, 576p: 720;
	//Height - 1080p: 1080, 720p : 720, 576p : 576;

	menu.scroll = 0;
	menu.prevOptions = 0;
	menu.scrollPos = 0;
	menu.infoText = "";
	menu.infoLines = 0;

	menu.maxOptions = 18;

	menu.x = 0.4;
	menu.y = 0.4;
	menu.width = 0.2;
	menu.headerHeight = 0.05;
	menu.footerHeight= 0.04;

	menu.optionSpacing = 0.04;
	menu.optionPadding = 0.01;
	menu.optionFontSize = 0.7;

	menu.subDepth = 0;
	menu.framesPassed = 0;

	menu.colors.bg = LightBlack;
	menu.colors.text = White;
	menu.colors.primary = White;
	menu.colors.primaryContrast1 = Black;
	menu.colors.primaryContrast2 = Grey;

	menu.font = FONT_NORMAL;

	game.inGame = true;

	options.grid = false;
	options.fov = 65;

	options.testIndex = 0;
}
