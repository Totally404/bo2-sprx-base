void doNothing(){}

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

