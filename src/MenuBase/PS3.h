#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/prx.h>
#include <sys/syscall.h>
#include <sys/ppu_thread.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/types.h>
#include <math.h>
#include <fastmath.h>
#include <cellstatus.h>
#include <sys/timer.h>
#include <cell/sysmodule.h>
#include <sys/random_number.h>
#include <ppu_intrinsics.h>
#include <spu_printf.h>
#include <ctype.h>


struct opd_s
{
	uint32_t sub;
	uint32_t toc;
};

int sys_ppu_thread_exit()
{
	system_call_1(41, 0);
	return_to_user_prog(int);
}

int cstrcmp(const char* s1, const char* s2)//Please note if its equal to zero the strings are equal
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int console_write(const char * s)
{
	uint32_t len;
	system_call_4(403, 0, (uint64_t) s, std::strlen(s), (uint64_t) &len);
	return_to_user_prog(int);
}

int32_t write_process(uint64_t ea, const void * data, uint32_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), ea, size, (uint64_t)data);
	return_to_user_prog(int32_t);
}

sys_ppu_thread_t create_thread(void (*entry)(uint64_t), int priority, size_t stacksize, const char* threadname, sys_ppu_thread_t tid)
{	
	if(sys_ppu_thread_create(&tid, entry, 0, priority , stacksize, 0, threadname) != CELL_OK)
	{
		console_write("Thread creation failed\n");	
	}
	else
	{
		console_write("Thread created\n");	
	}	
    return tid;
}

void sleep(usecond_t time)  //1 second = 1000
{
	sys_timer_usleep(time * 1000);
}

namespace PS3
{
	float floatArray[100];
    float* ReadFloat(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            floatArray[i] =  *(float*)(address + (i * 0x04));
        }
        return floatArray;
    }

	char byteArray[100];
	char* ReadBytes(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            byteArray[i] =  *(char*)(address + (i));
        }
        return byteArray;
    }

	void WriteFloat(int address, float* input, int length)
	{
		for (int i = 0; i < length; i++)
		{
			*(float*)(address + (i * 4)) = input[i];
		}
	}

	void WriteBytes(int address, char* input, int length)
    {
        for (int i = 0; i < length; i++)
        {
            *(char*)(address + (i)) = input[i];
        }
    }

	float intArray[100];
    float* ReadInt(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            intArray[i] =  *(int*)(address + (i * 0x04));
        }
        return intArray;
    }

	void WriteInt(int address, int* input, int length)
    {
        for (int i = 0; i < length; i++)
        {
            *(int*)(intArray + (i * 0x04)) = input[i];
        }
    }

	void WriteString(int address, char* string)
	{
		int FreeMem = 0x1D00000;
		int strlength = std::strlen(string);
		*(char**)FreeMem = string;
		char* StrBytes = PS3::ReadBytes(*(int*)FreeMem, strlength);
		PS3::WriteBytes(address, StrBytes, strlength);
	}

	struct readstr
	{
		char returnRead[100];
	}ReturnRead[1000];
	int strcount;

	char* ReadString(int address)
	{
		strcount ++;
		memset(&ReturnRead[strcount].returnRead[0], 0, sizeof(ReturnRead[strcount].returnRead));
		int strlength = 100;
		char* StrBytes = ReadBytes(address, strlength);
		for (int i = 0; i < strlength; i++)
		{
			if (StrBytes[i] != 0x00)
				ReturnRead[strcount].returnRead[i] = StrBytes[i];
			else
				break;
		}
		return ReturnRead[strcount].returnRead;
	}
};

extern "C" {
	int _sys_printf(const char *fmt, ...);
	int _sys_snprintf(char *, size_t, const char*, ...);
	int _sys_sprintf(char *, const char*, ...);
	void* _sys_malloc(size_t size);
	void _sys_free(void *ptr);
}

#define printf _sys_printf
#define snprintf _sys_snprintf
#define sprintf _sys_sprintf
#define malloc _sys_malloc
#define free _sys_free


