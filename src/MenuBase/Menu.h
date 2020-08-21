void doMenu() {
	int opts = min(menu.maxOptions, menu.prevOptions);

	//Background
	drawShader(menu.x, menu.y + menu.headerHeight, menu.width, (menu.optionSpacing * opts), menu.colors.bg);

	//Header
	drawShader(menu.x, menu.y, menu.width, menu.headerHeight, LightBlack8);
	//Header Title
	drawText("Menu Title", menu.x + (menu.width / 2), menu.y + menu.headerHeight / 2, menu.font, 1, menu.colors.text, 0.5, 0.5);

	//Footer
	float lineHeight = 0.02;
	drawShader(menu.x, menu.y + menu.headerHeight + (menu.optionSpacing * opts), menu.width, menu.footerHeight + menu.infoLines * lineHeight, LightBlack8);
	//Footer Text
	menu.infoLines = drawText(menu.infoText, menu.x + menu.optionPadding, menu.y + menu.headerHeight + (menu.optionSpacing * opts) + menu.footerHeight / 2, menu.font, 0.5, menu.colors.text, 0, 0.5, menu.width - menu.optionPadding * 2, lineHeight);

	//Scrollbar
	drawShader(menu.x, menu.y + menu.headerHeight + (menu.optionSpacing * (menu.scroll - menu.scrollPos)), menu.width, menu.optionSpacing, menu.colors.primary);
	//Side scrollbar
	if (menu.prevOptions > menu.maxOptions) {
		float vScrollThickness = 0.002;
		drawShader(menu.x + menu.width - vScrollThickness, menu.y + menu.headerHeight, vScrollThickness, menu.optionSpacing * opts, Black);
		drawShader(menu.x + menu.width - vScrollThickness, menu.y + menu.headerHeight + (menu.optionSpacing * opts * menu.scrollPos / menu.prevOptions), vScrollThickness, menu.optionSpacing * opts * opts / menu.prevOptions, menu.colors.primary);
	}
		
	addSubMenu("Sub 1", Main, MainSub1, "This is a\ntest sub text\n123");
	addSubMenu("Sub 2", Main, MainSub2);
	addSubMenu("Name", Main, Name);
	addSubMenu("Designer", Main, Designer);

	 
	addSubMenu("Sub 1 Sub 1", MainSub1, MainSub1Sub1);
	addOption("Sub 1 Sub 1 Option 1", MainSub1Sub1, doNothing, "This option is just a test and does not actually do anything");

	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());
	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());
	addFloatOption("FOV", MainSub2, options.fov, 10, 170, 0.5, 0, "Field of view");
	addArrayOption("Test", MainSub2, testArr, 4, options.testIndex);

	addOption("Pre Option 1", Pre, doNothing);
	addSubMenu("Name", Pre, Name, "Change name options");
	addSubMenu("Designer", Pre, Designer);

	addOption("Custom", Name, openNameKeyboard, "Open a keyboard to type a custom name");


	addBoolOption("Toggle Grid", Designer, toggleGrid, options.grid);
	addFloatOption("X", Designer, menu.x, 0, 1, 0.002, 3);
	addFloatOption("Y", Designer, menu.y, 0, 1, 0.003, 3);
	addFloatOption("Width", Designer, menu.width, 0, 1, 0.001, 3);
	addFloatOption("Option Spacing", Designer, menu.optionSpacing, 0, 1, 0.0005, 3);
	addFloatOption("Header Height", Designer, menu.headerHeight, 0, 1, 0.001, 3);
	addFloatOption("Footer Height", Designer, menu.footerHeight, 0, 1, 0.001, 3);
	addFloatOption("Font Size", Designer, menu.optionFontSize, 0, 2, 0.01, 2);
}