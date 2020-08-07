enum subMenus {
	Closed,

	Main,
	MainSub1,
	MainSub2,
	MainSub1Sub1,

	Pre,
	PreSub1,
	PreSub2,
	PreSub1Sub1
};

struct _menu{
	int scroll, options, prevOptions, subDepth, framesPassed;
	bool open;
	float x, y, width, height, scale;
	float optionPadding, optionSpacing, optionFontSize;
	float* bgColor, accentColor;
	subMenus subMenu;
	struct _ncolors {
		float* bg;
		float* text;
		float* scrollBar;
		float* scrollText;
	}colors;
	char *font;
}menu;

struct _screen{
	int width, height;
}screen;

struct _game {
	bool inGame;
}game;

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
float Black[4] = { 0, 0, 0, 1.0f };
float LightBlack[4] = { 0, 0, 0, 0.6f };
float Mint[4] = { 0.04, 0.82, 0.43, 1 };
float Grey[4] = { 0.32, 0.33, 0.34, 1 };
float mainBlue[4] = { 0, 0.67f, 0.54f, 0.7f };
float mainBlueESP[4] = { 0, 0.67f, 0.54f, 0.2f };
float WhiteESP[4] = { 1.0f, 1.0f, 1.0f, 0.2f };
float mainBlueAlpha[4] = { 0, 0.67f, 0.54f, 1 };
float LightGrey[4] = { 0.20, 0.20, 0.20, 0.95 };
float LightBlack3[4] = { 0, 0, 0, 0.3f };
float LightBlack5[4] = { 0, 0, 0, 0.5f };
float FullWhite[4] = { 1.0, 1.0f, 1.0f, 1 };

void setVars() {
	//screen width gets dynamically set later
	//Width - 1080p: 1280, 720p: 1280, 576p: 720;
	//Height - 1080p: 1080, 720p : 720, 576p : 576;

	menu.scroll = 0;
	menu.prevOptions = 0;

	menu.x = 0.4;
	menu.y = 0.4;
	menu.width = 0.2;
	menu.height = 0.05;

	menu.optionSpacing = 0.04;
	menu.optionPadding = 0.01;

	menu.subDepth = 0;
	menu.framesPassed = 0;

	menu.colors.bg = LightBlack;
	menu.colors.text = White;
	menu.colors.scrollBar = White;
	menu.colors.scrollText = Black;

	menu.font = FONT_NORMAL;
}