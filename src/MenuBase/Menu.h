void doMenu() {
	//change variables such as x, y, width colors etc here to be different for pre game and non host
	if(isInGame()){
		menu.x = 0.4;
		menu.y = 0.4;
	}
	else{
		menu.x = 0.2;
		menu.y = 0.1;
	}

	//Background
	drawShader(menu.x, menu.y, menu.width, menu.height + (menu.optionSpacing * menu.prevOptions), menu.colors.bg);

	//Title
	drawText("Menu Title", menu.x + menu.optionPadding, menu.y + menu.height, menu.font, 1, menu.colors.text);

	//Scrollbar
	drawShader(menu.x, menu.y + menu.height  + (menu.optionSpacing * menu.scroll), menu.width, menu.optionSpacing, menu.colors.scrollBar);

	addSubMenu("Sub 1", Main, MainSub1);
	addSubMenu("Sub 2", Main, MainSub2);

	addSubMenu("Sub 1 Sub 1", MainSub1, MainSub1Sub1);
	addOption("Sub 1 Sub 1 Option 1", MainSub1Sub1, doNothing);

	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());
	addBoolOption("Red Boxes", MainSub2, toggleRedBoxes, redBoxesBool());

	addOption("Pre Option 1", Pre, doNothing);
}