void drawShader(float x, float y, float width, float height, float* color)
{
	R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 1, 1, 1, 1, color, Material_RegisterHandle("white", 0, false, 0));
}
void drawText(const char *xtext, float x, float y, const char *xfont, float xfontSize, float* color)
{
	R_AddCmdDrawText(xtext, 0x7FFFFFFF, R_RegisterFont(xfont, 0), x * screen.width, y * screen.height, xfontSize, xfontSize * screen.height / 720, 0, color, 0);
}
void drawOnShader(float x, float y, float width, float height, float* color)
{
	R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 0, 0, 1, 1, color, Material_RegisterHandle("white", 0, 0, 0)); //check mark doesnt work in zombies :/
}
void drawOffShader(float x, float y, float width, float height, float* color)
{
	R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 0, 0, 0, 0, color, Material_RegisterHandle("white", 0, false, 0));
}
/*void drawNumberedShader(int shaderID, float x, float y, float width, float height, float* color)
{
	if(var.scroll == shaderID){ //current scroll
		R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 1, 1, 1, 1, scrollColor, Material_RegisterHandle("white", 0, false, 0));
	}
	else R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 1, 1, 1, 1, color, Material_RegisterHandle("white", 0, false, 0));
}
void drawNumberedText(int textID, const char *xtext, float xx, float xy, const char *xfont, float xfontSize, float* color)
{
	if(var.lrScroll == textID){
		R_AddCmdDrawText(xtext, 0x7FFFFFFF, R_RegisterFont(xfont, 0), xx, xy, xfontSize, xfontSize * screen.height / 720, 0, Black, 0);
	}
	else R_AddCmdDrawText(xtext, 0x7FFFFFFF, R_RegisterFont(xfont, 0), xx, xy, xfontSize, xfontSize * screen.height / 720, 0, color, 0);
}
*/

bool isInGame() {
	return *(char*)(0x1CB68C0 + 0x18);
}

//Function pointer array used to call function passed into addOption
void typedef (*voidFunc)();
voidFunc menuFunctions[100];

//Used to store the chain of submenus entered
subMenus nextSubs[100];
subMenus subChain[100];

//Used to the get the position of scroll when closing the current submenu
int scrollChain[100];

void openSubMenu() {
	subChain[menu.subDepth] = menu.subMenu;
	scrollChain[menu.subDepth] = menu.scroll;
	menu.subMenu = nextSubs[menu.scroll];
	menu.subDepth++;
	menu.scroll = 0;
}

void addOption(char* text, subMenus parentSub, void(*f)()) {
	if (parentSub == menu.subMenu) {
		menuFunctions[menu.options] = f;
		if (menu.scroll == menu.options) { //current scroll
			//x: menu position + option padding to bring text in a bit
			//y: menu position + header height for first option position, spacing between options * (option +1 because text gets drawn above y instead of below y like shaders) to get current option y, + an arbitrary amount that makes it line up better, should be based on font height
			drawText(text, menu.x + menu.optionPadding, menu.y + menu.height + (menu.optionSpacing * (menu.options + 1)) - 0.006, menu.font, 0.7, menu.colors.scrollText);
		}
		else {
			drawText(text, menu.x + menu.optionPadding, menu.y + menu.height + (menu.optionSpacing * (menu.options + 1)) - 0.006, menu.font, 0.7, menu.colors.text);
		}
		menu.options++;
	}
}

void addSubMenu(char* text, subMenus parentSub, subMenus childSub) {
	if (parentSub == menu.subMenu) {
		nextSubs[menu.options] = childSub;
		addOption(text, parentSub, openSubMenu);
	}
}

void addBoolOption(char* text, subMenus parentSub, void(*f)(), bool optionBool) {
	if (parentSub == menu.subMenu) {
		float shaderSize = 0.008;
		if (optionBool) {
			//x: current menu position + width to be right of menu, - shader size to align the right of the shader with the right side of the menu, - padding to move inwards a little
			//y: menu position + header height for first option position, spacing between options * option to get current option y, + half of the difference between option spacing and the shader size to vertically center on the option
			//height: *16/9 to account for screen being more wide than tall 
			//+ (menu.optionSpacing - shaderSize) / 2
			drawShader(menu.x + menu.width - shaderSize - menu.optionPadding, menu.y + menu.height + menu.optionSpacing * menu.options + (menu.optionSpacing/2 - shaderSize), shaderSize, shaderSize * 16 / 9, Green);
		}
		else {
			drawShader(menu.x + menu.width - shaderSize - menu.optionPadding, menu.y + menu.height + menu.optionSpacing * menu.options + (menu.optionSpacing/2 - shaderSize), shaderSize, shaderSize * 16 / 9, Red);
		}
		addOption(text, parentSub, f);
	}
}

void open() {
	menu.open = true;
	if(isInGame()) menu.subMenu = Main;
	else menu.subMenu = Pre;
	menu.scroll = 0;
	menu.framesPassed = 0;
}

void close() {
	menu.open = false;
	menu.subMenu = Closed;
	menu.framesPassed = 0;
}

void select() {
	menuFunctions[menu.scroll]();
	menu.framesPassed = 0;
}

void back() {
	if (menu.subDepth > 0) {
		menu.subMenu = subChain[menu.subDepth - 1];
		menu.scroll = scrollChain[menu.subDepth - 1];
		menu.subDepth--;
	}
	else {
		menu.open = false;
	}
	menu.framesPassed = 0;
}

void scrollUp() {
	menu.scroll--;
	if (menu.scroll < 0) {
		menu.scroll = menu.options - 1;
	}
	menu.framesPassed = 0;
}

void scrollDown() {
	menu.scroll++;
	if (menu.scroll >= menu.options) {
		menu.scroll = 0;
	}
	menu.framesPassed = 0;
}