//Hook Functions
int Memcpy(void* destination, const void* source, size_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);
	__dcbst(destination);
	__sync();
	__isync();
	return_to_user_prog(int);
}
void PatchInJump(int Address, int Destination)
{
	int FuncBytes[4];
	Destination = *(int*)Destination;
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);
	if (Destination & 0x8000) FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF);
	FuncBytes[2] = 0x7D6903A6;
	FuncBytes[3] = 0x4E800420;
	Memcpy((void*)Address, FuncBytes, 4 * 4);
}
void patcher(int Address, int Destination, bool Linked)
{
	int FuncBytes[4];
	Destination = *(int *)Destination;
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);
	if (Destination & 0x8000)
		FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF); // addi    %r11, %r11, dest&0xFFFF
	FuncBytes[2] = 0x7D6903A6; // mtctr    %r11
	FuncBytes[3] = 0x4E800420; // bctr
	if (Linked)
		FuncBytes[3] += 1; // bctrl
	Memcpy((void*)Address, FuncBytes, 4 * 4);
}
void hookFunctionStart(int Address, int saveStub, int Destination)
{
	saveStub = *(int*)saveStub;
	int BranchtoAddress = Address + (4 * 4);
	int StubData[8];
	StubData[0] = 0x3D600000 + ((BranchtoAddress >> 16) & 0xFFFF);
	if (BranchtoAddress & 0x8000) StubData[0] += 1;
	StubData[1] = 0x396B0000 + (BranchtoAddress & 0xFFFF);
	StubData[2] = 0x7D6903A6;
	Memcpy(&StubData[3], (void*)Address, 4 * 4);
	StubData[7] = 0x4E800420;
	Memcpy((void*)saveStub, StubData, 8 * 4);
	PatchInJump(Address, Destination);
}

//Create Thread
sys_ppu_thread_t id;
sys_ppu_thread_t create_thread(void(*entry)(uint64_t), int priority, size_t stacksize, const char* threadname)
{
	if (sys_ppu_thread_create(&id, entry, 0, priority, stacksize, 0, threadname) != CELL_OK)
	{
		console_write("Thread creation failed\n");
	}
	else
	{
		console_write("Thread created\n");
	}
	return id;
}

// Reverses a string 'str' of length 'len' 
void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

