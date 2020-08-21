void drawShader(float x, float y, float width, float height, float* color)
{
	R_AddCmdDrawStretchPic(x * screen.width, y * screen.height, width * screen.width, height * screen.height, 1, 1, 1, 1, color, Material_RegisterHandle("white", 0, false, 0));
}
int drawText(char text[], float x, float y, const char *font, float fontSize, float* color, float hAlign = 0.0f, float vAlign = 0.0f, float wrapWidth = 0, float wrapSpacing = 0)
{
	if (wrapWidth == 0) {
		R_AddCmdDrawText(text, 0x7FFFFFFF, R_RegisterFont(font, 0), x * screen.width - (R_TextWidth(0, text, 0x7FFFFFFF, R_RegisterFont(font, 0)) * fontSize * hAlign), y * screen.height + (screen.height * ((-vAlign + 1.25) * 0.68) * fontSize / 25), fontSize, fontSize * screen.height / 720, 0, color, 0);
		return 0;
	}
	else { //wrap text
		int prevSpace = 0;
		int lineStart = 0;
		int lines = 0;
		bool last = false;
		char wrapText[255];
		int i = 0;
		for (;; i++) {
			if (text[i] == ' ' || text[i] == '\n' || text[i] == '\0') {
				//from start of line to current space
				memcpy(wrapText, &text[lineStart], i - lineStart);
				wrapText[i - lineStart] = '\0';

				bool overflow = R_TextWidth(0, wrapText, 0x7FFFFFFF, R_RegisterFont(font, 0)) * fontSize > wrapWidth * screen.width;
				
				if (overflow || text[i] == '\n') {// || text[i] == '\n' || text[i] == '\0'
					//if single word is long enough to wrap or is \n
					if (lineStart == prevSpace || text[i] == '\n') prevSpace = i;

					//from start of line to previous space because current space is too long
					memcpy(wrapText, &text[lineStart], prevSpace - lineStart);
					wrapText[prevSpace - lineStart] = '\0';

					drawText(wrapText, x, y + wrapSpacing * lines, font, fontSize, color, hAlign, vAlign);

					//sets start of next line to last space +1 for next character
					lineStart = prevSpace + 1;
					lines++;
				}
				prevSpace = max(i, lineStart);
				//if last word is long enough to wrap, fixes blank line at the end
				if (text[i] == '\0' && prevSpace ==  lineStart) return lines - 1;
			}
			if (text[i] == '\0') break;
		}
		//draw last line
		memcpy(wrapText, &text[lineStart], max(0, i - lineStart));
		wrapText[max(0, i - lineStart)] = '\0';
		drawText(wrapText, x, y + wrapSpacing * lines, font, fontSize, color, hAlign, vAlign);

		return lines;
	}
}

void doNothing() {}

bool isInGame() {
	return *(char*)(0x1CB68C0 + 0x18);
}

//Function pointer used to call function passed into addOption
void typedef (*voidFunc)();
voidFunc menuFunction;

//Used to store the chain of submenus entered
subMenus nextSub;
subMenus subChain[100];

//Used to the get the position of scroll when closing the current submenu
int scrollChain[100];
int scrollPosChain[100];

//Used to store the addFloatOption arguments
struct _floatOption {
	float* var;
	float min, max, step;
	bool smooth;
}floatOption;

//Used to store the addArrayOption arguments
struct _arrayOption {
	char* arr[100];
	int length;
	int* index;
}arrayOption;

enum optionTypes {
	NullOption,
	Option,
	SubOption,
	BoolOption,
	TextOption,
	FloatOption,
	ArrayOption,
};

optionTypes optionType;

void openSubMenu() {
	subChain[menu.subDepth] = menu.subMenu;
	scrollChain[menu.subDepth] = menu.scroll;
	scrollPosChain[menu.subDepth] = menu.scrollPos;
	menu.subMenu = nextSub;
	menu.subDepth++;
	menu.scroll = 0;
	menu.scrollPos = 0;
}

bool addOption(char* text, subMenus parentSub, void(*f)(), char* infoText = "") {
	if (parentSub == menu.subMenu) {
		if (menu.options >= menu.scrollPos && menu.options < menu.scrollPos + menu.maxOptions) {
			if (menu.scroll == menu.options) { //current scroll
				menuFunction = f;
				if (optionType == NullOption) {
					optionType = Option;
				}
				menu.infoText = infoText != "" ? infoText : text;
				//x: menu position + option padding to bring text in a bit
				//y: menu position + header height for first option position, spacing between options * (option +1 because text gets drawn above y instead of below y like shaders) to get current option y, + an arbitrary amount that makes it line up better, should be based on font height
				drawText(text, menu.x + menu.optionPadding, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos)) + menu.optionSpacing * 0.5, menu.font, menu.optionFontSize, menu.colors.primaryContrast1, 0, 0.5);
			}
			else {
				drawText(text, menu.x + menu.optionPadding, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos)) + menu.optionSpacing * 0.5, menu.font, menu.optionFontSize, menu.colors.text, 0, 0.5);
			}
			menu.options++;
			return true;
		}
		menu.options++;
	}
	return false;
}

