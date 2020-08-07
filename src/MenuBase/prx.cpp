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

	char buffer[1000];
	sprintf(buffer, "Width: %i\nHeight: %i\n\nShort width: %i\nShort height: %i\n\n\n", screen.width, screen.height, *(short*)0xD933D4, *(short*)0xD933D6);
	console_write(buffer);

	if (menu.open) {
		doMenu();
	}

	if (menu.framesPassed > 10) {
		if (menu.open) {
			if (detect(UP)) {
				scrollUp();
			}
			if (detect(DOWN)) {
				scrollDown();
			}
			if (isInGame() && detect(LEFT) || !isInGame() && detect(L2)) {
				back();
			}
			if (isInGame() && detect(RIGHT) || !isInGame() && detect(R2)) {
				select();
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

extern "C" int _MenuBase_prx_entry(void)
{
	setVars();
	hookFunctionStart((int)0x3971A0, (int)Menu_PaintAll_Stub, (int)paintHook);
    return SYS_PRX_RESIDENT;
}
