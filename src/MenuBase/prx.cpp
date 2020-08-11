#include "PS3.h"
#include "Addresses.h"
#include "TOC.h"
#include "Vars.h"
#include "HudsFunc.h"
#include "Buttons.h"
#include "Functions.h"
#include "Menu.h"

SYS_MODULE_INFO( MenuBase, 0, 1, 1);
SYS_MODULE_START( _MenuBase_prx_entry );

SYS_LIB_DECLARE_WITH_STUB( LIBNAME, SYS_LIB_AUTO_EXPORT, STUBNAME );
SYS_LIB_EXPORT( _MenuBase_export_function, LIBNAME );

void paintHook(int x, int xx) {
	Menu_PaintAll_Stub(x, xx);

	screen.width = *(short*)(0xD933D4);//Width
	screen.height = *(short*)(0xD933D6);//Height

	if (game.inGame != isInGame()) joiningLeavingGame();
	game.inGame = isInGame();

	constRun();

	if (menu.open) {
		doMenu();
	}

	if (menu.framesPassed > 10) {
		if (menu.open) {
			if (detect(UP)) {
				scrollUp();
			}
			else if (detect(DOWN)) {
				scrollDown();
			}
			else if (isInGame() && detect(LEFT) || !isInGame() && detect(L2)) {
				if (optionType == FloatOption) {
					floatOptionMinus();
				}
				else if (optionType == ArrayOption) {
					arrayOptionPrev();
				}
				else {
					back();
				}
			}
			else if (isInGame() && detect(RIGHT) || !isInGame() && detect(R2)) {
				if (optionType == FloatOption) {
					floatOptionAdd();
				}
				else if (optionType == ArrayOption) {
					arrayOptionNext();
				}
				else {
					select();
				}
			}
			if (detect(R3)) {
				close();
			}
		}
		else {
			if (isInGame() && detect(LEFT) || !isInGame() && detect(R2)) {
				open();
			}
		}
	}
	menu.framesPassed++;
	menu.prevOptions = menu.options;
	menu.options = 0;
}

// An exported function is needed to generate the project's PRX stub export library
extern "C" int _MenuBase_export_function(void)
{
    return CELL_OK;
}

sys_ppu_thread_t MainThread;

void Thread(uint64_t nothing) {
	
	sleep(10000);
	console_write("Loaded");
	setVars();
	hookFunctionStart((int)0x3971A0, (int)Menu_PaintAll_Stub, (int)paintHook);
	while (true) {
		if (keyboardOpened) {
			char output[1024];
			Keyboard::oskdialog_mode = Keyboard::MODE_OPEN;
			while (Keyboard::oskdialog_mode != Keyboard::MODE_EXIT)
			{
				Keyboard::keyboard(output, "", "Insert Text");
			}
			keyboardFunction(output);
			keyboardOpened = false;
		}
		sleep(0.05);
	}
	sys_ppu_thread_exit(nothing);
}

extern "C" int _MenuBase_prx_entry(void)
{
	sys_ppu_thread_create(&MainThread, Thread, 0, 0x4AA, 0x7000, 0, "Main Thread");
    return SYS_PRX_RESIDENT;
}