// Converts a given integer x to string str[].  
// d is the number of digits required in the output.  
// If d is more than the number of digits in x,  
// then 0s are added at the beginning. 
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If number of digits required is more, then 
	// add 0s at the beginning 
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string. 
void ftoa(float n, char* res, int afterpoint)
{

	// Extract integer part 
	int ipart = (int)n;

	// Extract floating part 
	float fpart = n - (float)ipart;

	// convert integer part to string 
	int i = intToStr(ipart, res, 0);

	if (n < 1 && n >= 0) {
		res[i] = '0'; // add 0
		i++;
	}

	// check for display option after point 
	if (afterpoint != 0) {
		res[i] = '.'; // add dot 

		// Get the value of fraction part upto given no. 
		// of points after dot. The third parameter  
		// is needed to handle cases like 233.007 
		fpart = fpart * pow(10.0f, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

#include <sys/return_code.h>
#include <sysutil/sysutil_oskdialog.h>
#include <sysutil/sysutil_msgdialog.h>


namespace Dialog
{
	bool Finished = false;
	bool YESNO = false;

	enum {
		MODE_IDLE = 0,
		MODE_ERRORCODE_DIALOG_TEST,
		MODE_STRING_OK,
		MODE_STRING_YESNO,
		MODE_STRING_DIALOG_3,
		MODE_STRING_DIALOG_4,
		MODE_STRING_DIALOG_5,
		MODE_CHECK_PROGRESSBAR_1,
		MODE_RUNNING,
		MODE_CHECK_TIMEOUT,
		MODE_TIMEOUT_NEXT,
		MODE_EXIT
	};

	static int msgdialog_mode = MODE_IDLE;

	static void cb_dialogText1(int button_type, void *userdata)
	{
		switch (button_type) {

		case CELL_MSGDIALOG_BUTTON_OK:
			msgdialog_mode = MODE_EXIT;
			break;
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
			msgdialog_mode = MODE_EXIT;
			break;

		default:
			msgdialog_mode = MODE_EXIT;
			break;
		}
	}
	static void cb_dialogText2(int button_type, void *userdata)
	{
		switch (button_type) {

		case CELL_MSGDIALOG_BUTTON_YES:
			YESNO = true;
			msgdialog_mode = MODE_EXIT;
			break;

		case CELL_MSGDIALOG_BUTTON_NO:
			msgdialog_mode = MODE_EXIT;
			break;

		case CELL_MSGDIALOG_BUTTON_ESCAPE:
			msgdialog_mode = MODE_EXIT;
			break;

		default:
			break;
		}
	}


	void ShowYESNO(char* msg)
	{
		int ret = 0;
		unsigned int type = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL
			| CELL_MSGDIALOG_TYPE_BG_INVISIBLE
			| CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO
			| CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
			| CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_YES;


		switch (msgdialog_mode)
		{
		case MODE_IDLE:
			break;

		case MODE_STRING_YESNO:
			cellMsgDialogOpen2(type, msg, cb_dialogText2, NULL, NULL);
			msgdialog_mode = MODE_RUNNING;
			break;

		case MODE_EXIT:
			cellMsgDialogClose((float)1);
			break;

		default:
			break;

		}


	}
	void Show(char* msg)
	{

		unsigned int type = CELL_MSGDIALOG_TYPE_SE_TYPE_ERROR
			| CELL_MSGDIALOG_TYPE_BG_INVISIBLE
			| CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK
			| CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
			| CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK;


		switch (msgdialog_mode)
		{
		case MODE_IDLE:
			break;

		case MODE_STRING_OK:
			cellMsgDialogOpen2(type, msg, cb_dialogText1, NULL, NULL);
			msgdialog_mode = MODE_EXIT;
			break;

		case MODE_EXIT:
			cellMsgDialogClose((float)5);
			break;

		default:
			break;

		}

		while (Dialog::msgdialog_mode != Dialog::MODE_EXIT) {}  //wait for the dialog to end :P
	}

	void End()
	{
		Finished = false;
		YESNO = false;
	}


}



namespace Keyboard
{
	enum {
		MODE_IDLE = 0,
		MODE_OPEN,
		MODE_RUNNING,
		MODE_CLOSE,
		MODE_ENTERED,
		MODE_CANCELED,
		SET_ABORT_TIMER,
		CHANGE_PANEL_MODE,
		CHANGE_SCALE,
		SET_CALLBACK,
		MODE_EXIT,
		START_DIALOG_TYPE,
		START_SEPARATE_TYPE_1,
		START_SEPARATE_TYPE_2,
	};
	static int oskdialog_mode = MODE_IDLE;

	int getkbLen(char* str)
	{
		int nullCount = 0;
		int i = 0; //num of chars..
		for (i = 0; i < 64; i++)
		{
			if (nullCount == 2) { break; }
			if (*(str + i) == 0x00) { nullCount++; }
			else { nullCount = 0; }
		}
		return i;
	}
	void makekbStr(char* str, char* dest, int len)
	{
		int nulls = 0;
		for (int i = 0; i < len; i++)
		{
			if (*(str + i) == 0x00) { nulls++; }
			else { *(dest + i - nulls) = *(str + i); }
		}
		*(dest + len + 1 - nulls) = 0x00;  //make sure its nulled...
	}

	static void sysutil_callback(uint64_t status, uint64_t param, void *userdata)
	{
		(void)param;
		(void)userdata;
		int ret = 0;

		switch (status) {
		case CELL_SYSUTIL_OSKDIALOG_LOADED:
			break;
		case CELL_SYSUTIL_OSKDIALOG_FINISHED:
			oskdialog_mode = MODE_CLOSE;
			break;
		case CELL_SYSUTIL_OSKDIALOG_UNLOADED:
			break;
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			oskdialog_mode = MODE_EXIT;
			break;
		case CELL_SYSUTIL_DRAWING_BEGIN:
			break;
		case CELL_SYSUTIL_DRAWING_END:
			break;
		case CELL_SYSUTIL_OSKDIALOG_INPUT_ENTERED:
			oskdialog_mode = MODE_ENTERED;
			break;
		case CELL_SYSUTIL_OSKDIALOG_INPUT_CANCELED:
			oskdialog_mode = MODE_CANCELED;
			break;
		case CELL_SYSUTIL_OSKDIALOG_INPUT_DEVICE_CHANGED:
			break;
		case CELL_SYSUTIL_OSKDIALOG_DISPLAY_CHANGED:
			break;
		case CELL_SYSUTIL_SYSTEM_MENU_CLOSE:

			break;
		case CELL_SYSUTIL_SYSTEM_MENU_OPEN:
			break;
		default:
			break;
		}
	}
	int keyboard(char* dest, char* INIT_TEXT, char* MESSAGE)
	{


#pragma region Declarations

		int ret;
		CellOskDialogInputFieldInfo inputFieldInfo;
		inputFieldInfo.message = (uint16_t*)MESSAGE;
		inputFieldInfo.init_text = (uint16_t*)INIT_TEXT;
		inputFieldInfo.limit_length = CELL_OSKDIALOG_STRING_SIZE;


		CellOskDialogCallbackReturnParam OutputInfo;
		OutputInfo.result = CELL_OSKDIALOG_INPUT_FIELD_RESULT_OK;
		OutputInfo.numCharsResultString = 32;

		uint16_t Result_Text_Buffer[CELL_OSKDIALOG_STRING_SIZE + 1];
		OutputInfo.pResultString = Result_Text_Buffer;

		ret = cellOskDialogSetKeyLayoutOption(CELL_OSKDIALOG_10KEY_PANEL | CELL_OSKDIALOG_FULLKEY_PANEL);


		CellOskDialogPoint pos;
		pos.x = 0.0;  pos.y = 0.0;
		int32_t LayoutMode = CELL_OSKDIALOG_LAYOUTMODE_X_ALIGN_CENTER | CELL_OSKDIALOG_LAYOUTMODE_Y_ALIGN_TOP;
		ret = cellOskDialogSetLayoutMode(LayoutMode);


		CellOskDialogParam dialogParam;
		dialogParam.allowOskPanelFlg = CELL_OSKDIALOG_PANELMODE_ENGLISH;
		/*E Panel to display first */
		dialogParam.firstViewPanel = CELL_OSKDIALOG_PANELMODE_ALPHABET;
		/* E Initial display position of the on-screen keyboard dialog */
		dialogParam.controlPoint = pos;
		/*E Prohibited operation flag(s) (ex. CELL_OSKDIALOG_NO_SPACE) */
		dialogParam.prohibitFlgs = 0;


#pragma endregion

		sys_timer_usleep(16 * 1000);
		ret = cellSysutilCheckCallback();


		if (oskdialog_mode == MODE_OPEN)
		{
			if (cellSysutilRegisterCallback(0, sysutil_callback, NULL) != 0) { console_write("\nCouldn't register the keyboard !\n"); }
			ret = cellOskDialogLoadAsync(SYS_MEMORY_CONTAINER_ID_INVALID, &dialogParam, &inputFieldInfo);
			oskdialog_mode = MODE_RUNNING;
		}

		if (oskdialog_mode == MODE_ENTERED)
		{
			ret = cellOskDialogGetInputText(&OutputInfo);
			oskdialog_mode = MODE_RUNNING;
		}
		if (oskdialog_mode == MODE_CLOSE)
		{
			ret = cellOskDialogUnloadAsync(&OutputInfo);

			int strLen = getkbLen((char*)(*(&OutputInfo.pResultString)));
			makekbStr((char*)(*(&OutputInfo.pResultString)), dest, strLen);
			inputFieldInfo.init_text = (uint16_t*)INIT_TEXT;
			if (cellSysutilUnregisterCallback(0) != 0) { console_write("\nCouldn't unload the keyboard !\n"); }
			oskdialog_mode = MODE_EXIT;
		}

		return 0;
	}


}