bool addTextOption(char* text, subMenus parentSub, void(*f)(), char* textOption, char* infoText = "") {
	if (addOption(text, parentSub, f, infoText)) {
		if (menu.scroll == menu.options - 1) { //current scroll
			if (optionType == NullOption) {
				optionType = TextOption;
			}
			drawText(textOption, menu.x + menu.width - menu.optionPadding, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos - 1)) + menu.optionSpacing * 0.5, menu.font, menu.optionFontSize, menu.colors.primaryContrast1, 1, 0.5);
		}
		else {
			drawText(textOption, menu.x + menu.width - menu.optionPadding, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos - 1)) + menu.optionSpacing * 0.5, menu.font, menu.optionFontSize, menu.colors.text, 1, 0.5);
		}
		return true;
	}
	return false;
}

void addSubMenu(char* text, subMenus parentSub, subMenus childSub, char* infoText = "") {
	if (addTextOption(text, parentSub, openSubMenu, ">", infoText)) {
		if (menu.scroll == menu.options - 1) { //current scroll
			nextSub = childSub;
			optionType = SubOption;
		}
	}
}

void addBoolOption(char* text, subMenus parentSub, void(*f)(), bool optionBool, char* infoText = "") {
	if (addOption(text, parentSub, f, infoText)) {
		if (menu.scroll == menu.options - 1) { //current scroll
			optionType = BoolOption;
		}
		float shaderSize = 0.0109375;
		float shaderSizeInner = 0.0078;
		//x: current menu position + width to be right of menu, - shader size to align the right of the shader with the right side of the menu, - padding to move inwards a little
		//y: menu position + header height for first option position, spacing between options * option to get current option y, + half of the difference between option spacing and the shader size to vertically center on the option
		//height: *16/9 to account for screen being more wide than tall 
		drawShader(menu.x + menu.width - shaderSize - menu.optionPadding, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - menu.scrollPos - 1) + (menu.optionSpacing / 2 - shaderSize), shaderSize, shaderSize * 16 / 9, Black);
		if (optionBool) {
			drawShader(menu.x + menu.width - shaderSizeInner - menu.optionPadding - (shaderSize - shaderSizeInner) / 2, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - 1) + (menu.optionSpacing / 2 - shaderSizeInner), shaderSizeInner, shaderSizeInner * 16 / 9, menu.colors.primary);
		}
	}
}


/// <summary>
/// 
/// </summary>
/// <param name="smooth">Remove the delay between presses, its probably best to have the step value small as it will be called 60 times per second</param>
/// <param name="f">Function called when the value is changed</param>
/// <remarks></remarks>
void addFloatOption(char* text, subMenus parentSub, float &var, float min = 0, float max = 1, float step = 0.1, int decimals = 1, bool smooth = true, void(*f)() = doNothing, char* infoText = "") {
	if (addOption(text, parentSub, f, infoText)) {
		char buffer[64];
		ftoa(var, buffer, decimals);
		float sliderWidth = 0.03;
		float sliderHeight = 0.02;
		if (menu.scroll == menu.options - 1) { //current scroll
			optionType = FloatOption;
			/* idk why this doesn't work
			char infoBuffer[255];
			sprintf(infoBuffer, "%s: %s", infoText != "" ? infoText : text, buffer);
			menu.infoText = infoBuffer; */
			floatOption.var = &var;
			floatOption.min = min;
			floatOption.max = max;
			floatOption.step = step;
			floatOption.smooth = smooth;
			drawShader(menu.x + menu.width - sliderWidth - menu.optionPadding, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - menu.scrollPos - 1) + (menu.optionSpacing / 2 - sliderHeight / 16 * 9), sliderWidth, sliderHeight, LightBlack3);
			drawShader(menu.x + menu.width - sliderWidth - menu.optionPadding, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - menu.scrollPos - 1) + (menu.optionSpacing / 2 - sliderHeight / 16 * 9), (var - min) / (max - min) * sliderWidth , sliderHeight, Black);
			drawText(buffer, menu.x + menu.width - menu.optionPadding - sliderWidth / 2, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos - 1)) + menu.optionSpacing * 0.5, menu.font, 0.5, White, 0.5, 0.5);
		}
		else {
			drawShader(menu.x + menu.width - sliderWidth - menu.optionPadding, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - menu.scrollPos - 1) + (menu.optionSpacing / 2 - sliderHeight / 16 * 9), sliderWidth, sliderHeight, Black);
			drawShader(menu.x + menu.width - sliderWidth - menu.optionPadding, menu.y + menu.headerHeight + menu.optionSpacing * (menu.options - menu.scrollPos - 1) + (menu.optionSpacing / 2 - sliderHeight / 16 * 9), (var - min) / (max - min) * sliderWidth , sliderHeight, Grey);
			drawText(buffer, menu.x + menu.width - menu.optionPadding - sliderWidth / 2, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.options - menu.scrollPos - 1)) + menu.optionSpacing * 0.5, menu.font, 0.5, White, 0.5, 0.5);

		}
	}
}

