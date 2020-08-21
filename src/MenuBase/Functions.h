void constRun() {

	if (options.grid) {
		for (float x = 0; x < 1; x+= 0.1) {
			drawShader(x, 0, 0.001, 1, White);
		}
		for (float y = 0; y < 1; y += 0.1) {
			drawShader(0, y, 1, 0.001, White);
		}
		
		for (float x = 0; x < 1; x += 0.025) {
			drawShader(x, 0, 0.001, 1, LightWhite);
		}
		for (float y = 0; y < 1; y += 0.025) {
			drawShader(0, y, 1, 0.001, LightWhite);
		}
	}

	if (options.fov != 65) {
		*(float*)0x1CC5218 = options.fov;
	}
}

//gets value from memory rather than a variable so it is always updated, even after changing in an rtm
bool redBoxesBool() {;
	return *(char*)0x783E3 == 0x01;
}

void toggleRedBoxes() {
	if (redBoxesBool()) {
		unsigned char bytes[4] = { 0x38, 0x60, 0x00, 0x00 };
		write_process(0x783E0, &bytes, 4);
	}
	else {
		unsigned char bytes[4] = { 0x38, 0x60, 0x00, 0x01 };
		write_process(0x783E0, &bytes, 4);
	}
}

//Designer
void toggleGrid() { options.grid = !options.grid; }

void openKeyboard(void(*f)(char*)) {
	keyboardOpened = true;
	keyboardFunction = f;
}
void setName(char* name) {
	strcpy((char*)0x026C067F, name);
	strcpy((char*)0x026C0658, name);
}
void openNameKeyboard() {
	openKeyboard(setName);
}
