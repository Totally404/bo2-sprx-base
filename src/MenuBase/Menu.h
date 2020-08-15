void doMenu() {
	int opts = min(menu.maxOptions, menu.prevOptions);

	//Background
	drawShader(menu.x, menu.y, menu.width, menu.height + (menu.optionSpacing * opts), menu.colors.bg);

	//Title
	drawText("Menu Title", menu.x + (menu.width / 2), menu.y + menu.height / 2, menu.font, 1, menu.colors.text, 0.5, 0.5);

	//Scrollbar
	drawShader(menu.x, menu.y + menu.height + (menu.optionSpacing * (menu.scroll - menu.scrollPos)), menu.width, menu.optionSpacing, menu.colors.primary);

	//Side scrollbar
	if (menu.prevOptions > menu.maxOptions) {
		float vScrollThickness = 0.002;
		drawShader(menu.x + menu.width - vScrollThickness, menu.y + menu.height, vScrollThickness, menu.optionSpacing * opts, Black);
		drawShader(menu.x + menu.width - vScrollThickness, menu.y + menu.height + (menu.optionSpacing * opts * menu.scrollPos / menu.prevOptions), vScrollThickness, menu.optionSpacing * opts * opts / menu.prevOptions, menu.colors.primary);
	}
		
	addSubMenu("Sub 1", Main, MainSub1);
	addSubMenu("Sub 2", Main, MainSub2);
	addSubMenu("Name", Main, Name);
	addSubMenu("Designer", Main, Designer);


	addSubMenu("Sub 1 Sub 1", MainSub1, MainSub1Sub1);
	addOption("Sub 1 Sub 1 Option 1", MainSub1Sub1, doNothing);

	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());
	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());
	addFloatOption("FOV", MainSub2, options.fov, 10, 170, 0.5, 0);
	addArrayOption("Test", MainSub2, testArr, 4, options.testIndex);

	addOption("Pre Option 1", Pre, doNothing);
	addSubMenu("Name", Pre, Name);
	addSubMenu("Designer", Pre, Designer);

	addOption("Custom", Name, openNameKeyboard);


	addBoolOption("Toggle Grid", Designer, toggleGrid, options.grid);
	addFloatOption("Menu X", Designer, menu.x, 0, 1, 0.002, 3);
	addFloatOption("Menu Y", Designer, menu.y, 0, 1, 0.003, 3);
	addFloatOption("Menu Width", Designer, menu.width, 0, 1, 0.001, 3);
	addFloatOption("Menu Height", Designer, menu.height, 0, 1, 0.001, 3);
	addFloatOption("Menu Option Spacing", Designer, menu.optionSpacing, 0, 1, 0.0005, 3);
	addFloatOption("Font Size", Designer, menu.optionFontSize, 0, 2, 0.01, 2);
}