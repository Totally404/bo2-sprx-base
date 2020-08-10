#define TOC (0xD67E98)

#define FONT_SMALL_DEV    "fonts/720/smallDevFont"
#define FONT_BIG_DEV    "fonts/720/bigDevFont"
#define FONT_CONSOLE    "fonts/720/consoleFont"
#define FONT_BIG        "fonts/720/bigFont"
#define FONT_SMALL        "fonts/720/smallFont"		//these all work now
#define FONT_BOLD        "fonts/720/boldFont"
#define FONT_NORMAL        "fonts/720/normalFont"
#define FONT_EXTRA_BIG    "fonts/720/extraBigFont"
#define FONT_OBJECTIVE    "fonts/720/objectiveFont" 

int R_RegisterFont_a[2] = { Addresses::R_RegisterFont, TOC };
int R_AddCmdDrawTextInternal_a[2] = { Addresses::R_AddCmdDrawText, TOC };
int Material_RegisterHandle_a[2] = { Addresses::Material_RegisterHandle, TOC };
int R_AddCmdDrawStretchPicInternal_a[2] = { Addresses::R_AddCmdDrawStretchPic, TOC };
int R_TextWidth_t[2] = { Addresses::R_TextWidth, TOC };

void*(*R_RegisterFont)(const char * name, int imageTrack) = (void*(*)(const char*, int))&R_RegisterFont_a;
void(*R_AddCmdDrawText)(const char* text, int maxChars, void* font, float x, float y, float yScale, float xScale, float rotation, float* colors, int Style) = (void(*)(const char*, int, void*, float, float, float, float, float, float*, int))&R_AddCmdDrawTextInternal_a;
void*(*Material_RegisterHandle)(const char *name, int imageTrack, bool errorIfMissing, int waitTime) = (void*(*)(const char*, int, bool, int))&Material_RegisterHandle_a;
void(*R_AddCmdDrawStretchPic)(float x, float y, float w, float h, float s0, float t0, float s1, float t1, float* color, void *material) = (void(*)(float, float, float, float, float, float, float, float, float*, void*))&R_AddCmdDrawStretchPicInternal_a;
int(*R_TextWidth)(int localClientNum, const char *text, int maxChars, void *font) = (int(*)(int localClientNum, const char *text, int maxChars, void *font))&R_TextWidth_t;


int Cbuff_AddText_t[2] = { 0x313C18, TOC };
int Dvar_GetBool_a[2] = { 0x3DA948, TOC };
int Dvar_FindMalleableVar_a[2] = { 0x3DA628, TOC };


/*void(*cBuf_Addtext)(int localClientNum, char* text) = (void(*)(int, char*))&Cbuff_AddText_t;
bool(*Dvar_GetBool_f)(dvar_t* LocalClient) = (bool(*)(dvar_t*))&Dvar_GetBool_a;
dvar_t*(*Dvar_FindMalleableVar)(const char* LocalClient) = (dvar_t*(*)(const char*))&Dvar_FindMalleableVar_a;
bool getBool(const char *dvarName)
{
	if (Dvar_GetBool_f(Dvar_FindMalleableVar(dvarName)))
		return true;
	else
		return false;
}*/

void Menu_PaintAll_Stub(int a, int b) {
	__asm("li %r3, 0x3");
}