char* arropt[100];
void addArrayOption(char* text, subMenus parentSub, char* arr[], int arrLength, int &indexVar, void(*f)() = doNothing, char* infoText = "") {
	if (addTextOption(text, parentSub, f, arr[indexVar], infoText)) {
		if (menu.scroll == menu.options - 1) { //current scroll
			optionType = ArrayOption;
			for (unsigned int i = 0; i < arrLength; i++) {
				arrayOption.arr[i] = arr[i];
			}
			arrayOption.length = arrLength;
			arrayOption.index = &indexVar;
		}
	}
}

void open() {
	menu.open = true;
	if(isInGame()) menu.subMenu = Main;
	else menu.subMenu = Pre;
	menu.scroll = 0;
	menu.scrollPos = 0;
	menu.framesPassed = 0;
	menu.subDepth = 0;
}

void close() {
	menu.open = false;
	menu.subMenu = Closed;
	menu.framesPassed = 0;
	menu.subDepth = 0;
}

void select() {
	menuFunction();
	menu.framesPassed = 0;
	optionType = NullOption;
}

void back() {
	if (menu.subDepth > 0) {
		menu.subMenu = subChain[menu.subDepth - 1];
		menu.scroll = scrollChain[menu.subDepth - 1];
		menu.scrollPos = scrollPosChain[menu.subDepth - 1];
		menu.subDepth--;
	}
	else {
		menu.open = false;
	}
	menu.framesPassed = 0;
	optionType = NullOption;
}

void scrollUp() {
	menu.scroll--;
	if (menu.scroll < menu.scrollPos) {
		menu.scrollPos--;
	}
	if (menu.scroll < 0) {
		menu.scroll = menu.options - 1;
		menu.scrollPos = menu.options - min(menu.maxOptions, menu.options);
	}
	menu.framesPassed = 0;
	optionType = NullOption;
}

void scrollDown() {
	menu.scroll++;
	if (menu.scroll >= menu.scrollPos + menu.maxOptions) {
		menu.scrollPos++;
	}
	if (menu.scroll >= menu.options) {
		menu.scroll = 0;
		menu.scrollPos = 0;
	}
	menu.framesPassed = 0;
	optionType = NullOption;
}

void floatOptionAdd() {
	if (*floatOption.var < floatOption.max) {
		*floatOption.var += floatOption.step;
	}
	else {
		*floatOption.var = floatOption.max;
	}
	if (!floatOption.smooth) { menu.framesPassed = 0; }
	menuFunction();
}

void floatOptionMinus() {
	if (*floatOption.var > floatOption.min) {
		*floatOption.var-= floatOption.step;
	}
	else {
		*floatOption.var = floatOption.min;
	}
	if (!floatOption.smooth) { menu.framesPassed = 0; }
	menuFunction();
}

void arrayOptionNext() {
	char buffer[100];
	sprintf(buffer, "Index: %i\nLength: %i\n", *arrayOption.index, arrayOption.length);
	console_write(buffer);
	if (*arrayOption.index >= arrayOption.length - 1) {
		console_write("if");
		*arrayOption.index = 0;
	}
	else {
		console_write("else");
		*arrayOption.index = *arrayOption.index + 1;
	}
	menu.framesPassed = 0;
	menuFunction();
}

void arrayOptionPrev() {
	console_write("arrayOptionPrev\n");
	char buffer[100];
	sprintf(buffer, "Index: %i\n", *arrayOption.index);
	console_write(buffer);
	if (*arrayOption.index > 0) {
		console_write("if");
		*arrayOption.index = *arrayOption.index - 1;
	}
	else {
		console_write("*else");
		*arrayOption.index = arrayOption.length - 1;
	}
	menu.framesPassed = 0;
	menuFunction();
}

void joiningLeavingGame() {
	close();

	//change variables such as x, y, width colors etc here to be different for pre game and non host
	if (isInGame()) {
		menu.x = 0.725;
		menu.y = 0.075;
		menu.width = 0.2;
	}
	else {
		menu.x = 0.025;
		menu.y = 0.05;
		menu.width = 0.135;
	}
}